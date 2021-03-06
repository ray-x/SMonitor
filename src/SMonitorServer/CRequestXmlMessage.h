#ifndef CREQUESTXMLMESSAGE_H
#define CREQUESTXMLMESSAGE_H

#include "CMessage.h"
#include <QObject>

//����XML��Ϣ
class CRequestXmlMessage : public CMessage
{
	Q_OBJECT

public:
	CRequestXmlMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CRequestXmlMessage();

	virtual void packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket);
};

#endif //CREQUESTXMLMESSAGE_H