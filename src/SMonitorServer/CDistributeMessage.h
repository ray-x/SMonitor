#ifndef CDistributeMessage_H
#define CDistributeMessage_H

#include "CMessage.h"
#include <QObject>

//消息通知
class CDistributeMessage : public CMessage
{
	Q_OBJECT

public:
	CDistributeMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CDistributeMessage();

	virtual void packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket);
};

#endif //CDistributeMessage_H