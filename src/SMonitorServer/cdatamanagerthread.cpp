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

//���������Ϊ��������Ϊ�첽�������ݿ�
void CDataManagerThread::AddDataMotion(DataMotion& dataMotion)
{
	QMutexLocker locker(&m_DataLock);
	m_DataMotionVector.append(dataMotion);
}

//�������ݿ��ļ�
//bSaveImmediately - true Save�������洢 - false Save����ͨ������ģʽ�������Ҫ����Commit����
//bCreateIfInExist - true ������ݿⲻ���ڻ��Զ����� - false ���ݿⲻ���ڣ��ᴴ��ʧ�ܣ�����false
bool CDataManagerThread::LoadDatabase(const QString& strFilePath, bool bCreateIfInExist/* = true*/)
{
	return DatabaseManager::Instance()->LoadDatabase(strFilePath, bCreateIfInExist);
}

//ͨ���ŵ����жϿͻ����Ƿ����
bool CDataManagerThread::IsClientExist(const QString& clientId)
{
	//1.�ж����ݿ����Ƿ����
	if(DatabaseManager::Instance()->IsClientExist(clientId))
		return true;

	//2.�ж��б�
	if(isClientExistInDataVector(clientId))
		return true;

	//3.�жϻ����б�
	if(isClientExistInToExcuteDataVector(clientId))
		return true;

	return false;
}

//�ж�����Ƿ����
bool CDataManagerThread::IsSoftwareExist(const QString& strSoftwatename)
{
	return DatabaseManager::Instance()->IsSoftwareExist(strSoftwatename);
}

//�ж������װ֪ͨ��Ϣ�Ƿ����
bool CDataManagerThread::IsSoftInstallInfoExist(const QString& clientId, const QString& name, const QString& version)
{
	//1.�ж����ݿ����Ƿ����
	if(DatabaseManager::Instance()->IsSoftInstallInfoExist(clientId, name, version))
		return true;

	//2.�ж��б�
	if(isSoftInstallInfoExistInDataVector(clientId, name, version))
		return true;

	//3.�жϻ����б�
	if(isSoftInstallInfoExistInToExcuteDataVector(clientId, name, version))
		return true;

	return false;
}

//�ж����ڲ���������Ϣ�Ƿ����
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
		//����Ƿ���������Ҫд�����ݿ�
		if(CheckDataList())
		{
			//д����
			WriteToDatabase();
		}

		time_t curTime;
		time(&curTime);

		if(curTime - prevTime > MessageDataMediator->m_lExcelSavePerTime)
		{
			WriteShopInfoToExcel();
			prevTime = curTime;
		}

		msleep(10000); //�ռ�10��֮�ڵ�SQL
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

//������д�����ݿ�
void CDataManagerThread::WriteToDatabase()
{
	DatabaseManager::Instance()->WriteDataMotionList(m_ToBeExcuteDataMotionVector);

	QMutexLocker locker(&m_ToBeExcuteDataLock);
	m_ToBeExcuteDataMotionVector.clear();
}

//�ж�DataMotionVector���Ƿ����Client
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

//�ж�ToBeExcuteDataVector���Ƿ����Client
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

//�ж�DataMotionVector���Ƿ����SoftInstallInfo
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

//�ж�ToBeExcuteDataVector���Ƿ����SoftInstallInfo
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

//д���ŵ���Ϣ��EXCEL
void CDataManagerThread::WriteShopInfoToExcel()
{
	if(!MessageDataMediator->m_strExcelSavePath.isEmpty())
	{
		QHash<int, QStringList> data;
		DatabaseManager::Instance()->ReadShopInfoHash(data);
		if(ExcelHelper::Instance()->ToExcel(MessageDataMediator->m_strExcelSavePath, data))
		{
			ServerLogger->AddLog("[%s]excelд��ɹ�", MessageDataMediator->m_strExcelSavePath.toLocal8Bit().constData());
		}
	}
}