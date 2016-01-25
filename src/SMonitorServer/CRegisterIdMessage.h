#ifndef CREGISTERIDMESSAGE_H
#define CREGISTERIDMESSAGE_H

#include "CMessage.h"
#include <QObject>

//ÃÅµê±àºÅ×¢²á
class CRegisterIdMessage : public CMessage
{
	Q_OBJECT

public:
	CRegisterIdMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CRegisterIdMessage();

	virtual void packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket);

protected:
	bool m_bIdExist;
};

#endif // CREGISTERIDMESSAGE_H
