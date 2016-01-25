#include "..\StdAfx.h"
#include "srvexception.h"

CSrvException::CSrvException(void)
{
	m_iExpCode = 0;
	m_iExpLineNumber = 0;
}

CSrvException::CSrvException(const QString& expDescription, int expCode/*=0*/)
{ 
	m_strExpDescription = expDescription;

	if(expCode != 0)
	{
		m_strExpDescription += QString::fromLocal8Bit(" ´íÎó´úÂëÊÇ[%1]").arg(expCode);
	}

	this->m_iExpCode = expCode;
}
CSrvException::~CSrvException(void)
{
}

QString CSrvException::GetExpDescription()
{
	return this->m_strExpDescription;
}

int CSrvException::GetExpCode()
{
	return this->m_iExpCode;
}

long CSrvException::GetExpLine()
{
	return this->m_iExpLineNumber;
}