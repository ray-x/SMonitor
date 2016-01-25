#include "stdafx.h"
#include "CModifyIdMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "CMessageFactory.h"
#include "databasemanager.h"

CModifyIdMessage::CModifyIdMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
	m_bIdExist = false;
}

CModifyIdMessage::~CModifyIdMessage()
{

}

void CModifyIdMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_REGISTERID_RSP");
	xml.OutOfElem();
	xml.AddChildElem("Result");
	xml.IntoElem();
	xml.AddAttrib("ErrorCode", m_bIdExist ? 1 : 0);
	xml.AddAttrib("Value", QString::fromLocal8Bit(m_bIdExist ? "编号已存在" : "修改成功").toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);;
	netMessage.msgLen = xml.GetDoc().length();
}

bool CModifyIdMessage::treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket)
{
	m_bIdExist = false;
	CMarkup xml;
	if(xml.SetDoc(netMessage.msgBuffer))
	{
		if(xml.FindChildElem("Info"))
		{
			xml.IntoElem();
			QString strOldId = QString::fromStdString(xml.GetAttrib("OldId"));
			QString strNewId = QString::fromStdString(xml.GetAttrib("NewId"));
			if(DataManagerThread->IsClientExist(strNewId))
			{
				m_bIdExist = true;
			}
			else
			{
				DataMotion dataMotion;
				dataMotion.msg = netMessage.msgHead;
				dataMotion.clientInfo.id = strNewId;
				dataMotion.clientInfo.version = strOldId;
				DataManagerThread->AddDataMotion(dataMotion);
			}
			xml.OutOfElem();
		}
	}

	if(NULL != pFactory)
	{
		return pFactory->sendMessage(clientSocket, netMessage.msgHead);
	}

	return true;
}