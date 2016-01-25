#include "stdafx.h"
#include "CRequestHelpInfoMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
#include "CMessageFactory.h"
CRequestHelpInfoMessage::CRequestHelpInfoMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CRequestHelpInfoMessage::~CRequestHelpInfoMessage()
{

}

void CRequestHelpInfoMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_HELPINFO_RSP");
	xml.OutOfElem();
	xml.AddChildElem("Result");
	xml.IntoElem();
	xml.AddAttrib("ErrorCode", 0);
	xml.AddAttrib("Value", QString::fromLocal8Bit("³É¹¦").toStdString().c_str());
	xml.OutOfElem();
	xml.AddChildElem("Info");
	xml.IntoElem();

	HelpInfoMap temp = MessageDataMediator->m_HelpInfoMap;
	int index = 0;
	QMapIterator<QString, QString> iter(temp);
	while (iter.hasNext())
	{
		iter.next();
		xml.AddChildElem("HelpInfo");
		xml.IntoElem();
		xml.AddAttrib("Label", iter.key().toStdString().c_str());
		xml.AddAttrib("Value", iter.value().toStdString().c_str());
		xml.OutOfElem();
	}

	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();
}

bool CRequestHelpInfoMessage::treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket)
{
	if(NULL != pFactory)
	{
		return pFactory->sendMessage(clientSocket, netMessage.msgHead);
	}

	return true;
}