#ifndef CLOGININMESSAGE_H
#define CLOGININMESSAGE_H

#include "CMessage.h"
#include <QObject>

//Ç©µ½ÏûÏ¢
class CLogininMessage : public CMessage
{
	Q_OBJECT

public:
	CLogininMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CLogininMessage();

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);
};

#endif // CLOGININMESSAGE_H
