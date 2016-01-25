#include "StdAfx.h"
#include "LogBase.h"
#include "LogManager.h"

LogBase::LogBase(QObject *parent/* = 0*/) 
	: QObject(parent)
{
	m_pLogManager = new CLogManager();
}


LogBase::~LogBase(void)
{
	if(NULL != m_pLogManager)
	{
		delete m_pLogManager;
		m_pLogManager = NULL;
	}
}

//������־ϵͳ
bool LogBase::Start(const QString& strLogPath)
{
	if(NULL == m_pLogManager)
		return false;

	return m_pLogManager->Start(strLogPath);
}

//�����־
void LogBase::AddLog(const QString& strLog)
{
	if(NULL == m_pLogManager)
		return;

	m_pLogManager->AddLog(strLog);
}

//��ȡ���һ����־
QString LogBase::GetLastLog()
{
	if(NULL == m_pLogManager)
		return "";

	return m_pLogManager->m_strLastLog;
}