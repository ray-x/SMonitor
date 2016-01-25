#ifndef CSoftUploadMessage_H
#define CSoftUploadMessage_H

#include "CMessage.h"
#include <QObject>

//ÃÅµê±àºÅ×¢²á
class CSoftUploadMessage : public CMessage
{
	Q_OBJECT

public:
	CSoftUploadMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CSoftUploadMessage();

	virtual bool packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CNetClt* pNet);

private:
	QString m_strSoft;
};

#endif // CSoftUploadMessage_H
