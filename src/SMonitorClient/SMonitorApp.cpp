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

// SMonitorApp.cpp : IMPLEMENTATION
//


#include "CSMonitorClient.h"
#include "CLoginDialog.h"
#include "NetClt.h"
#include "NetClass.h"
#include "CMessageDataMediator.h"
#include "SMonitorApp.h"
#include "CMisc.h"

#include "stdafx.h"

#include "CMessageEventMediator.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QTranslator>
#include <QTextCodec>

// CSMonitorApp

IMPLEMENT_DYNCREATE(CSMonitorApp, CWinApp)

CSMonitorApp::CSMonitorApp()
{
}

CSMonitorApp::~CSMonitorApp()
{
}

BOOL CSMonitorApp::InitInstance()
{
	int nNum = 0;
	char *chPara = "";
	QApplication a(nNum, &chPara);

	CoInitialize(NULL);

	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (NO_ERROR != nResult)
	{
		QMessageBox::warning(NULL, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("SOCKET库启动失败"));
		return FALSE;
	}
	//禁止双开
	HANDLE hObject = CreateMutex(NULL, FALSE, m_pszExeName);
	if(ERROR_ALREADY_EXISTS == GetLastError())
	{
		CloseHandle(hObject);
		QMessageBox::warning(NULL, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("软件已经启动"));
		return FALSE;
	}

	ClientLogger->Start(QApplication::applicationDirPath() + "/Log/");

	NetClass->m_pMessageEventMediator = new CMessageEventMediator();
	NetClass->m_pNetClt = new CNetClt(NetClass->m_pMessageEventMediator);

	ClientLogger->AddLog(QString::fromLocal8Bit("---------------------------------------------"));
	ClientLogger->AddLog(QString::fromLocal8Bit("客户端启动"));
	//读取INI配置
	readConfigure();
	NetClass->m_pNetClt->SetIP(m_strSvrIP.toStdString().c_str());
	NetClass->m_pNetClt->SetPort(m_nSvrPort);
	NetClass->m_pNetClt->Start();

	QFile file(":/Resources/blue.css");
	if(file.open(QFile::ReadOnly))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("打开样式文件成功"));
		if(CMisc::IsOsVerHigherXP())
			qApp->setStyleSheet(QLatin1String(file.readAll()));
		else
			qApp->setStyleSheet(QLatin1String(file.readAll().append("\r\nQProgressBar::chunk{margin: 0.5px; background-color: #06B025;}")));
		
		qApp->setPalette(QPalette(QColor("#F0F0F0")));

		if(MessageDataMediator->m_bTextMode)
			ClientLogger->AddLog(qApp->styleSheet());

		file.close();
	}

	//查询门店信息
	MessageDataMediator->QueryShopInfo(true);
	
	CLoginDialog dlg;
	bool bInputID = false;
	if(m_strClientID.isEmpty())
	{
		if(dlg.exec() == QDialog::Rejected){
			a.quit();
			return 0;
		}

		bInputID = true;
	}

	CSMonitorClient w;
// 	if(bInputID || NetClass->m_pNetClt->m_bIsOnLine)
// 	{
// 		NetClass->m_pNetClt->SendMsg(EMSG_REQUESTXML, true);
// 	}
// 	else
// 	{
// 		NetClass->m_pNetClt->m_bAutoSendQueryXMLMsg = true;
// 	}
	
	w.show();
	return a.exec();
}

int CSMonitorApp::ExitInstance()
{
	CoUninitialize();
	WSACleanup();
	return CWinApp::ExitInstance();
}

void CSMonitorApp::readConfigure()
{
	QString path = QApplication::applicationDirPath() + "/configure.ini";
	QSettings settings(path, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");
	MessageDataMediator->m_strNewId = m_strClientID = getId();
	m_strSvrIP = settings.value("ServerInfo/ServerIP", QVariant("")).toString();
	m_nSvrPort = settings.value("ServerInfo/ServerPort", QVariant("")).toInt();
	m_strHostName = settings.value("ServerInfo/HostName", QVariant("")).toString();

	if(m_strSvrIP.isEmpty() && !m_strHostName.isEmpty())
	{
		QStringList strIpList = CMisc::GetIpByHostName(m_strHostName);
		if(strIpList.count() <= 0)
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("解析域名失败，错误码[%1]").arg(WSAGetLastError()));
		}
		else
		{
			m_strSvrIP = strIpList[0];
		}
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("服务器IP[%1]").arg(m_strSvrIP));

	if(m_strSvrIP.isEmpty())
	{
		m_strSvrIP = "127.0.0.1";
	}

	if(0 == m_nSvrPort)
	{
		m_nSvrPort = 14640;
	}

	QSettings settingsRegedit(HKCU, QSettings::NativeFormat);
	settingsRegedit.setIniCodec(QTextCodec::codecForName("UTF-8"));
	MessageDataMediator->m_strSvrUpdateXmlUrl = settingsRegedit.value("XmlUrl/UpdateXml").toString();
	MessageDataMediator->m_strHelpInfoXmlUrl = settingsRegedit.value("XmlUrl/HelpInfoXml").toString();
	MessageDataMediator->m_strVersionCheckFile = settingsRegedit.value("XmlUrl/VersionCheckFile").toString();
	//MessageDataMediator->m_bTextMode = settingsRegedit.value("Connect/TextMode").toString() == "1"; yhb
	ClientLogger->AddLog(QString::fromLocal8Bit("客户端模式:[%1]").arg(MessageDataMediator->m_bTextMode ? QString::fromLocal8Bit("测试") : QString::fromLocal8Bit("正常")));
}

QString CSMonitorApp::getId()
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec("UTF-8");
	return settings.value("RegisterID/ID", QString("")).toString();
}

BEGIN_MESSAGE_MAP(CSMonitorApp, CWinApp)
END_MESSAGE_MAP()


// CSMonitorApp 消息处理程序
