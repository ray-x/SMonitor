#include "CClientLog.h"
#include <QtCore/QCoreApplication>
#include <QDebug>
CClientLog::CClientLog(QObject *parent)
	: LogBase(qApp)
{

}

CClientLog::~CClientLog()
{
	qDebug() << "~CClientLog()";
}

CClientLog* CClientLog::m_Instance = NULL;

CClientLog* CClientLog::Instance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CClientLog();
		Q_ASSERT(m_Instance);
	}

	return m_Instance;
}