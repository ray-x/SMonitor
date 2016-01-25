#ifndef CRequestHelpInfoMessage_H
#define CRequestHelpInfoMessage_H

#include "CMessage.h"
#include <QObject>

//«Î«ÛXMLœ˚œ¢
class CRequestHelpInfoMessage : public CMessage
{
	Q_OBJECT

public:
	CRequestHelpInfoMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CRequestHelpInfoMessage();

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);
};

#endif //CRequestHelpInfoMessage_H