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

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);
};

#endif //CRequestMsgMessage_H