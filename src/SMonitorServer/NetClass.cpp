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
#include "NetClass.h"
#include <QtCore/QCoreApplication>
#include "IOCPNet.h"
#include "CMessageEventMediator.h"
#include "CSoftUpgradeWidget.h"

CNetClass::CNetClass(QObject *parent)
	: QObject(qApp)
{
	m_pNetClt = NULL;
	m_pMessageEventMediator = NULL;
	m_pSoftUpgradeWidget = NULL;
}


CNetClass::~CNetClass(void)
{
	RELEASE(m_pNetClt);
	RELEASE(m_pMessageEventMediator);
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
