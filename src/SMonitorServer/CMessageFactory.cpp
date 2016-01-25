#include "stdafx.h"
#include "CMessageFactory.h"
#include "CMessage.h"
#include "CLogininMessage.h"
#include "CRegisterIdMessage.h"
#include "IOCPNet.h"
#include "CMessageEventMediator.h"
#include "CRequestXmlMessage.h"
#include "CModifyIdMessage.h"
#include "CSoftInstallMessage.h"
#include "CRequestHelpInfoMessage.h"
#include "CDistributeMessage.h"
#include "CRequestMsgMessage.h"
#include "CSoftUploadMessage.h"
CMessageFactory::CMessageFactory(CMessageEventMediator* pMessageEventMediator, CIOCPNet* pNet, QObject *parent)
	: QObject(parent)
{
	m_pMessageEventMediator = pMessageEventMediator;
	m_pNet = pNet;
	Init();
}

CMessageFactory::~CMessageFactory()
{
	Clear();
}

//������Ϣ
bool CMessageFactory::sendMessage(SocketContext* clientSocket, const MsgHead msgHead, bool bSendAnsyc)
{
	if(NULL == m_pNet)
		return false;

	CMessage* pMessage = dispatchMessage(msgHead);
	if(NULL == pMessage)
	{
		ServerLogger->AddLog("��֧�ֵ���Ϣ���� [%d]", msgHead);
		return false;
	}

	bool bRet = true;
	NetMessage netMessage;
	netMessage.msgHead = msgHead;
	pMessage->packedSendMessage(netMessage);

	//����ѹ��
	char* pSendBuffer = NULL;
	unsigned long lSendLength = 0;
	bRet = CNetMessageWrapper::GetNetMessageToBuffer(netMessage, pSendBuffer, lSendLength);
	RELEASE(netMessage.msgBuffer);
	if(!bRet) return false;

	QString strClientIP = inet_ntoa(clientSocket->m_ClientAddr.sin_addr);
	if(bSendAnsyc)
	{
		if(lSendLength >= MAX_BUFFER_LEN)
		{
			ServerLogger->AddLog("��ͻ���[%s]������Ϣ[%d] �ֽ���[%d]����������ֽ�����[%d]", strClientIP.toStdString().c_str(), msgHead, lSendLength, MAX_BUFFER_LEN);
			bRet = false;
		}
		else
		{
			ServerLogger->AddLog("��ͻ���[%s]������Ϣ[%d]", strClientIP.toStdString().c_str(), msgHead);
			bRet = m_pNet->SendData(clientSocket, pSendBuffer, lSendLength);
		}	
	}
	else
	{//��ʱ��֧��ͬ������ yhb
		ServerLogger->AddLog("��ͻ���[%s]������Ϣ[%d]", strClientIP.toStdString().c_str(), msgHead);
		bRet = m_pNet->SendData(clientSocket, pSendBuffer, lSendLength);
	}

	RELEASE(pSendBuffer);
	return bRet;
}

//Ⱥ����Ϣ 
bool CMessageFactory::sendMessageToAll(const MsgHead msgHead, bool bSendAnsyc/* = true*/)
{
	if(NULL == m_pNet)
		return false;

	CMessage* pMessage = dispatchMessage(msgHead);
	if(NULL == pMessage)
	{
		ServerLogger->AddLog("��֧�ֵ���Ϣ���� [%d]", msgHead);
		return false;
	}

	bool bRet = true;
	NetMessage netMessage;
	netMessage.msgHead = msgHead;
	pMessage->packedSendMessage(netMessage);

	//����ѹ��
	char* pSendBuffer = NULL;
	unsigned long lSendLength = 0;
	bRet = CNetMessageWrapper::GetNetMessageToBuffer(netMessage, pSendBuffer, lSendLength);
	RELEASE(netMessage.msgBuffer);
	if(!bRet) return false;

	if(bSendAnsyc)
	{
		if(lSendLength >= MAX_BUFFER_LEN)
		{
			ServerLogger->AddLog("Ⱥ����Ϣ[%d] �ֽ���[%d]����������ֽ�����[%d]", msgHead, lSendLength, MAX_BUFFER_LEN);
			bRet = false;
		}
		else
		{
			ServerLogger->AddLog("Ⱥ����Ϣ[%d]", msgHead);
			bRet = m_pNet->SendToClients(pSendBuffer, lSendLength);
		}	
	}
	else
	{//��ʱ��֧��ͬ������ yhb
		ServerLogger->AddLog("Ⱥ����Ϣ[%d]", msgHead);
		bRet = m_pNet->SendToClients(pSendBuffer, lSendLength);
	}

	RELEASE(pSendBuffer);
	return bRet;
}

