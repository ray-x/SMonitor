#ifndef CMESSAGEMEDIATOR_H
#define CMESSAGEMEDIATOR_H

#include <QObject>
#include <QMap>
#include "../SMonitorServer/CNetMessage.h"

typedef QMap<QString, QString> StringMap;

class CMessageEventMediator : public QObject
{
	Q_OBJECT

public:
	CMessageEventMediator(QObject *parent = 0);
	~CMessageEventMediator();

Q_SIGNALS:
	void sigLoginIn();
	void sigError(int nMsgHead, const QString& strError);
	void sigGetXml(const QString& strUpdateXmlPath, const QString& strHelpInfoXmlPath, const QString& strVersionCheckFile);
	void sigRegisterSuccess();
	void sigModifySuccess();
	void sigServerHasGetSoftInstall(const QString& name, const QString& version);
	void sigGetHelpInfoSucess(const StringMap&);
	void sigServerDistributeMsg(const QString& msg, const QString& time); //服务器推送过来消息通知
	void sigGetDistributeMsg(const QString& msg, const QString& time);    //客户端轮询获取消息通知
};



#endif // CMESSAGEMEDIATOR_H
