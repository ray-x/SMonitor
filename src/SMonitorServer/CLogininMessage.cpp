#include "stdafx.h"
#include "CLogininMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "CMessageFactory.h"
CLogininMessage::CLogininMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CLogininMessage::~CLogininMessage()
{

}

void CLogininMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_LOGIN_RSP");
	xml.OutOfElem();
	xml.AddChildElem("Result");
	xml.IntoElem();
	xml.AddAttrib("ErrorCode", 0);
	xml.AddAttrib("Value", QString::fromLocal8Bit("登陆成功").toStdString().c_str());
	xml.OutOfElem();

	const char* src = xml.GetDoc().c_str();
	char* buffer = new char[strlen(src) + 1];
	memset(buffer, 0, strlen(src) + 1);
	strcpy(buffer, src);

	netMessage.msgBuffer = buffer;
	netMessage.msgLen = xml.GetDoc().length();
}

bool CLogininMessage::treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket)
{
	//记录到数据库，更新界面 yhb todo

	if(NULL != pFactory)
	{
		return pFactory->sendMessage(clientSocket, netMessage.msgHead);
	}

	return true;
}