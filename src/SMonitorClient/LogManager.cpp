#include "LogManager.h"
#include <QDir>
#include <QDate>
#include <QMutex>
#include <QFile>
#include <QtCore/QCoreApplication>
CLogManager::CLogManager(QObject *parent/* = 0*/)
	:QThread(qApp)
{
	m_bIsStart = false;
	m_bThreadExit = true;
}

CLogManager::~CLogManager()
{
	Clear();
}

//启动日志系统
bool CLogManager::Start(const QString& strPath)
{
	if(strPath.isNull() || strPath.isEmpty())
	{
		return false;
	}

	if(m_bIsStart) return true;

	m_bIsStart = true;
	m_strPath = strPath;
	start();
	m_bThreadExit = false;
	return true;
}

//添加日志
void CLogManager::AddLog(const QString& strLog)
{
	if(strLog.isNull() || strLog.isEmpty())
		return;

	if(!m_bIsStart) return;

	QMutexLocker locker(&m_TempLogInfoListMutex);
	m_strLastLog = strLog;
	LOGINFO logInfo;
	logInfo.strDataTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
	logInfo.strLogInfo = strLog;
	m_TempLogInfoList.push_back(logInfo);
}

//释放资源
void CLogManager::Clear()
{
	if(!m_bIsStart) return;

	m_bThreadExit = true;
	msleep(100);
	if(isRunning())
	{
		msleep(900);
	}
	terminate();

	QMutexLocker locker(&m_TempLogInfoListMutex);
	m_TempLogInfoList.clear();
}

//检查日志列表
bool CLogManager::CheckLogInfoList()
{
	if(m_TempLogInfoList.count() <= 0) return false;

	QMutexLocker locker(&m_TempLogInfoListMutex);
	m_strLogInfo.clear();
	int nMaxLog = 1;
	for (int i = 0; i < m_TempLogInfoList.count() && nMaxLog <= 10; nMaxLog++)
	{
		LOGINFO& logInfo = m_TempLogInfoList[i];
		m_strLogInfo += QString::fromLocal8Bit("[%1]%2\r\n").arg(logInfo.strDataTime).arg(logInfo.strLogInfo);
		m_TempLogInfoList.removeAt(i);
	}

	return !m_strLogInfo.isEmpty();
}

//写日志
void CLogManager::LoggerWrite()
{
	QDir dir;
	dir.mkpath(m_strPath);

	QString strLogPath = QString::fromLocal8Bit("%1\\%2.log").arg(m_strPath).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));

	QFile file(strLogPath);
	if(file.open(QIODevice::ReadWrite | QIODevice::Append))
	{
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
		file.write(m_strLogInfo.toStdString().c_str());
		file.close();
	}
}

void CLogManager::run()
{
	while(!m_bThreadExit)
	{
		//检查是否有日志
		if(CheckLogInfoList())
		{
			//写日志
			LoggerWrite();
		}

		msleep(10);
	}

	m_bThreadExit = true;
}