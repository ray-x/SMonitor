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

	virtual void packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket);

protected:
	QString m_strName;
	QString m_strVersion;
	bool    m_bAddSuccess;
	QString m_strClientInstalledResult;
};

#endif // CSoftUploadMessage_H
