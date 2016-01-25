#ifndef CREGISTERIDMESSAGE_H
#define CREGISTERIDMESSAGE_H

#include "CMessage.h"
#include <QObject>

//�ŵ���ע��
class CRegisterIdMessage : public CMessage
{
	Q_OBJECT

public:
	CRegisterIdMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CRegisterIdMessage();

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);
};

#endif // CREGISTERIDMESSAGE_H
