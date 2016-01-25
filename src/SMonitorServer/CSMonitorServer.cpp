/*
This file is part of SMonitor.
Copyright  2015~2016  by: rayx  email rayx.cn@gmail.com
SMonitor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SMonitor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "CSMonitorServer.h"
#include "CTitleWidget.h"
#include "CSideBarWidget.h"
#include "CSystemTrayIcon.h"
#include "CMessageEventMediator.h"
#include "CSoftUpgradeWidget.h"
#include "CHelpInfoWidget.h"
#include "CStatisticsWidget.h"
#include "CStatusWidget.h"
#include "databasemanager.h"
#include "CMessageDataMediator.h"
#include "CMisc.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStackedWidget>
#include <QFile>
#include <QMouseEvent>
#include <QDir>

CSMonitorServer::CSMonitorServer(QWidget *parent)
	: QFrame(parent)
	, m_bServerIsStarted(false)
{
	//ui.setupUi(this);
	setWindowIcon(QIcon(":/Resources/normal.png"));
	NetClass->m_pMessageEventMediator = m_pMessageEventMediator = new CMessageEventMediator();
	NetClass->m_pNetClt = m_pIOCPnet = new CIOCPNet(m_pMessageEventMediator);
	
	setObjectName("mainFrame");
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setWidgetStyleSheet(":/Resources/blue.css");
	setMinimumSize(QSize(900, 600));

	m_titleWidget = new CTitleWidget(this);
	m_titleWidget->setParent(this);
	m_titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_titleWidget->setTitle(QString::fromLocal8Bit("SMonitor软件安装/升级管理系统 - 服务器(正式版20150320)"));
	connect(m_titleWidget, &CTitleWidget::minimumzed, this, &CSMonitorServer::setMinimumed);
	connect(m_titleWidget, &CTitleWidget::maximumzed, this, &CSMonitorServer::setMaximumed);
	connect(m_titleWidget, &CTitleWidget::closed, this, &CSMonitorServer::setMinimumed);

	m_trayIcon = new CSystemTrayIcon(this);
	m_trayIcon->setHidden(true);
	m_trayIcon->show();

	connect(m_trayIcon, &CSystemTrayIcon::activated, this, &CSMonitorServer::activedByTrayIcon);
	connect(m_trayIcon, &CSystemTrayIcon::hideOrShowWidget, this, &CSMonitorServer::hideOrShow);
	connect(m_trayIcon, &CSystemTrayIcon::quitApplication, this, &CSMonitorServer::close);
	connect(m_trayIcon, &CSystemTrayIcon::aboutApplication, this, &CSMonitorServer::about);

	m_stackedWidget = new QStackedWidget(this);
	m_sideBarWidget = new CSideBarWidget(this);
	initTabWidget();

	m_sideBarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//m_sideBarWidget->setMinimumHeight(300);
	connect(m_sideBarWidget, &CSideBarWidget::currentIndexChanged, 
		m_stackedWidget, &QStackedWidget::setCurrentIndex);

	QHBoxLayout* hBoxLayout = new QHBoxLayout;
	hBoxLayout->setMargin(0);
	hBoxLayout->setSpacing(0);
	hBoxLayout->addWidget(m_sideBarWidget);
	hBoxLayout->addWidget(m_stackedWidget);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(m_titleWidget);
	mainLayout->addLayout(hBoxLayout);

	//QPushButton *pPushStartBtn = new QPushButton(QString::fromLocal8Bit("启动服务器"), this);
	//connect(pPushStartBtn, &QPushButton::pressed, this, &CSMonitorServer::startServer);

	//QPushButton *pPushEndBtn = new QPushButton(QString::fromLocal8Bit("关闭服务器"), this);
	//connect(pPushEndBtn, &QPushButton::pressed, this, &CSMonitorServer::endServer);

	//QHBoxLayout *pHBoxLayout = new QHBoxLayout(this);
	//pHBoxLayout->addWidget(pPushStartBtn);
	//pHBoxLayout->addWidget(pPushEndBtn);

	//setLayout(pHBoxLayout);

	startServer();
	showMaximized();
}

CSMonitorServer::~CSMonitorServer()
{
}

void CSMonitorServer::initTabWidget()
{
	m_sideBarWidget->addTab(QString::fromLocal8Bit("升级管理"), QChar(0xf0b1));
	m_homePageWidget = new CSoftUpgradeWidget(this);
	m_stackedWidget->addWidget(m_homePageWidget);

	m_sideBarWidget->addTab(QString::fromLocal8Bit("软件统计"), QChar(0xf0c2));
	m_statisticsWidget = new CStatisticsWidget(this);
	m_stackedWidget->addWidget(m_statisticsWidget);

	m_sideBarWidget->addTab(QString::fromLocal8Bit("服务状态"), QChar(0xf109));
	m_statusWidget = new CStatusWidget(this);
	m_stackedWidget->addWidget(m_statusWidget);

	m_sideBarWidget->addTab(QString::fromLocal8Bit("帮助"), QChar(0xf0fd));
	m_helpPageWidget = new CHelpInfoWidget(this);
	m_stackedWidget->addWidget(m_helpPageWidget);

	connect(m_homePageWidget, &CSoftUpgradeWidget::datasaved, m_statisticsWidget, &CStatisticsWidget::initData);
	connect(m_homePageWidget, &CSoftUpgradeWidget::itemAdded, m_statisticsWidget, &CStatisticsWidget::addItem);
	connect(m_homePageWidget, &CSoftUpgradeWidget::itemRemoved, m_statisticsWidget, &CStatisticsWidget::removeItem);
	connect(DatabaseManager::Instance(), &DatabaseManager::sig_updateAllNumData, m_statisticsWidget, &CStatisticsWidget::updateAllNumData);
	connect(DatabaseManager::Instance(), &DatabaseManager::sig_initData, m_statisticsWidget, &CStatisticsWidget::initData);

	m_sideBarWidget->setCurrentIndex(0);

	NetClass->m_pSoftUpgradeWidget = m_homePageWidget;
}

void CSMonitorServer::startServer()
{
	if(m_bServerIsStarted || NULL == m_pIOCPnet)
		return;

	ServerLogger->AddLog("启动服务器中...");

	QString dataPath = QApplication::applicationDirPath() + "/ServerLogger/CommLog/";
	//dataPath = "d:/ServerLogger/CommLog"; //yhb

	QDir dir;
	if(!dir.mkpath(dataPath))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("日志目录[%1]创建失败，请确认目标磁盘是否空间已满").arg(dataPath),
			QString::fromLocal8Bit("确定"));
		return;
	}

	m_pIOCPnet->SetSvrPort(MessageDataMediator->m_nListenPort);
	if(!m_pIOCPnet->StartLogger(dataPath.toLocal8Bit().constData()))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("无法启动日志功能，服务器启动失败"),
			QString::fromLocal8Bit("确定"));

		ServerLogger->AddLog("启动服务器失败, m_IOCPnet.StartLogger 启动失败");
		return;
	}

	m_pIOCPnet->SetMaxConnectClientsNum(MessageDataMediator->m_nMaxClientsNum); //设置客户端并发连接数
	if(!m_pIOCPnet->StartServer())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("服务器启动失败"),
			QString::fromLocal8Bit("确定"));

		ServerLogger->AddLog("启动服务器失败, m_IOCPnet.StartServer 启动失败");
		return;
	}

	ServerLogger->AddLog("服务器启动成功");


	//设置服务器开启状态
	m_bServerIsStarted = true;
}

void CSMonitorServer::endServer()
{
	if(!m_bServerIsStarted || NULL == m_pIOCPnet)
		return;

	m_pIOCPnet->StopServer();
	m_pIOCPnet->ClearClientSocketMap();
	ServerLogger->AddLog("成功关闭服务器");

	m_bServerIsStarted = false;
}

void CSMonitorServer::closeEvent(QCloseEvent *)
{
	ServerLogger->AddLog("服务器软件关闭");
	QStringList exeList;
	exeList << "SMonitorServer.exe";
	CMisc::KillAllProcessesByTaskKiller(exeList);

 	//endServer();
 	this->close();
 	WSACleanup(); 
 	qApp->quit();
}

void CSMonitorServer::setWidgetStyleSheet(const QString& style)
{
	QFile file(style);
	file.open(QFile::ReadOnly);
	qApp->setStyleSheet(QLatin1String(file.readAll()));
	qApp->setPalette(QPalette(QColor("#F0F0F0")));
}

void CSMonitorServer::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(event->pos().y() < 27)
			m_bPressed = true;
	}

	//窗口移动距离
	m_movePoint = event->globalPos() - frameGeometry().topLeft();
}

void CSMonitorServer::mouseMoveEvent(QMouseEvent *event)
{
	if(m_bPressed)
	{
		QPoint movePos = event->globalPos() - m_movePoint;
		int distance = movePos.manhattanLength();
		bool bMoved =  distance > QApplication::startDragDistance() ? true : false;
		if(bMoved)
		{
			this->move(movePos);
		}
	}
}

void CSMonitorServer::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_bPressed = false;
	}
}

void CSMonitorServer::setMinimumed()
{
	m_trayIcon->setHidden(false);
	this->showMinimized();
}

void CSMonitorServer::setMaximumed()
{
	if(this->isMaximized())
		this->showNormal();
	else
		this->showMaximized();
}

void CSMonitorServer::activedByTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger)
	{
		m_trayIcon->setHidden(true);
		showNormal();
		activateWindow();
		this->raise();
	}
}

void CSMonitorServer::hideOrShow()
{
	if(this->isHidden())
	{
		this->show();
		m_trayIcon->setHidden(true);
	}
	else
	{
		this->hide();
		m_trayIcon->setHidden(false);
	}
}

void CSMonitorServer::about()
{

}