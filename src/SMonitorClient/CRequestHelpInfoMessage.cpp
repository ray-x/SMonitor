#include "CRequestHelpInfoMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"

#include <QMap>

CRequestHelpInfoMessage::CRequestHelpInfoMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CRequestHelpInfoMessage::~CRequestHelpInfoMessage()
{

}

bool CRequestHelpInfoMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_HELPINFO_REQ");
	xml.OutOfElem();
	xml.AddChildElem("Info");
	xml.IntoElem();
	xml.AddAttrib("Id", MessageDataMediator->m_strNewId.toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();
	return true;
}

bool CRequestHelpInfoMessage::treatMessage(const NetMessage& netMessage, CNetClt* pNet)
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
				ClientLogger->AddLog(QString::fromLocal8Bit("RequestHelpInfo [%1] : \r\n[%2]").arg(errorInfo).arg(netMessage.msgBuffer));
				emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, errorInfo);
				return true;
			}
		}

		if(xml.FindChildElem("Info"))
		{
			xml.IntoElem();

			QMap<QString, QString> infoMap;
			//获取HelpInfo.... sbb
			while(xml.FindChildElem("HelpInfo"))
			{
				xml.IntoElem();
				infoMap.insert(QString::fromStdString(xml.GetAttrib("Label")), 
					QString::fromStdString(xml.GetAttrib("Value")));
				xml.OutOfElem();
			}
			xml.OutOfElem();

			//提交成功信号.... sbb
			qRegisterMetaType<StringMap>("StringMap");  
			emit NetClass->m_pMessageEventMediator->sigGetHelpInfoSucess(infoMap);
		}
		else
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("RequestHelpInfo收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
			emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
		}
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("RequestHelpInfo收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
		emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
	}

	return true;
}