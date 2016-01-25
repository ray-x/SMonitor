#include "stdafx.h"
#include "CMessageDataMediator.h"
#include <QtCore/QCoreApplication>
CMessageDataMediator::CMessageDataMediator(QObject *parent)
	: QObject(qApp)
{
	m_nListenPort = 14640;
	m_nMaxClientsNum = -1;
	m_bTextMode = false; //yhb
	m_strExcelSavePath = "";
	m_lExcelSavePerTime = 1800;
}

CMessageDataMediator::~CMessageDataMediator()
{

}

CMessageDataMediator* CMessageDataMediator::m_Instance = NULL;

CMessageDataMediator* CMessageDataMediator::Instance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CMessageDataMediator();
		Q_ASSERT(m_Instance);
	}

	return m_Instance;
}
