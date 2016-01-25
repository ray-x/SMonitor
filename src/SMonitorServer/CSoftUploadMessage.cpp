#include "stdafx.h"
#include "CSoftUploadMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "CMessageFactory.h"
#include "databasemanager.h"

CSoftUploadMessage::CSoftUploadMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{
	m_bAddSuccess = false;
	m_strClientInstalledResult = "";
}

CSoftUploadMessage::~CSoftUploadMessage()
{ 

}

void CSoftUploadMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Header");
	xml.IntoElem();
	xml.AddAttrib("MsgType", "EMSG_SOFTUPLOAD_RSP");
	xml.OutOfElem();
	xml.AddChildElem("Result");
	xml.IntoElem();
	xml.AddAttrib("ErrorCode", 0);
	xml.AddAttrib("Value", QString::fromLocal8Bit(m_bAddSuccess ? "添加成功" : "添加失败，XML无法解析").toStdString().c_str());
	xml.AddAttrib("CorrsLinkRunState", m_strClientInstalledResult.toStdString().c_str());
	xml.OutOfElem();

	netMessage.msgBuffer = packXml(&xml);;
	netMessage.msgLen = xml.GetDoc().length();
}

bool CSoftUploadMessage::treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket)
{
	m_bAddSuccess = false;
	m_strClientInstalledResult = "";
	CMarkup xml;
	if(xml.SetDoc(netMessage.msgBuffer))
	{
		if(xml.FindChildElem("InfoList"))
		{
			xml.IntoElem();
			QString strId = QString::fromStdString(xml.GetAttrib("Id"));
			if(!DataManagerThread->IsSoftInfoExist(strId) && DatabaseManager::Instance()->IsClientExist(strId))
			{
				DataMotion dataMotion;
				dataMotion.msg = netMessage.msgHead;
				dataMotion.clientInfo.id = strId;
				while(xml.FindChildElem("Software"))
				{
					xml.IntoElem();
					SoftwareInfo softwareInfo;
					softwareInfo.name = QString::fromStdString(xml.GetAttrib("Name"));
					softwareInfo.version = QString::fromStdString(xml.GetAttrib("Version"));
					softwareInfo.category = QString::fromStdString(xml.GetAttrib("Category"));
					dataMotion.softwareInfoVector.push_back(softwareInfo);
					xml.OutOfElem();
				}
				xml.OutOfElem();

				if(dataMotion.softwareInfoVector.count() > 0)
				{
					DataManagerThread->AddDataMotion(dataMotion);
				}

				if(xml.FindChildElem("ExtInfo"))
				{
					xml.IntoElem();
					QString strShopId = QString::fromStdString(xml.GetAttrib("UnitNum"));
					QString strClientInstalledResult = QString::fromStdString(xml.GetAttrib("CrosslinkRunInstalledState"));
					if(!strShopId.isEmpty())
					{
						int nMotion = 0; //默认应该是插入2，但是不做插入，只更新 
						if(DatabaseManager::Instance()->GetCrosslinkRunInstalledResult(strShopId, m_strClientInstalledResult))
						{
							if(strClientInstalledResult == m_strClientInstalledResult)
							{//服务器数据和上报数据一致，不更新
								nMotion = 0; //不更新
							}
							else
							{//服务器数据和上报数据不一致，更新
								nMotion = 1; //更新
							}
						}

						if(0 != nMotion)
						{
							DataMotion dataMotion;
							dataMotion.msg = EMSG_CROSSRUNINFO;
							dataMotion.userInfoMap.insert("UnitNum", QString::fromStdString(xml.GetAttrib("UnitNum")));
							dataMotion.userInfoMap.insert("UnitName", QString::fromStdString(xml.GetAttrib("UnitName")));
							dataMotion.userInfoMap.insert("CrosslinkRunInstalledState", strClientInstalledResult);
							dataMotion.userInfoMap.insert("Motion", QString::fromLocal8Bit("%1").arg(nMotion));
							DataManagerThread->AddDataMotion(dataMotion);
						}
					}
					xml.OutOfElem();
				}
			}

			m_bAddSuccess = true;
		}
	}

	if(NULL != pFactory)
	{
		return pFactory->sendMessage(clientSocket, netMessage.msgHead);
	}

	return true;
}