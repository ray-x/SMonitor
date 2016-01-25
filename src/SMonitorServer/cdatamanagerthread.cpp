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
#include "cdatamanagerthread.h"
#include <QtCore/QCoreApplication>
#include <QMutex>
#include "databasemanager.h"
#include <time.h>
#include "CMessageDataMediator.h"
#include "excelhelper.h"
#include <QStringList>

CDataManagerThread* CDataManagerThread::m_Instance = NULL;

CDataManagerThread* CDataManagerThread::Instance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CDataManagerThread();
		Q_ASSERT(m_Instance);
	}

	return m_Instance;
}

CDataManagerThread::CDataManagerThread(QObject *parent)
	: QThread(qApp)
{
	m_bThreadExit = false;
}

CDataManagerThread::~CDataManagerThread()
{
	Stop();
}

bool CDataManagerThread::Start()
{
	Stop();
	start();
	m_bThreadExit = false;
	return true;
}

void CDataManagerThread::Stop()
{	
	if(m_bThreadExit)
		return;

	m_bThreadExit = true;
	msleep(1000);
	terminate();
	QMutexLocker locker(&m_DataLock);
	m_DataMotionVector.clear();
	m_ToBeExcuteDataMotionVector.clear();
}

//添加数据行为，此数据为异步插入数据库
void CDataManagerThread::AddDataMotion(DataMotion& dataMotion)
{
	QMutexLocker locker(&m_DataLock);
	m_DataMotionVector.append(dataMotion);
}

//加载数据库文件
//bSaveImmediately - true Save会立即存储 - false Save操作通过事务模式，最后需要调用Commit保存
//bCreateIfInExist - true 如果数据库不存在会自动创建 - false 数据库不存在，会创建失败，返回false
bool CDataManagerThread::LoadDatabase(const QString& strFilePath, bool bCreateIfInExist/* = true*/)
{
	return DatabaseManager::Instance()->LoadDatabase(strFilePath, bCreateIfInExist);
}

//通过门店编号判断客户端是否存在
bool CDataManagerThread::IsClientExist(const QString& clientId)
{
	//1.判断数据库中是否存在
	if(DatabaseManager::Instance()->IsClientExist(clientId))
		return true;

	//2.判断列表
	if(isClientExistInDataVector(clientId))
		return true;

	//3.判断缓存列表
	if(isClientExistInToExcuteDataVector(clientId))
		return true;

	return false;
}

//判断软件是否存在
bool CDataManagerThread::IsSoftwareExist(const QString& strSoftwatename)
{
	return DatabaseManager::Instance()->IsSoftwareExist(strSoftwatename);
}

//判断软件安装通知信息是否存在
bool CDataManagerThread::IsSoftInstallInfoExist(const QString& clientId, const QString& name, const QString& version)
{
	//1.判断数据库中是否存在
	if(DatabaseManager::Instance()->IsSoftInstallInfoExist(clientId, name, version))
		return true;

	//2.判断列表
	if(isSoftInstallInfoExistInDataVector(clientId, name, version))
		return true;

	//3.判断缓存列表
	if(isSoftInstallInfoExistInToExcuteDataVector(clientId, name, version))
		return true;

	return false;
}

//判断正在插入的软件信息是否存在
bool CDataManagerThread::IsSoftInfoExist(const QString& clientId)
{
	{
		QMutexLocker locker(&m_DataLock);
		int nCount = m_DataMotionVector.count();
		for(int i = 0; i < nCount; i++)
		{
			const DataMotion& dataMotion = m_DataMotionVector[i];
			if(dataMotion.msg == EMSG_SOFTUPLOAD)
			{
				if(dataMotion.clientInfo.id == clientId)
					return true;
			}
		}
	}

	{
		QMutexLocker locker(&m_ToBeExcuteDataLock);
		int nCount = m_ToBeExcuteDataMotionVector.count();
		for(int i = 0; i < nCount; i++)
		{
			const DataMotion& dataMotion = m_ToBeExcuteDataMotionVector[i];
			if(dataMotion.msg == EMSG_SOFTUPLOAD)
			{
				if(dataMotion.clientInfo.id == clientId)
					return true;
			}
		}
	}

	return false;
}

void CDataManagerThread::run()
{
	time_t prevTime;
	time(&prevTime);
	WriteShopInfoToExcel(); 

	while(!m_bThreadExit)
	{
		//检查是否有数据需要写入数据库
		if(CheckDataList())
		{
			//写数据
			WriteToDatabase();
		}

		time_t curTime;
		time(&curTime);

		if(curTime - prevTime > MessageDataMediator->m_lExcelSavePerTime)
		{
			WriteShopInfoToExcel();
			prevTime = curTime;
		}

		msleep(10000); //收集10秒之内的SQL
	}
}

