#ifndef CMESSAGE_H
#define CMESSAGE_H
#include "CNetStruct.h"
#include <QObject>
#include "../SMonitorServer/CNetMessage.h"
#include "CMessageDataMediator.h"
class CMessageEventMediator;
class CMarkup;
class CMessage : public QObject
{
	Q_OBJECT

public:
	CMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CMessage();

	virtual bool packedSendMessage(NetMessage& netMessage) = 0;
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet) = 0;

	char* packXml(CMarkup *xml);
	bool checkError(CMarkup *xml, int &errorCode, QString& errorInfo);

protected:
	CMessageEventMediator* m_pMessageEventMediator;
};

#endif // CMESSAGE_H
