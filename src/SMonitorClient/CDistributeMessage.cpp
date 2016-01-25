#include "CDistributeMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
CDistributeMessage::CDistributeMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CDistributeMessage::~CDistributeMessage()
{

}

bool CDistributeMessage::packedSendMessage(NetMessage& netMessage)
{
	return false;
}

bool CDistributeMessage::treatMessage(const NetMessage& netMessage, CNetClt* pNet)
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
				ClientLogger->AddLog(QString::fromLocal8Bit("DistributeMessage [%1] : \r\n[%2]").arg(errorInfo).arg(netMessage.msgBuffer));
				emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, errorInfo);
				return true;
			}
		}

		if(xml.FindChildElem("Info"))
		{
			xml.IntoElem();
			QString strMsg = QString::fromStdString(xml.GetAttrib("Msg"));
			QString strTime = QString::fromStdString(xml.GetAttrib("Time"));
			xml.OutOfElem();

			ClientLogger->AddLog(QString::fromLocal8Bit("服务器 推送通知[%1] 推送时间[%2]").arg(strMsg).arg(strTime));
			emit NetClass->m_pMessageEventMediator->sigServerDistributeMsg(strMsg, strTime);
		}
		else
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("DistributeMessage收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
			emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
		}
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("DistributeMessage收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
		emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
	}

	return true;
}