#ifndef CMESSAGEDATAMEDIATOR_H
#define CMESSAGEDATAMEDIATOR_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include "../SMonitorServer/CNetMessage.h"

class CMessageDataMediator : public QObject
{
	Q_OBJECT

public:
	CMessageDataMediator(QObject *parent = 0);
	~CMessageDataMediator();

	static CMessageDataMediator* Instance();

	QString m_strOldId;             //�ɵ��ŵ���
	QString m_strNewId;             //�µ��ŵ���
	QString m_strSoftname;          //��װ�õ��������
	QString m_strVersion;           //����汾��

	//ȫ�ֱ�־λ
	bool    m_bIsCanConnectedTime;        //��ǰ�Ƿ��ڿ����ӷ�����ʱ��
	QString m_strSvrUpdateXmlUrl;         //����������XML
	QString m_strHelpInfoXmlUrl;          //������ϢXML
	QString m_strVersionCheckFile;        //�ͻ��˰汾�űȽ��ļ�URL
	QString m_strClientVersion;           //�ͻ��˰汾��
	bool    m_bClientStartOver;           //�ͻ����������
	bool    m_bTextMode;                  //����ģʽ
	bool    m_bLocalCrossLinkIsInstalled; //����CrossLink�Ƿ��Ѱ�װ
	bool    m_bCrossLinkIsInstalled;

	//�ŵ���Ϣ
	QString m_strShopId;                  //�ŵ���
	QString m_strUnitNum;                 //��֯���
	QString m_strUnitName;                //��֯����

public:

	//����������ϢMAP����ֵ
	void setConnectMsgMapProperty(MsgHead msgHead, bool bRecved);

	//��ʼ��������ϢMAP
	void initConnectMsgMap();

	//�����Ϣ
	void addConnectMsgProperty(MsgHead msgHead, bool bRecved);

	//��ȡ��Ϣ����
	bool getConnectMsg(MsgHead msgHead, bool& bRecved);

	//�ж�������Ϣ�Ƿ�ȫ���յ�
	bool isAllMsgRecved(bool bNeedHasMsg);

	//�����ϢMAP
	void clearConnectMsgMap();

	//��ѯ�ŵ���Ϣ
	void QueryShopInfo(bool bRefreshRead);

	//�Ƿ���Ҫ�ϴ�CrossLinkRun��װ��Ϣ
	bool NeedUploadCrossLinkRunState();

private:

	bool GetAccessDataPath(QString& strDataPath);

private:
	static CMessageDataMediator* m_Instance;
	QMutex                       m_ConnectMsgMapMutex;    //�ϴ���ϢMAP��
	QMap<MsgHead, bool>          m_ConnectMsgMap;
};

#define MessageDataMediator CMessageDataMediator::Instance()

#endif // CMESSAGEDATAMEDIATOR_H
