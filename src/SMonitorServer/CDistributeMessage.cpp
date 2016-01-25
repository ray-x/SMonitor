#include "stdafx.h"
#include "CDistributeMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
#include "CMessageFactory.h"
CDistributeMessage::CDistributeMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CDistributeMessage::~CDistributeMessage()
{

}

void CDistributeMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_DISTRIBUTEMSG_REQ");
	xml.OutOfElem();
	xml.AddChildElem("Result");
	xml.IntoElem();
	xml.AddAttrib("ErrorCode", 0);
	xml.AddAttrib("Value", QString::fromLocal8Bit("³É¹¦").toStdString().c_str());
	xml.OutOfElem();
	xml.AddChildElem("Info");
	xml.IntoElem();
	xml.AddAttrib("Msg", MessageDataMediator->m_strDistributeMsg.toStdString().c_str());
	xml.AddAttrib("Time", MessageDataMediator->m_strDistributeTime.toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();
}

bool CDistributeMessage::treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket)
{
	return true;
}