#include "CRequestMsgMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
CRequestMsgMessage::CRequestMsgMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CRequestMsgMessage::~CRequestMsgMessage()
{

}

bool CRequestMsgMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_REQUESTMSG_REQ");
	xml.OutOfElem();
	xml.AddChildElem("Info");
	xml.IntoElem();
	xml.AddAttrib("Id", MessageDataMediator->m_strNewId.toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();
	return true;
}

bool CRequestMsgMessage::treatMessage(const NetMessage& netMessage, CNetClt* pNet)
{
	MessageDataMediator->setConnectMsgMapProperty(EMSG_REQUESTMSG, true);
	CMarkup xml;
	if(xml.SetDoc(netMessage.msgBuffer))
	{
		int errCode = 0;
		QString errorInfo = "";
		if(checkError(&xml, errCode, errorInfo))
		{
			if(0 != errCode)
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("RequestMsg [%1] : \r\n[%2]").arg(errorInfo).arg(netMessage.msgBuffer));
				emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, errorInfo);
				return true;
			}
		}

		if(xml.FindChildElem("Info"))
		{
			xml.IntoElem();
			while(xml.FindChildElem("Message"))
			{
				xml.IntoElem();
				QString strMsg = QString::fromStdString(xml.GetAttrib("Msg"));
				QString strTime = QString::fromStdString(xml.GetAttrib("Time"));
				emit NetClass->m_pMessageEventMediator->sigGetDistributeMsg(strMsg, strTime);
				xml.OutOfElem();
			}
			xml.OutOfElem();
			//ClientLogger->AddLog(QString::fromLocal8Bit("从服务器获取的XML 路径为[%1]").arg(strXMLPath));
			
		}
		else
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("RequestMsg收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
			emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
		}
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("RequestMsg收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
		emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
	}
	return true;
}