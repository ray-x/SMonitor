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

#include "CSMonitorClient.h"
#include "CSystemTrayIcon.h"
#include "CTitleWidget.h"
#include "CSideBarWidget.h"
#include "CPushButton.h"
#include "CHomePageWidget.h"
#include "CSoftTabWidget.h"
#include "CSoftUpdateWidget.h"
#include "CClientLog.h"
#include "NetClt.h"
#include "Markup.h"
#include "stdafx.h"
#include "CHTTPDownloader.h"
#include "CMessageEventMediator.h"
#include "CMessageDataMediator.h"
#include "NetClass.h"
#include "CDownloadCM.h"
#include "CMisc.h"
#include "CLoginDialog.h"
#include "CHelpInfoWidget.h"
#include "CSoftManager.h"
#include "CSplashscreen.h"
#include "CLoginDialog.h"
#include "CSoftInstallResultSendTh.h"
#include "CSoftManager.h"
#include "CCategoryFactory.h"

#include <QDebug>
#include <QFrame>
#include <QApplication>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QFileSystemWatcher>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QLabel>
#include <QSettings>
#include <QMessageBox>
#include <QMouseEvent>
#include <QApplication>
#include <QSysInfo>
#include <QTextCodec>

CSMonitorClient::CSMonitorClient(QWidget *parent)
	: QFrame(parent)
	, m_bPressed(false)
{
	ClientLogger->AddLog(QString::fromLocal8Bit("客户端版本号【%1】 电话号码【%2】").arg(MessageDataMediator->m_strClientVersion).arg(MessageDataMediator->m_strNewId));

	//setWindowIcon(QIcon(":/Resources/normal.png"));
	setMinimumSize(QSize(900, 600));

 	m_pMessageEventMediator = NetClass->m_pMessageEventMediator;
 	m_pNetClient = NetClass->m_pNetClt;
	//connect(m_pMessageEventMediator, &CMessageEventMediator::sigServerHasGetSoftInstall, SoftInstallResultSendManager, &CSoftInstallResultSendThread::serverHasGetSoftInstall);
	//SoftInstallResultSendManager->Start(QApplication::applicationDirPath() + "/updateTask.xml");

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setObjectName("mainFrame");
	//setWidgetStyleSheet(":/Resources/blue.css");

	m_titleWidget = new CTitleWidget(this);
	m_titleWidget->setParent(this);
	//m_titleWidget->setCloseButtonHint(false);
	m_titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_titleWidget->setTitle(QString::fromLocal8Bit("SMonitor软件安装/升级管理系统(正式版)"));

	m_trayIcon = new CSystemTrayIcon(this);
	m_trayIcon->setHidden(true);
	m_trayIcon->show();
	
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

	connect(m_trayIcon, &CSystemTrayIcon::activated, this, &CSMonitorClient::activedByTrayIcon);
	connect(m_trayIcon, &CSystemTrayIcon::hideOrShowWidget, this, &CSMonitorClient::hideOrShow);
	connect(m_trayIcon, &CSystemTrayIcon::quitApplication, this, &CSMonitorClient::close);
	//connect(m_trayIcon, &CSystemTrayIcon::aboutApplication, this, &CSMonitorClient::about);
	connect(m_trayIcon, &CSystemTrayIcon::messageClicked, this, &CSMonitorClient::updateArrived);
	connect(m_titleWidget, &CTitleWidget::minimumzed, this, &CSMonitorClient::setMinimumed);
	connect(m_titleWidget, &CTitleWidget::closed, this, &CSMonitorClient::setMinimumed);
	connect(SoftManager, &CSoftManager::quitApplication, this, &CSMonitorClient::close);

	//首先需要加载本地保存的数据
	CategoryFactory->LoadLocalData();

	//加载缓存
	CategoryFactory->CheckUpgradeData();

	connect(CategoryFactory, &CCategoryFactory::upgradeNotify, m_trayIcon, &CSystemTrayIcon::setState);
	//如果服务器断开，则加载上一次下载的服务器数据
	//if(!NetClass->m_pNetClt->m_bIsOnLine){
	//	CategoryFactory->CheckUpgradeData();
	//	ClientLogger->AddLog(QString::fromLocal8Bit("客户端连接服务器异常，加载前一次下载至本地的配置数据"));
	//}
	connect(SoftManager, &CSoftManager::InstallFinished, CategoryFactory, &CCategoryFactory::InstallFinished);
	
	//CXmlParse::Instance()->LoadLocalData();
	//connect(CXmlParse::Instance(), &CXmlParse::upgradeNotify, m_trayIcon, &CSystemTrayIcon::setState);
	//connect(CXmlParse::Instance(), &CXmlParse::upgradeFinished, m_trayIcon, &CSystemTrayIcon::setState);

	//CHTTPDownloader* gHTTPDownload = new CHTTPDownloader(this);
	//gHTTPDownload->downloadFileSync(QString::fromLocal8Bit("http://127.0.0.1:8000/83001 - 副本.zip"), "D:\\test\\3.jpg", 0, true);

	//ClientLogger->AddLog(QString::fromLocal8Bit("客户端启动"));
	connect(m_pMessageEventMediator, &CMessageEventMediator::sigGetXml, this, &CSMonitorClient::setServerXmlUrl);

	//半小时申请一次
	m_reqXMLTimerId = startTimer(60000 * 30);

	SoftManager->Start();

	downloadUpdateXmlFromServer();
	downloadHelpInfoXmlFromServer();
	checkUpdate();
}