bool CDataManagerThread::CheckDataList()
{
	if(m_DataMotionVector.count() <= 0) return false;

	QMutexLocker locker(&m_DataLock);
	m_ToBeExcuteDataMotionVector.clear();
	int nDataMotionNums = 1;
	for (int i = 0; i < m_DataMotionVector.count() && nDataMotionNums <= MaxDataMotionWriteToDataBase; nDataMotionNums++)
	{
		m_ToBeExcuteDataMotionVector.append(m_DataMotionVector.at(i));
		m_DataMotionVector.removeAt(i);
	}

	return !m_ToBeExcuteDataMotionVector.isEmpty();
}

//将数据写入数据库
void CDataManagerThread::WriteToDatabase()
{
	DatabaseManager::Instance()->WriteDataMotionList(m_ToBeExcuteDataMotionVector);

	QMutexLocker locker(&m_ToBeExcuteDataLock);
	m_ToBeExcuteDataMotionVector.clear();
}

//判断DataMotionVector里是否存在Client
bool CDataManagerThread::isClientExistInDataVector(const QString& clientId)
{
	QMutexLocker locker(&m_DataLock);
	int nCount = m_DataMotionVector.count();
	for(int i = 0; i < nCount; i++)
	{
		const DataMotion& dataMotion = m_DataMotionVector[i];
		if(dataMotion.msg == EMSG_REGISTERID || dataMotion.msg == EMSG_MODIFYID)
		{
			if(dataMotion.clientInfo.id == clientId)
				return true;
		}
	}

	return false;
}

//判断ToBeExcuteDataVector里是否存在Client
bool CDataManagerThread::isClientExistInToExcuteDataVector(const QString& clientId)
{
	QMutexLocker locker(&m_ToBeExcuteDataLock);
	int nCount = m_ToBeExcuteDataMotionVector.count();
	for(int i = 0; i < nCount; i++)
	{
		const DataMotion& dataMotion = m_ToBeExcuteDataMotionVector[i];
		if(dataMotion.msg == EMSG_REGISTERID || dataMotion.msg == EMSG_MODIFYID)
		{
			if(dataMotion.clientInfo.id == clientId)
				return true;
		}
	}

	return false;
}

//判断DataMotionVector里是否存在SoftInstallInfo
bool CDataManagerThread::isSoftInstallInfoExistInDataVector(const QString& clientId, const QString& name, const QString& version)
{
	QMutexLocker locker(&m_DataLock);
	int nCount = m_DataMotionVector.count();
	for(int i = 0; i < nCount; i++)
	{
		const DataMotion& dataMotion = m_DataMotionVector[i];
		if(dataMotion.msg == EMSG_SOFTINSTALL)
		{
			if(dataMotion.clientInfo.id == clientId && 
				dataMotion.softwareInfo.name == name && 
				dataMotion.softwareInfo.version == version)
				return true;
		}
	}

	return false;
}

//判断ToBeExcuteDataVector里是否存在SoftInstallInfo
bool CDataManagerThread::isSoftInstallInfoExistInToExcuteDataVector(const QString& clientId, const QString& name, const QString& version)
{
	QMutexLocker locker(&m_ToBeExcuteDataLock);
	int nCount = m_ToBeExcuteDataMotionVector.count();
	for(int i = 0; i < nCount; i++)
	{
		const DataMotion& dataMotion = m_ToBeExcuteDataMotionVector[i];
		if(dataMotion.msg == EMSG_SOFTINSTALL)
		{
			if(dataMotion.clientInfo.id == clientId && 
				dataMotion.softwareInfo.name == name && 
				dataMotion.softwareInfo.version == version)
				return true;
		}
	}

	return false;
}

//写入门店信息到EXCEL
void CDataManagerThread::WriteShopInfoToExcel()
{
	if(!MessageDataMediator->m_strExcelSavePath.isEmpty())
	{
		QHash<int, QStringList> data;
		DatabaseManager::Instance()->ReadShopInfoHash(data);
		if(ExcelHelper::Instance()->ToExcel(MessageDataMediator->m_strExcelSavePath, data))
		{
			ServerLogger->AddLog("[%s]excel写入成功", MessageDataMediator->m_strExcelSavePath.toLocal8Bit().constData());
		}
	}
}