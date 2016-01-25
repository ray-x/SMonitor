#ifndef CMESSAGE_H
#define CMESSAGE_H
#include <QObject>
#include "CNetMessage.h"
#include "CMessageDataMediator.h"
#include "IOCPNet.h"

class CMessageFactory;
class CMessageEventMediator;
class CMarkup;
class CMessage : public QObject
{
	Q_OBJECT

public:
	CMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CMessage();

	virtual void packedSendMessage(NetMessage& netMessage) = 0;
	virtual bool treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket) = 0;

	char* packXml(CMarkup *xml);
	bool checkError(CMarkup *xml, int &errorCode, QString& errorInfo);

protected:
	CMessageEventMediator* m_pMessageEventMediator;
};

#endif // CMESSAGE_H
