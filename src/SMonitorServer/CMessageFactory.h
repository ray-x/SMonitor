#ifndef CMESSAGEFACTORY_H
#define CMESSAGEFACTORY_H

#include "CNetMessage.h"
#include <QObject>
#include "CMessageEventMediator.h"
#include "CMessage.h"
#include "IOCPNet.h"
class CLogininMessage;
class CRegisterIdMessage;
class CMessageEventMediator;
class CRequestXmlMessage;
class CModifyIdMessage;
class CSoftInstallMessage;
class CRequestHelpInfoMessage;
class CDistributeMessage;
class CRequestMsgMessage;
class CSoftUploadMessage;
class CMessageFactory : public QObject
{
	Q_OBJECT

public:
	CMessageFactory(CMessageEventMediator* pMessageEventMediator, CIOCPNet* pNet, QObject *parent = 0);
	~CMessageFactory();

	//������Ϣ 
	virtual bool sendMessage(SocketContext* clientSocket, const MsgHead msgHead, bool bSendAnsyc = true);

	//Ⱥ����Ϣ 
	virtual bool sendMessageToAll(const MsgHead msgHead, bool bSendAnsyc = true);

	//������Ϣ
	virtual bool treatMessage(SocketContext* clientSocket, const NetMessage& netMessage);

protected:

	//�ַ���Ϣ
	CMessage* dispatchMessage(MsgHead msgHead);

	void Init();
	void Clear();

protected:
	CLogininMessage*         m_pLogininMessage;
	CRegisterIdMessage*      m_pRegisterMessage;
	CRequestXmlMessage*      m_pRequestXmlMessage;
	CModifyIdMessage*        m_pModifyIdMessage;
	CSoftInstallMessage*     m_pSoftInstallMessage;
	CRequestHelpInfoMessage* m_pRequestHelpInfoMessage;
	CDistributeMessage*      m_pDistributeMessage;
	CRequestMsgMessage*      m_pRequestMsgMessage;
	CSoftUploadMessage*      m_pSoftUploadMessage;
private:
	CIOCPNet* m_pNet;
	CMessageEventMediator* m_pMessageEventMediator;
};

#endif // CMESSAGEFACTORY_H
