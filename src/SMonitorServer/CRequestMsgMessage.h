#ifndef CRequestMsgMessage_H
#define CRequestMsgMessage_H

#include "CMessage.h"
#include <QObject>

//ÇëÇóXMLÏûÏ¢
class CRequestMsgMessage : public CMessage
{
	Q_OBJECT

public:
	CRequestMsgMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CRequestMsgMessage();

	virtual void packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket);
};

#endif //CRequestMsgMessage_H