//������Ϣ
bool CMessageFactory::treatMessage(SocketContext* clientSocket, const NetMessage& netMessage)
{
	CMessage* pMessage = dispatchMessage(netMessage.msgHead);
	if(NULL == pMessage)
	{
		ServerLogger->AddLog("�յ��ݲ�֧�ֵ���Ϣ���� [%d]", netMessage.msgHead);
		return true;    //��Ϊ�޷�ʶ�𣬱�ʶû�õ���Ϣ�������ⲿɾ��
	}

	if(NULL == netMessage.msgBuffer)
	{
		ServerLogger->AddLog("��Ϣ���� [%d] msgBuffer Ϊ�գ� ���ܴ���", netMessage.msgHead);
		return true;    //��Ϊ�޷�ʶ�𣬱�ʶû�õ���Ϣ�������ⲿɾ��
	}

	ServerLogger->AddLog("�ͻ���[%s]��Ϣ����\r\n[%s]", inet_ntoa(clientSocket->m_ClientAddr.sin_addr), netMessage.msgBuffer);

	bool bRet = pMessage->treatMessage(netMessage, this, clientSocket);
	RELEASEONLY(netMessage.msgBuffer);
	return bRet;
}

//�ַ���Ϣ
CMessage* CMessageFactory::dispatchMessage(MsgHead msgHead)
{
	//1.�����Ϣӳ��
	CMessage* pMessage = NULL;
	switch(msgHead)
	{
	case EMSG_UNKNOWN:
		pMessage = NULL;
		break;
	case EMSG_LOGININ:
		pMessage = m_pLogininMessage;
		break;
	case EMSG_REGISTERID:
		pMessage = m_pRegisterMessage;
		break;
	case EMSG_REQUESTXML:
		pMessage = m_pRequestXmlMessage;
		break;
	case EMSG_MODIFYID:
		pMessage = m_pModifyIdMessage;
		break;
	case EMSG_SOFTINSTALL:
		pMessage = m_pSoftInstallMessage;
		break;
	case EMSG_HELPINFO:
		pMessage = m_pRequestHelpInfoMessage;
		break;
	case EMSG_DISTRIBUTEMSG:
		pMessage = m_pDistributeMessage;
		break;
	case EMSG_REQUESTMSG:
		pMessage = m_pRequestMsgMessage;
		break;
	case EMSG_SOFTUPLOAD:
		pMessage = m_pSoftUploadMessage;
		break;
	default:
		break;
	}

	return pMessage;
}

void CMessageFactory::Init()
{
	//2.��ӳ�ʼ��
	m_pLogininMessage = new CLogininMessage(m_pMessageEventMediator);
	m_pRegisterMessage = new CRegisterIdMessage(m_pMessageEventMediator);
	m_pRequestXmlMessage = new CRequestXmlMessage(m_pMessageEventMediator);
	m_pModifyIdMessage = new CModifyIdMessage(m_pMessageEventMediator);
	m_pSoftInstallMessage = new CSoftInstallMessage(m_pMessageEventMediator);
	m_pRequestHelpInfoMessage = new CRequestHelpInfoMessage(m_pMessageEventMediator);
	m_pDistributeMessage = new CDistributeMessage(m_pMessageEventMediator);
	m_pRequestMsgMessage = new CRequestMsgMessage(m_pMessageEventMediator);
	m_pSoftUploadMessage = new CSoftUploadMessage(m_pMessageEventMediator);
}

void CMessageFactory::Clear()
{
	//3.����
	RELEASE(m_pLogininMessage);
	RELEASE(m_pRegisterMessage);
	RELEASE(m_pRequestXmlMessage);
	RELEASE(m_pModifyIdMessage);
	RELEASE(m_pSoftInstallMessage);
	RELEASE(m_pRequestHelpInfoMessage);
	RELEASE(m_pDistributeMessage);
	RELEASE(m_pRequestMsgMessage);
	RELEASE(m_pSoftUploadMessage);
}

