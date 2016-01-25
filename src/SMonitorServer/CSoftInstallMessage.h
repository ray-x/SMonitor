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

	virtual void packedSendMessage(NetMessage& netMessage);
	virtual bool treatMessage(const NetMessage& netMessage, CMessageFactory* pFactory, SocketContext* clientSocket);

protected:
	QString m_strName;
	QString m_strVersion;
	bool    m_bAddSuccess;
};

#endif // CSoftInstallMessage_H
