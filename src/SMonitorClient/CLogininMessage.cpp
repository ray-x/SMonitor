#include "CLogininMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
CLogininMessage::CLogininMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CLogininMessage::~CLogininMessage()
{

}

bool CLogininMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_LOGIN_REQ");
	xml.OutOfElem();
	xml.AddChildElem("ClientInfo");
	xml.IntoElem();
	xml.AddAttrib("version", MessageDataMediator->m_strClientVersion.toStdString().c_str());
	xml.AddAttrib("Id", MessageDataMediator->m_strNewId.toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();
	return true;
}

bool CLogininMessage::treatMessage(const NetMessage& netMessage, CNetClt* pNet)
{
	CMarkup xml;
	if(xml.SetDoc(netMessage.msgBuffer))
	{
		int errCode = 0;
		QString errorInfo = "";
		if(checkError(&xml, errCode, errorInfo))
		{
			if(0 != errCode)
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("Loginin [%1] : \r\n[%2]").arg(errorInfo).arg(netMessage.msgBuffer));
				emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, errorInfo);
				return true;
			}
		}

		emit m_pMessageEventMediator->sigLoginIn();
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("Loginin收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
		emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
	}

	
	return true;
}