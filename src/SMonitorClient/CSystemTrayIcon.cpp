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

#include "CSystemTrayIcon.h"
#include "common.h"
#include "CMisc.h"
#include "CMessageDataMediator.h"
#include "CHTTPDownloader.h"
#include "CClientLog.h"

#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QMessageBox>

CSystemTrayIcon::CSystemTrayIcon(QObject *parent)
	: QSystemTrayIcon(parent)
	, m_waitid(0)
	, m_pVersionFileDownloader(NULL)
{
	setToolTip(QString::fromLocal8Bit("SMonitor�����װ/��������ϵͳ"));
	m_curIconType = SMonitor::UnInstall;
	setState(SMonitor::Normal);

	m_TrayIconMenu = new QMenu;

	m_hideShowAct = new QAction(QIcon(":/Resources/hidden.png"), QString::fromLocal8Bit("����"), m_TrayIconMenu);
	//m_aboutAct = new QAction(QIcon(":/Resources/help.png"), QString::fromLocal8Bit("����"), m_TrayIconMenu);
	m_upgradeAct = new QAction(QIcon(), QString::fromLocal8Bit("�����ͻ���"), m_TrayIconMenu);
	m_exitAct = new QAction(QIcon(":/Resources/exit.png"), QString::fromLocal8Bit("�˳�"), m_TrayIconMenu);

	m_TrayIconMenu->addAction(m_hideShowAct);
	//m_TrayIconMenu->addAction(m_aboutAct);
	m_TrayIconMenu->addSeparator();
	m_TrayIconMenu->addAction(m_upgradeAct);
	m_TrayIconMenu->addSeparator();
	m_TrayIconMenu->addAction(m_exitAct);

	this->setContextMenu(m_TrayIconMenu);

	connect(m_hideShowAct, &QAction::triggered, this, &CSystemTrayIcon::hideOrShowWidget);
	//connect(m_aboutAct, &QAction::triggered, this, &CSystemTrayIcon::aboutApplication);
	connect(m_exitAct, &QAction::triggered, this, &CSystemTrayIcon::quitApplication);
	connect(m_upgradeAct, &QAction::triggered, this, &CSystemTrayIcon::selfUpgrade);
}

CSystemTrayIcon::~CSystemTrayIcon()
{
	qDebug() << "~CSystemTrayIcon()";
	DELETE_POINTER(m_hideShowAct);
	//DELETE_POINTER(m_aboutAct);
	DELETE_POINTER(m_exitAct);
	DELETE_POINTER(m_TrayIconMenu);
}

void CSystemTrayIcon::setState(SMonitor::Type state)
{
	if(m_curIconType == state)
		return;

	m_curIconType = state;

	switch(state)
	{
	case SMonitor::Normal:
		this->setIcon(QIcon(":/Resources/normal.png"));
		break;
	case SMonitor::Upgrade:
	case SMonitor::Installing:
		{
			this->setIcon(QIcon(":/Resources/update.png"));
			showMessage(QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("������������������߰汾������Բ鿴���顣"),
				QSystemTrayIcon::Information, 5000);
		}
		break;
	default:
		break;
	}
}

void CSystemTrayIcon::setHidden(bool hidden)
{
	if(!hidden)
	{
		m_hideShowAct->setText(QString::fromLocal8Bit("��ʾ"));
		m_hideShowAct->setIcon(QIcon(":/Resources/show.png"));
	}
	else
	{
		m_hideShowAct->setText(QString::fromLocal8Bit("����"));
		m_hideShowAct->setIcon(QIcon(":/Resources/hidden.png"));
	}
}

