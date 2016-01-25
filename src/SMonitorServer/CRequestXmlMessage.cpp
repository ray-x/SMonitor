#include "stdafx.h"
#include "CRequestXmlMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
#include "CMessageFactory.h"
CRequestXmlMessage::CRequestXmlMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CRequestXmlMessage::~CRequestXmlMessage()
{

}

void CRequestXmlMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_REQUESTXML_RSP");
	xml.OutOfElem();
	xml.AddChildElem("Result");
	xml.IntoElem();
	xml.AddAttrib("ErrorCode", 0);
	xml.AddAttrib("Value", QString::fromLocal8Bit("成功").toStdString().c_str());
	xml.OutOfElem();
	xml.AddChildElem("SvrInfo");
	xml.IntoElem();
	xml.AddAttrib("UpdateXmlUrl", MessageDataMediator->m_strUpdateXMLUrl.toStdString().c_str());
	xml.AddAttrib("HelpInfoXmlUrl", MessageDataMediator->m_strHelpXMLUrl.toStdString().c_str());
	xml.AddAttrib("VersionCheckFileUrl", MessageDataMediator->m_strClientVersionUrl.toStdString().c_str());
	xml.AddAttrib("TextMode", MessageDataMediator->m_bTextMode ? "1" : "0");
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();
}

bool CRequestXmlMessage::treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket)
{
	//请求XML时检测客户端ID,如果不存在，执行注册ID流程
	CMarkup xml;
	if(xml.SetDoc(netMessage.msgBuffer))
	{
		if(xml.FindChildElem("Info"))
		{
			xml.IntoElem();
			QString strId = QString::fromStdString(xml.GetAttrib("Id"));
			if(!DataManagerThread->IsClientExist(strId))
			{
				DataMotion dataMotion;
				dataMotion.msg = EMSG_REGISTERID;
				dataMotion.clientInfo.id = strId;
				dataMotion.clientInfo.version = ""; //yhb 版本号暂时不用
				DataManagerThread->AddDataMotion(dataMotion);
			}
		}
	}

	if(NULL != pFactory)
	{
		return pFactory->sendMessage(clientSocket, netMessage.msgHead);
	}

	return true;
}