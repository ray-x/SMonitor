#include "stdafx.h"
#include "CSoftInstallMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "CMessageFactory.h"
#include "databasemanager.h"

CSoftInstallMessage::CSoftInstallMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
	m_bAddSuccess = false;
}

CSoftInstallMessage::~CSoftInstallMessage()
{

}

void CSoftInstallMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_SOFTINSTALL_RSP");
	xml.OutOfElem();
	xml.AddChildElem("Result");
	xml.IntoElem();
	xml.AddAttrib("ErrorCode", 0);
	xml.AddAttrib("Value", QString::fromLocal8Bit(m_bAddSuccess ? "��ӳɹ�" : "���ʧ�ܣ�XML�޷�����").toStdString().c_str());
	xml.OutOfElem();
	xml.AddChildElem("Info");
	xml.IntoElem(); 
	xml.AddAttrib("Name", m_strName.toStdString().c_str());  //����������� yhb
	xml.AddAttrib("Version", m_strVersion.toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);;
	netMessage.msgLen = xml.GetDoc().length();
}

bool CSoftInstallMessage::treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket)
{
	m_bAddSuccess = false;
	CMarkup xml;
	if(xml.SetDoc(netMessage.msgBuffer))
	{
		if(xml.FindChildElem("Info"))
		{
			xml.IntoElem();
			QString strId = QString::fromStdString(xml.GetAttrib("Id"));
			m_strName = QString::fromStdString(xml.GetAttrib("Name"));
			m_strVersion = QString::fromStdString(xml.GetAttrib("Version"));

			 /*&&  �жϵ�ǰ�ύ��������ƣ��汾���Ƿ�������б����һ�� */
			if(DatabaseManager::Instance()->IsSoftwareExist(m_strName, m_strVersion) && !DataManagerThread->IsSoftInstallInfoExist(strId, m_strName, m_strVersion))
			{
				//���ϣ�����б�Ϊ�����б��������ݿ��жϣ���ʡЧ�� yhb

				DataMotion dataMotion;
				dataMotion.msg = netMessage.msgHead;
				dataMotion.clientInfo.id = strId;
				dataMotion.softwareInfo.name = m_strName;
				dataMotion.softwareInfo.version = m_strVersion;
				DataManagerThread->AddDataMotion(dataMotion);
			}
			xml.OutOfElem();
			m_bAddSuccess = true;
		}
	}

	if(NULL != pFactory)
	{
		return pFactory->sendMessage(clientSocket, netMessage.msgHead);
	}

	return true;
}