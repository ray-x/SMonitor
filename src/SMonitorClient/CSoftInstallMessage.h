#ifndef CSoftInstallMessage_H
#define CSoftInstallMessage_H

#include "CMessage.h"
#include <QObject>

//ÃÅµê±àºÅ×¢²á
class CSoftInstallMessage : public CMessage
{
	Q_OBJECT

public:
	CSoftInstallMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CSoftInstallMessage();

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);
};

#endif // CSoftInstallMessage_H
