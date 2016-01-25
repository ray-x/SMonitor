#ifndef CREQUESTXMLMESSAGE_H
#define CREQUESTXMLMESSAGE_H

#include "CMessage.h"
#include <QObject>

//«Î«ÛXMLœ˚œ¢
class CRequestXmlMessage : public CMessage
{
	Q_OBJECT

public:
	CRequestXmlMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CRequestXmlMessage();

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);
};

#endif //CREQUESTXMLMESSAGE_H