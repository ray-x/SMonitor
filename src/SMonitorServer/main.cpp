#include "stdafx.h"
#include "CSMonitorServer.h"
#include <QtWidgets/QApplication>
#include "CMessageDataMediator.h"
#include "databasemanager.h"
#include "CLoginDialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include "CMisc.h"
bool loadSocket()
{
	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (NO_ERROR != nResult)
	{
		return false;
	}

	return true;
}

void readConfigure()
{
	QString path = QApplication::applicationDirPath() + "/configure.ini";
	MessageDataMediator->m_nListenPort = CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "ListenPort", 14640);

	char chBuf[512] = {0};
	CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "UpdateXml", "", chBuf, sizeof(chBuf));
	MessageDataMediator->m_strUpdateXMLUrl = QString::fromLocal8Bit(chBuf);

	memset(chBuf, 0, sizeof(chBuf));
	CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "HelpInfoXml", "", chBuf, sizeof(chBuf));
	MessageDataMediator->m_strHelpXMLUrl = QString::fromLocal8Bit(chBuf);

	memset(chBuf, 0, sizeof(chBuf));
	CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "VersionCheckFile", "", chBuf, sizeof(chBuf));
	MessageDataMediator->m_strClientVersionUrl = QString::fromLocal8Bit(chBuf);
	
	memset(chBuf, 0, sizeof(chBuf));
	CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "DowloadStatusFileUrl", "", chBuf, sizeof(chBuf));
	MessageDataMediator->m_strDownloadStatusFileUrl = QString::fromLocal8Bit(chBuf);

	int nTextMode = CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "TextMode", 0);
	//MessageDataMediator->m_bTextMode = 0 != nTextMode; yhb

	MessageDataMediator->m_nMaxClientsNum = CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "MaxClientsNum", -1);

	memset(chBuf, 0, sizeof(chBuf));
	CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "ExcelSavePath", "", chBuf, sizeof(chBuf));
	MessageDataMediator->m_strExcelSavePath = QString::fromLocal8Bit(chBuf);

	MessageDataMediator->m_lExcelSavePerTime = CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Server", "ExcelSavePerMinute", 30) * 60;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//禁止双开
	HANDLE hObject = CreateMutex(NULL, FALSE, "SMonitorServer.exe");
	if(ERROR_ALREADY_EXISTS == GetLastError())
	{
		CloseHandle(hObject);
		QMessageBox::warning(NULL, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("服务器已经启动"));
		return 0;
	}

	if(!loadSocket())
	{
		QMessageBox::information(NULL, "error", QString::fromLocal8Bit("加载SOCKET库失败, 服务器启动失败"));
		return 0;
	}

	
	//读取配置文件
	readConfigure();

	//启动服务器业务日志管理器
	QString dataPath = QApplication::applicationDirPath() + "/ServerLogger/SvrLog";
	QDir dir;
	if(!dir.mkpath(dataPath))
	{
		QMessageBox::warning(NULL, "Error", 
			QString::fromLocal8Bit("日志目录[%1]创建失败，请确认目标磁盘是否空间已满").arg(dataPath),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	//dataPath = "d:/ServerLogger/SvrLog";  //yhb
	if(!ServerLogger->Start(dataPath.toLocal8Bit().constData()))
	{
		QMessageBox::information(NULL, "error", QString::fromLocal8Bit("加载服务器日志系统失败, 服务器启动失败"));
		return 0;
	}


	CLoginDialog dlg;
	if(dlg.exec() == QDialog::Rejected)
	{
		return 0;
	}

	//启动数据管理器线程
	if(!CDataManagerThread::Instance()->Start())
	{
		QMessageBox::information(NULL, "error", QString::fromLocal8Bit("数据管理器线程启动失败, 服务器启动失败"));
		return 0;
	}

	ServerLogger->AddLog("服务器软件运行");

	CSMonitorServer w;
	w.show();
	return a.exec();
}


