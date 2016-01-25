#include "common.h"
#include "CRequestXmlMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
#include <QSettings>
#include <QTextCodec>
CRequestXmlMessage::CRequestXmlMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
}

CRequestXmlMessage::~CRequestXmlMessage()
{

}

bool CRequestXmlMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_REQUESTXML_REQ");
	xml.OutOfElem();
	xml.AddChildElem("Info");
	xml.IntoElem();
	xml.AddAttrib("Id", MessageDataMediator->m_strNewId.toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();
	return true;
}

bool CRequestXmlMessage::treatMessage(const NetMessage& netMessage, CNetClt* pNet)
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
				ClientLogger->AddLog(QString::fromLocal8Bit("RequestXml [%1] : \r\n[%2]").arg(errorInfo).arg(netMessage.msgBuffer));
				emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, errorInfo);
				MessageDataMediator->setConnectMsgMapProperty(EMSG_REQUESTXML, true);
				return true;
			}
		}

		if(xml.FindChildElem("SvrInfo"))
		{
			xml.IntoElem();
			QString strUpdateXMLPath = QString::fromStdString(xml.GetAttrib("UpdateXmlUrl"));
			QString strHelpInfoXMLPath = QString::fromStdString(xml.GetAttrib("HelpInfoXmlUrl"));
			QString strVersionCheckFile = QString::fromStdString(xml.GetAttrib("VersionCheckFileUrl"));
			//QString strTextMode = QString::fromStdString(xml.GetAttrib("TextMode"));
			xml.OutOfElem();

// 			if(strTextMode == "1" && !MessageDataMediator->m_bTextMode) yhb
// 			{//��������ģʽ
// 				ClientLogger->AddLog(QString::fromLocal8Bit("��������ģʽ"));
// 				MessageDataMediator->m_bTextMode = true;
// 				QSettings settings(HKCU, QSettings::NativeFormat);
// 				settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
// 				settings.setValue("Connect/TextMode", strTextMode);
// 			}
// 			else if(strTextMode != "1" && MessageDataMediator->m_bTextMode)
// 			{
// 				ClientLogger->AddLog(QString::fromLocal8Bit("�رղ���ģʽ"));
// 				MessageDataMediator->m_bTextMode = false;
// 				QSettings settings(HKCU, QSettings::NativeFormat);
// 				settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
// 				settings.setValue("Connect/TextMode", strTextMode);
// 			}

			ClientLogger->AddLog(QString::fromLocal8Bit("�ӷ�������ȡ������XML·��Ϊ[%1] ������ϢXML·��Ϊ[%2] �ͻ��˰汾�űȽ��ļ�·��Ϊ[%3]").arg(strUpdateXMLPath).arg(strHelpInfoXMLPath).arg(strVersionCheckFile));
			emit NetClass->m_pMessageEventMediator->sigGetXml(strUpdateXMLPath, strHelpInfoXMLPath, strVersionCheckFile);
		}
		else
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("RequestXml�յ�����Ϣ���� �޷�����ȷ���� : [%1]").arg(netMessage.msgBuffer));
			emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("�յ�����Ϣ���� �޷�����ȷ����"));
			MessageDataMediator->setConnectMsgMapProperty(EMSG_REQUESTXML, true);
		}
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("RequestXml�յ�����Ϣ���� �޷�����ȷ���� : [%1]").arg(netMessage.msgBuffer));
		emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("�յ�����Ϣ���� �޷�����ȷ����"));
		MessageDataMediator->setConnectMsgMapProperty(EMSG_REQUESTXML, true);
	}

	return true;
}