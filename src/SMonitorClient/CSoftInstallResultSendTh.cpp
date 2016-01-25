#include "CSoftInstallResultSendTh.h"
#include "Markup.h"
#include "CClientLog.h"
#include "NetClass.h"
#include "CMessageEventMediator.h"
#include "NetClt.h"
#include "CMessageDataMediator.h"
#include "CMisc.h"
#include <QtCore/QCoreApplication>
CSoftInstallResultSendThread::CSoftInstallResultSendThread(QObject *parent)
	: QThread(qApp)
{
	m_nHasWaitSendTime = 0;
	m_nSendWaitTime = 60000;
	m_bIsStart = false;
	m_bThreadExit = true;
	m_bIsTreatSendReceive = false;
}

CSoftInstallResultSendThread* CSoftInstallResultSendThread::m_Instance = NULL;

CSoftInstallResultSendThread* CSoftInstallResultSendThread::Instance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CSoftInstallResultSendThread();
		Q_ASSERT(m_Instance);
	}

	return m_Instance;
}

CSoftInstallResultSendThread::~CSoftInstallResultSendThread()
{
	Clear();
}

//启动
bool CSoftInstallResultSendThread::Start(const QString& strPath)
{
	if(strPath.isNull() || strPath.isEmpty())
	{
		return false;
	}

	if(m_bIsStart) return true;

	//从文件中还原上传队列到list
	m_strPath = strPath;
	ReloadFromFile();

	m_bIsStart = true;
	start();
	m_bThreadExit = false;
	return true;
}

//添加
void CSoftInstallResultSendThread::AddSoftData(const SoftData& softData)
{
	if(!m_bIsStart) return;

	QMutexLocker locker(&m_TempSoftDataListMutex);
	for (int i = 0; i < m_TempSoftDataList.count(); i++)
	{
		SoftData& refSoftData = m_TempSoftDataList[i];
		if(refSoftData.name == softData.name)
		{//同一软件多个版本只会有一个软件序列上传

			if(CMisc::IsNeedUpdate(refSoftData.version, softData.version)) //比较版本号 yhb
			{
				refSoftData.version = softData.version;
				ChangeInfoInFile(softData);
			}
			return;
		}
	}

	if(!WriteToFile(softData))
		ClientLogger->AddLog("安装结果写入文件失败!");

	m_TempSoftDataList.append(softData);
}

void CSoftInstallResultSendThread::serverHasGetSoftInstall(const QString& name, const QString& version)
{
	QMutexLocker locker(&m_TempSoftDataListMutex);
	for (int i = 0; i < m_TempSoftDataList.count(); i++)
	{
		SoftData& refSoftData = m_TempSoftDataList[i];
		if(refSoftData.name == name)
		{//同一软件多个版本只会有一个软件序列上传

			if(refSoftData.version <= version) //比较版本号 yhb
			{
				DeleteInfoInFile(refSoftData);
				m_TempSoftDataList.removeAt(i);
			}
			return;
		}
	}

	if(m_nCurSendSoftData.name == name && m_nCurSendSoftData.version == version)
	{
		m_bIsTreatSendReceive = false;
		m_nHasWaitSendTime = 0;
	}
}

//释放资源
void CSoftInstallResultSendThread::Clear()
{
	if(!m_bIsStart) return;

	m_bThreadExit = true;
	msleep(1000);
	terminate();

	QMutexLocker locker(&m_TempSoftDataListMutex);
	m_TempSoftDataList.clear();
}

//检查列表
bool CSoftInstallResultSendThread::CheckSoftDataList()
{
	if(m_TempSoftDataList.count() <= 0)
		return false;

	//如果正在处理一个请求并且还没到超时时间则继续等待
	if(m_bIsTreatSendReceive && (m_nHasWaitSendTime < m_nSendWaitTime))
		return false;

	QMutexLocker locker(&m_TempSoftDataListMutex);
	if(m_TempSoftDataList.count() <= 0)
		return false;

	m_nCurSendSoftData = m_TempSoftDataList[0];
	MessageDataMediator->m_strSoftname = m_nCurSendSoftData.name;
	MessageDataMediator->m_strVersion = m_nCurSendSoftData.version;
	
	return true;
}

//上传
void CSoftInstallResultSendThread::Update()
{
	if(NetClass->m_pNetClt->SendMsg(EMSG_SOFTINSTALL, false))
	{
		m_nHasWaitSendTime = 0;
		m_bIsTreatSendReceive = true;
	}
}

