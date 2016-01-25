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
	xml.AddAttrib("Value", QString::fromLocal8Bit(m_bAddSuccess ? "添加成功" : "添加失败，XML无法解析").toStdString().c_str());
	xml.OutOfElem();
	xml.AddChildElem("Info");
	xml.IntoElem(); 
	xml.AddAttrib("Name", m_strName.toStdString().c_str());  //中文问题测试 yhb
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

			 /*&&  判断当前提交的软件名称，版本号是否与软件列表里的一致 */
			if(DatabaseManager::Instance()->IsSoftwareExist(m_strName, m_strVersion) && !DataManagerThread->IsSoftInstallInfoExist(strId, m_strName, m_strVersion))
			{
				//接上，软件列表为共用列表，不从数据库判断，节省效率 yhb

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