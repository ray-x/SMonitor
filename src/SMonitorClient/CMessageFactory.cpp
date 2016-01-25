#include "CMessageFactory.h"
#include "CNetStruct.h"
#include "CMessage.h"
#include "CLogininMessage.h"
#include "CRegisterIdMessage.h"
#include "NetClt.h"
#include "CMessageEventMediator.h"
#include "CRequestXmlMessage.h"
#include "CModifyIdMessage.h"
#include "CSoftInstallMessage.h"
#include "CRequestHelpInfoMessage.h"
#include "CDistributeMessage.h"
#include "CRequestMsgMessage.h"
#include "CSoftUploadMessage.h"
CMessageFactory::CMessageFactory(CMessageEventMediator* pMessageEventMediator, CNetClt* pNet, QObject *parent)
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
bool CMessageFactory::sendMessage(const MsgHead msgHead, bool bSendAnsyc)
{
	if(NULL == m_pNet)
	{
		return false;
	}

	CMessage* pMessage = dispatchMessage(msgHead);
	if(NULL == pMessage)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��֧�ֵ���Ϣ���� [%1]").arg(msgHead));
		return false;
	}

	bool bRet = true;
	NetMessage netMessage;
	netMessage.msgHead = msgHead;
	if(!pMessage->packedSendMessage(netMessage))
	{
		RELEASE(netMessage.msgBuffer);
		return false;
	}

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
			ClientLogger->AddLog(QString::fromLocal8Bit("��Ϣ[%1] �ֽ���[%2]����������ֽ�����[%3]").arg(msgHead).arg(lSendLength).arg(MAX_BUFFER_LEN));
			bRet = false;
		}
		else
		{
			Buffer buffer;
			buffer.msgHead = msgHead;
			buffer.nBufferSize = lSendLength;
			memcpy(buffer.szBuf, pSendBuffer, lSendLength);
			bRet = m_pNet->SendToSvrAnsyc(buffer);
		}	
	}
	else
	{
		bRet = m_pNet->SendToSvr(pSendBuffer, lSendLength);
	}

	RELEASE(pSendBuffer);
	return bRet;
}

//������Ϣ
bool CMessageFactory::treatMessage(const NetMessage& netMessage)
{
	CMessage* pMessage = dispatchMessage(netMessage.msgHead);
	if(NULL == pMessage)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("�յ��ݲ�֧�ֵ���Ϣ���� [%1]").arg(netMessage.msgHead));
		return true;    //��Ϊ�޷�ʶ�𣬱�ʶû�õ���Ϣ�������ⲿɾ��
	}

	if(NULL == netMessage.msgBuffer)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��Ϣ���� [%1] msgBuffer Ϊ�գ� ���ܴ���").arg(netMessage.msgHead));
		return true;    //��Ϊ�޷�ʶ�𣬱�ʶû�õ���Ϣ�������ⲿɾ��
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("��Ϣ����\r\n[%1]").arg(netMessage.msgBuffer));
	bool bRet = pMessage->treatMessage(netMessage, m_pNet);
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
	case EMSG_MODIFYID:
		pMessage = m_pModifyIdMessage;
		break;
	case EMSG_REQUESTXML:
		pMessage = m_pRequestXmlMessage;
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