//记录到文件
bool CSoftInstallResultSendThread::WriteToFile(const SoftData& softData)
{
	CMarkup xml;
	if(!xml.Load(m_strPath.toLocal8Bit().constData()))
	{
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("Root");
		xml.AddChildElem("SoftwareList");
	}
	else
	{
		::SetFileAttributes(m_strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_NORMAL);
	}

	xml.ResetMainPos();
	if(!xml.FindChildElem("SoftwareList"))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("发现 WriteToFile xml!xml.FindChildElem(\"SoftwareList\") "));
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("Root");
		xml.AddChildElem("SoftwareList");
	}

	xml.IntoElem();
	xml.AddChildElem("Software");
	xml.IntoElem();
	xml.AddAttrib("name", softData.name.toStdString());
	xml.AddAttrib("version", softData.version.toStdString());
	xml.OutOfElem();
	xml.OutOfElem();

	bool bRet = xml.Save(m_strPath.toLocal8Bit().constData());
	::SetFileAttributes(m_strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN);
	return bRet;
}

//从文件中还原上传队列到list
bool CSoftInstallResultSendThread::ReloadFromFile()
{
	QMutexLocker locker(&m_TempSoftDataListMutex);
	CMarkup xml;
	if(!xml.Load(m_strPath.toLocal8Bit().constData()))
	{
		return false;
	}

	if(!xml.FindChildElem("SoftwareList"))
		return false;

	xml.IntoElem();
	while(xml.FindChildElem("Software"))
	{
		SoftData softData;
		xml.IntoElem();
		softData.name = QString::fromStdString(xml.GetAttrib("name"));
		softData.version =  QString::fromStdString(xml.GetAttrib("version"));
		xml.OutOfElem();
		m_TempSoftDataList.push_back(softData);
	}
	xml.OutOfElem();

	return true;
}

//改变信息
bool CSoftInstallResultSendThread::ChangeInfoInFile(const SoftData& softData)
{
	CMarkup xml;
	if(!xml.Load(m_strPath.toLocal8Bit().constData()))
	{
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("Root");
		xml.AddChildElem("SoftwareList");
		xml.IntoElem();
		xml.AddChildElem("Software");
		xml.IntoElem();
		xml.AddAttrib("name", softData.name.toStdString());
		xml.AddAttrib("version", softData.version.toStdString());
		xml.OutOfElem();
		xml.OutOfElem();
		bool bRet = xml.Save(m_strPath.toLocal8Bit().constData());
		::SetFileAttributes(m_strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
		return bRet;
	}

	if(xml.FindChildElem("SoftwareList"))
		return false;

	xml.IntoElem();
	bool bFind = false;
	while(!bFind && xml.FindChildElem("Software"))
	{
		xml.IntoElem();
		QString strName = QString::fromStdString(xml.GetAttrib("name"));
		if(strName == softData.name)
		{
			xml.SetAttrib("version", softData.version.toStdString());
			bFind = true;
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();

	if(!bFind)
	{
		xml.ResetMainPos();
		if(!xml.FindChildElem("SoftwareList"))
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("!xml.FindChildElem(\"SoftwareList\") 致命错误！"));
			return false;
		}

		xml.IntoElem();
		xml.AddChildElem("Software");
		xml.IntoElem();
		xml.AddAttrib("name", softData.name.toStdString());
		xml.AddAttrib("version", softData.version.toStdString());
		xml.OutOfElem();
		xml.OutOfElem();
	}

	::SetFileAttributes(m_strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_NORMAL); 
	bool bRet = xml.Save(m_strPath.toLocal8Bit().constData());
	::SetFileAttributes(m_strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
	return bRet;
}

//删除一条信息
bool CSoftInstallResultSendThread::DeleteInfoInFile(const SoftData& softData)
{
	CMarkup xml;
	if(!xml.Load(m_strPath.toLocal8Bit().constData()))
	{
		return true;
	}

	if(!xml.FindChildElem("SoftwareList"))
		return false;

	xml.IntoElem();

	bool bFind = false;
	while(!bFind && xml.FindChildElem("Software"))
	{
		xml.IntoElem();
		QString strName = QString::fromStdString(xml.GetAttrib("name"));
		if(strName == softData.name)
		{
			xml.RemoveElem();
			bFind = true;
			break;
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();

	bool bRet = true;
	if(bFind)
	{
		::SetFileAttributes(m_strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_NORMAL); 
		bRet = xml.Save(m_strPath.toLocal8Bit().constData());
		::SetFileAttributes(m_strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
	}

	return true;
}

void CSoftInstallResultSendThread::run()
{
	while(!m_bThreadExit)
	{
		if(CheckSoftDataList())
		{
			Update();
		}

		msleep(10);

		if(m_bIsTreatSendReceive)
			m_nHasWaitSendTime += 10;
	}

	m_bThreadExit = true;
}