//hb �Զ�����
void CSystemTrayIcon::selfUpgrade()
{
	if(MessageDataMediator->m_strVersionCheckFile.isEmpty())
	{
		QMessageBox::warning(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�ͻ��˰汾�������°汾��������£�"));
		return;
	}

	closeDownload();
	QString path = QApplication::applicationDirPath() + "/checkVersionFile.tmp";
	m_pVersionFileDownloader = new CHTTPDownloader();
	m_pVersionFileDownloader->downloadFileSync(MessageDataMediator->m_strVersionCheckFile, path, 0, true);
	connect(m_pVersionFileDownloader, &CHTTPDownloader::sig_DownFinished, this, &CSystemTrayIcon::downloadSuccess);
	connect(m_pVersionFileDownloader, &CHTTPDownloader::sig_ErrorOccur, this, &CSystemTrayIcon::downloadFileErrorOccur);

	ClientLogger->AddLog(QString::fromLocal8Bit("�������, ���Ի�ȡ�汾�Ƚ��ļ�[%1]").arg(MessageDataMediator->m_strVersionCheckFile));
}

void CSystemTrayIcon::downloadFileErrorOccur(const QString& errMsg)
{
	ClientLogger->AddLog(QString::fromLocal8Bit("���ذ汾�Ƚ��ļ�ʧ��, ������Ϣ[%1]").arg(errMsg));
	killTimer(m_waitid);
	QMessageBox::warning(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�汾��Ϣ��ȡʧ�ܣ����Ժ�����"));
	QString path = QApplication::applicationDirPath() + "/checkVersionFile.tmp";
	QFile::remove(path);
}

void CSystemTrayIcon::downloadSuccess()
{
	killTimer(m_waitid);
	ClientLogger->AddLog(QString::fromLocal8Bit("���ذ汾�Ƚ��ļ��ɹ�"));
	QString path = QApplication::applicationDirPath() + "/checkVersionFile.tmp";
	QString strSvrVersion;
	QFile file;
	file.setFileName(path);
	if(file.open(QIODevice::ReadWrite))
	{
		//�����ļ������
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::ExeOther | QFileDevice::WriteOther);
		strSvrVersion = file.readAll().constData();
		strSvrVersion = strSvrVersion.trimmed();
		file.close();
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("�汾�Ƚ��ļ��޷���[%1]").arg(path));
		QMessageBox::warning(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�汾��Ϣ��ȡʧ�ܣ����Ժ�����"));
		QFile::remove(path);
		return;
	}

	QFile::remove(path);

	ClientLogger->AddLog(QString::fromLocal8Bit("���µ�����汾��Ϊ[%1] ��������汾��Ϊ[%2]").arg(strSvrVersion).arg(MessageDataMediator->m_strClientVersion));

	if(!CMisc::IsNeedUpdate(MessageDataMediator->m_strClientVersion, strSvrVersion))
	{
		QMessageBox::warning(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�ͻ��˰汾�������°汾��������£�"));
		return;
	}

	QString strUpdateExe = QApplication::applicationDirPath() + "\\smsetup.exe";
	if(!CMisc::IsFileExist(strUpdateExe))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("Ŀ¼�µ���������smsetup.exe������, �޷����£�"));
		QMessageBox::warning(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("Ŀ¼�µ���������smsetup.exe������, �޷����£�"));
		return;
	}

	if(!CMisc::ExcuteExe(strUpdateExe, false, 0, false))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��������smsetup.exeʧ��, ���Ժ����ԣ�"));
		QMessageBox::warning(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��������smsetup.exeʧ��, ���Ժ����ԣ�"));
		return;
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("��ʼ�������"));
	emit quitApplication();
}

void CSystemTrayIcon::timerEvent(QTimerEvent *event)
{
	int id = event->timerId();
	if(m_waitid == id)
	{
		killTimer(m_waitid);
		closeDownload();
		ClientLogger->AddLog(QString::fromLocal8Bit("���ذ汾�Ƚ��ļ���ʱ"));
		QMessageBox::warning(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�汾��Ϣ��ȡʧ�ܣ����Ժ�����"));
	}
}

void CSystemTrayIcon::closeDownload()
{
	if(m_pVersionFileDownloader)
	{
		m_pVersionFileDownloader->stopDownload();
		if(NULL != m_pVersionFileDownloader)
		{
			delete m_pVersionFileDownloader;
			m_pVersionFileDownloader = NULL;
		}
	}
}