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

	virtual void packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket);
};

#endif //CRequestHelpInfoMessage_H