CSMonitorClient::~CSMonitorClient()
{
	qDebug() << "~CSMonitorClient()";
	DELETE_POINTER(m_trayIcon);
	DELETE_POINTER(m_titleWidget);
	DELETE_POINTER(m_stackedWidget);
	DELETE_POINTER(m_sideBarWidget);
	//DELETE_POINTER(m_mainFrame);
}

void CSMonitorClient::init()
{
	
}

void CSMonitorClient::initTabWidget()
{
	m_sideBarWidget->addTab(QString::fromLocal8Bit("首页"), QChar(0xf015));
	m_homePageWidget = new CHomePageWidget(this);
	connect(m_homePageWidget, &CHomePageWidget::NotifyOperation, this, &CSMonitorClient::downloadStarted);
	m_stackedWidget->addWidget(m_homePageWidget);
	NetClass->m_pHomePageWidget = m_homePageWidget;

	m_sideBarWidget->addTab(QString::fromLocal8Bit("软件列表"), QChar(0xf085));
	m_softListWidget = new CSoftTabWidget(this);
	m_stackedWidget->addWidget(m_softListWidget);

	m_sideBarWidget->addTab(QString::fromLocal8Bit("软件更新"), QChar(0xf021));
	m_updatePageWidget = new CSoftUpdateWidget(this);
	connect(m_updatePageWidget, &CSoftUpdateWidget::RowCountChanged, m_sideBarWidget, &CSideBarWidget::TipsNumberChanged);
	connect(m_updatePageWidget, &CSoftUpdateWidget::upgradeNotify, m_trayIcon, &CSystemTrayIcon::setState);
	m_stackedWidget->addWidget(m_updatePageWidget);

	m_sideBarWidget->addTab(QString::fromLocal8Bit("帮助"), QChar(0xf0fd));
	m_helpPageWidget = new CHelpInfoWidget(this);
	m_stackedWidget->addWidget(m_helpPageWidget);
	m_sideBarWidget->setCurrentIndex(0);

	connect(this, &CSMonitorClient::sig_showIndependentItem, m_homePageWidget, &CHomePageWidget::showIndependentItem);
	connect(this, &CSMonitorClient::sig_showIndependentItem, m_updatePageWidget, &CSoftUpdateWidget::showIndependentItem);
}

void CSMonitorClient::setWidgetStyleSheet(const QString& style)
{
	QFile file(style);
	file.open(QFile::ReadOnly);
	qApp->setStyleSheet(QLatin1String(file.readAll()));
	qApp->setPalette(QPalette(QColor("#F0F0F0")));
}

void CSMonitorClient::downloadUpdateXmlFromServer()
{
	if(!MessageDataMediator->m_strSvrUpdateXmlUrl.isEmpty())
	{
		QString path = QApplication::applicationDirPath() + "/Svr.data";
		CHTTPDownloader* gHTTPDownload = new CHTTPDownloader(this);
		gHTTPDownload->downloadFileSync(MessageDataMediator->m_strSvrUpdateXmlUrl, path, 0, true);
		connect(gHTTPDownload, &CHTTPDownloader::sig_DownFinished, CategoryFactory, &CCategoryFactory::CheckUpgradeData);
		connect(gHTTPDownload, &CHTTPDownloader::sig_ErrorOccur, this, &CSMonitorClient::downloadUpdateXMLErrorOccur);
	}
	else
	{
		MessageDataMediator->m_bClientStartOver = true;
		//NetClass->m_pNetClt->SendTextMsg();
		setSoftInfoCanUpload();
	}
}

