#ifndef CModifyIdMessage_H
#define CModifyIdMessage_H

#include "CMessage.h"
#include <QObject>

//ÃÅµê±àºÅ×¢²á
class CModifyIdMessage : public CMessage
{
	Q_OBJECT

public:
	CModifyIdMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CModifyIdMessage();

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);
};

#endif // CModifyIdMessage_H
