#include "CSoftInstallMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
CSoftInstallMessage::CSoftInstallMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{

}

CSoftInstallMessage::~CSoftInstallMessage()
{
}

bool CSoftInstallMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_SOFTINSTALL_REQ");
	xml.OutOfElem();
	xml.AddChildElem("Info");
	xml.IntoElem();
	xml.AddAttrib("Id", MessageDataMediator->m_strNewId.toStdString().c_str());
	xml.AddAttrib("Name", MessageDataMediator->m_strSoftname.toStdString().c_str());
	xml.AddAttrib("Version", MessageDataMediator->m_strVersion.toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();
	return true;
}

bool CSoftInstallMessage::treatMessage(const NetMessage& netMessage, CNetClt* pNet)
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
				ClientLogger->AddLog(QString::fromLocal8Bit("SoftInstall [%1] : \r\n[%2]").arg(errorInfo).arg(netMessage.msgBuffer));
				emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, errorInfo);
				return true;
			}
		}

		if(xml.FindChildElem("Info"))
		{
			xml.IntoElem();
			QString strName = QString::fromStdString(xml.GetAttrib("Name"));
			QString strVersion = QString::fromStdString(xml.GetAttrib("Version"));
			xml.OutOfElem();

			ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1] 版本号[%2] 安装结果被接收").arg(strName).arg(strVersion));
			emit NetClass->m_pMessageEventMediator->sigServerHasGetSoftInstall(strName, strVersion);
		}
		else
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("SoftInstall收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
			emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
		}
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("SoftInstall收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
		emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
	}

	return true;
}