void CSMonitorClient::downloadHelpInfoXmlFromServer()
{
	if(!MessageDataMediator->m_strHelpInfoXmlUrl.isEmpty())
	{
		QString path = QApplication::applicationDirPath() + "/Help.data";
		CHTTPDownloader* gHTTPDownload = new CHTTPDownloader(this);
		gHTTPDownload->downloadFileSync(MessageDataMediator->m_strHelpInfoXmlUrl, path, 0, true);
		connect(gHTTPDownload, &CHTTPDownloader::sig_DownFinished, m_helpPageWidget, &CHelpInfoWidget::updateHelpInfo);
		connect(gHTTPDownload, &CHTTPDownloader::sig_ErrorOccur, this, &CSMonitorClient::downloadHelpInfoXMLErrorOccur);
	}
}

void CSMonitorClient::checkUpdate()
{
	if(!MessageDataMediator->m_strVersionCheckFile.isEmpty())
	{
		QString path = QApplication::applicationDirPath() + "/autoCheckVersionFile.tmp";
		CHTTPDownloader* gHTTPDownload = new CHTTPDownloader(this);
		gHTTPDownload->downloadFileSync(MessageDataMediator->m_strVersionCheckFile, path, 0, true);
		connect(gHTTPDownload, &CHTTPDownloader::sig_DownFinished, this, &CSMonitorClient::slot_checkFile);
		connect(gHTTPDownload, &CHTTPDownloader::sig_ErrorOccur, this, &CSMonitorClient::downloadCheckFileErrorOccur);
	}
}

void CSMonitorClient::setServerXmlUrl(const QString& strUpdateXmlPath, const QString& strHelpInfoXmlPath, const QString& strVersionCheckFile)
{
	if(0 != MessageDataMediator->m_strSvrUpdateXmlUrl.compare(strUpdateXmlPath, Qt::CaseInsensitive))
	{
		QSettings settings(HKCU, QSettings::NativeFormat);
		settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
		settings.setValue("XmlUrl/UpdateXml", strUpdateXmlPath);

		if(MessageDataMediator->m_strSvrUpdateXmlUrl.isEmpty())
		{
			MessageDataMediator->m_strSvrUpdateXmlUrl = strUpdateXmlPath;
			downloadUpdateXmlFromServer();
		}
		else
		{
			MessageDataMediator->m_strSvrUpdateXmlUrl = strUpdateXmlPath;
			if(MessageDataMediator->m_bTextMode)
			{
				downloadUpdateXmlFromServer();
			}
			else
			{
				setSoftInfoCanUpload();
			}
		}
	}
	else
	{
		setSoftInfoCanUpload();
	}

	if(0 != MessageDataMediator->m_strHelpInfoXmlUrl.compare(strHelpInfoXmlPath, Qt::CaseInsensitive))
	{
		QSettings settings(HKCU, QSettings::NativeFormat);
		settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
		settings.setValue("XmlUrl/HelpInfoXml", strHelpInfoXmlPath);

		MessageDataMediator->m_strHelpInfoXmlUrl = strHelpInfoXmlPath;
		if(!MessageDataMediator->m_strHelpInfoXmlUrl.isEmpty())
		{
			downloadHelpInfoXmlFromServer();
		}
	}

	if(0 != MessageDataMediator->m_strVersionCheckFile.compare(strVersionCheckFile, Qt::CaseInsensitive))
	{
		QSettings settings(HKCU, QSettings::NativeFormat);
		settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
		settings.setValue("XmlUrl/VersionCheckFile", strVersionCheckFile);
		MessageDataMediator->m_strVersionCheckFile = strVersionCheckFile;
	}
}

void CSMonitorClient::downloadUpdateXMLErrorOccur(const QString& errMsg)
{
	ClientLogger->AddLog(QString::fromLocal8Bit("下载升级XML失败，错误信息[%1]").arg(errMsg));
	//CategoryFactory->CheckUpgradeData();
}


void CSMonitorClient::downloadHelpInfoXMLErrorOccur(const QString& errMsg)
{
	ClientLogger->AddLog(QString::fromLocal8Bit("下载帮助XML失败，错误信息[%1]").arg(errMsg));
}

