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

#include "NetClass.h"
#include <QtCore/QCoreApplication>
#include "NetClt.h"
#include "CMessageEventMediator.h"

#include <QDebug>

CNetClass::CNetClass(QObject *parent)
	: QObject(qApp)
{
	m_pNetClt = NULL;
	m_pMessageEventMediator = NULL;
	m_pHomePageWidget = NULL;
}


CNetClass::~CNetClass(void)
{
	qDebug() << "~CNetClass(void)";
	if(NULL != m_pMessageEventMediator)
	{
		delete m_pMessageEventMediator;
		m_pMessageEventMediator = NULL;
	}

	if(NULL != m_pNetClt)
	{
		delete m_pNetClt;
		m_pNetClt = NULL;
	}
}

CNetClass* CNetClass::m_Instance = NULL;

CNetClass* CNetClass::Instance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CNetClass();
		Q_ASSERT(m_Instance);
	}

	return m_Instance;
}
