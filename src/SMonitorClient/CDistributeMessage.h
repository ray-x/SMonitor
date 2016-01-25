#ifndef CDistributeMessage_H
#define CDistributeMessage_H

#include "CMessage.h"
#include <QObject>

//ÇëÇóXMLÏûÏ¢
class CDistributeMessage : public CMessage
{
	Q_OBJECT

public:
	CDistributeMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CDistributeMessage();

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);
};

#endif //CDistributeMessage_H