void CSMonitorClient::downloadCheckFileErrorOccur(const QString& errMsg)
{
	ClientLogger->AddLog(QString::fromLocal8Bit("下载版本检测文件失败，错误信息[%1]").arg(errMsg));
	QString path = QApplication::applicationDirPath() + "/autoCheckVersionFile.tmp";
	QFile::remove(path);
}

void CSMonitorClient::activedByTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger)
	{
		m_trayIcon->setHidden(true);
		showNormal();
		activateWindow();
		this->raise();
	}
}

void CSMonitorClient::maximized()
{
	if(this->isMaximized())
		this->showNormal();
	else
		this->showMaximized();
}

void CSMonitorClient::hideOrShow()
{
	if(this->isMinimized())
	{
		this->showNormal();
		m_trayIcon->setHidden(true);
	}
	else
	{
		this->showMinimized();
		m_trayIcon->setHidden(false);
	}
}

void CSMonitorClient::setMinimumed()
{
	m_trayIcon->setHidden(false);
	this->showMinimized();
}

void CSMonitorClient::updateArrived()
{
	show();
	m_sideBarWidget->setCurrentIndex(2);
}

//void CSMonitorClient::about()
//{
//	
//}

void CSMonitorClient::closeEvent(QCloseEvent *)
{
	SoftManager->StopDownload();

	if(NULL != m_pNetClient)
		m_pNetClient->Stop();

	WSACleanup();

	this->close();

	CMisc::KillSelf();

	//qApp->quit();
}

QSize CSMonitorClient::sizeHint()const
{
	return QSize(900, 600);
}

void CSMonitorClient::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(event->pos().y() < 28)
			m_bPressed = true;
	}

	//窗口移动距离
	m_movePoint = event->globalPos() - frameGeometry().topLeft();
}

void CSMonitorClient::mouseMoveEvent(QMouseEvent *event)
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

void CSMonitorClient::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_bPressed = false;
	}
}
//每隔半小时会重新向文件服务器获取配置xml以更新软件数据
void CSMonitorClient::timerEvent(QTimerEvent *event)
{
	if(m_reqXMLTimerId == event->timerId())
	{
		downloadUpdateXmlFromServer();
		downloadHelpInfoXmlFromServer();
		checkUpdate();
	}
}

void CSMonitorClient::downloadStarted(SMonitor::Type type, const QString& category, const QString& name)
{
	switch(type)
	{
	case SMonitor::Downloading:
		{
			m_sideBarWidget->setCurrentIndex(1);
			m_softListWidget->setTab(category);
		}
		break;
	case SMonitor::Upgrade:
		m_sideBarWidget->setCurrentIndex(2);
		break;
	default:
		break;
	}
}

void CSMonitorClient::slot_checkFile()
{
	ClientLogger->AddLog(QString::fromLocal8Bit("下载版本比较文件成功"));
	QString path = QApplication::applicationDirPath() + "/autoCheckVersionFile.tmp";
	QString strSvrVersion;
	QFile file;
	file.setFileName(path);
	if(file.open(QIODevice::ReadWrite))
	{
		//设置文件共享打开
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::ExeOther | QFileDevice::WriteOther);
		strSvrVersion = file.readAll().constData();
		strSvrVersion = strSvrVersion.trimmed();
		file.close();
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("版本比较文件无法打开[%1]").arg(path));
		QFile::remove(path);
		return;
	}

	QFile::remove(path);

	ClientLogger->AddLog(QString::fromLocal8Bit("最新的软件版本号为[%1] 本地软件版本号为[%2]").arg(strSvrVersion).arg(MessageDataMediator->m_strClientVersion));

	bool bNeedUpdate = CMisc::IsNeedUpdate(MessageDataMediator->m_strClientVersion, strSvrVersion);
	emit sig_showIndependentItem("SMonitorClient", strSvrVersion, bNeedUpdate);
}

void CSMonitorClient::setSoftInfoCanUpload()
{
	if(MessageDataMediator->m_bIsCanConnectedTime)
	{
		bool bRecved = false;
		if(MessageDataMediator->getConnectMsg(EMSG_SOFTUPLOAD, bRecved) && !bRecved)
		{
			NetClass->m_pNetClt->SendMsg(EMSG_SOFTUPLOAD, true);
		}

		MessageDataMediator->setConnectMsgMapProperty(EMSG_REQUESTXML, true);
	}
}