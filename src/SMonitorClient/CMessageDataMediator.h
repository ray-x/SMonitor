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

	QString m_strOldId;             //旧的门店编号
	QString m_strNewId;             //新的门店编号
	QString m_strSoftname;          //安装好的软件名称
	QString m_strVersion;           //软件版本号

	//全局标志位
	bool    m_bIsCanConnectedTime;        //当前是否处于可连接服务器时段
	QString m_strSvrUpdateXmlUrl;         //服务器升级XML
	QString m_strHelpInfoXmlUrl;          //帮助信息XML
	QString m_strVersionCheckFile;        //客户端版本号比较文件URL
	QString m_strClientVersion;           //客户端版本号
	bool    m_bClientStartOver;           //客户端启动完毕
	bool    m_bTextMode;                  //测试模式
	bool    m_bLocalCrossLinkIsInstalled; //本地CrossLink是否已安装
	bool    m_bCrossLinkIsInstalled;

	//门店信息
	QString m_strShopId;                  //门店编号
	QString m_strUnitNum;                 //组织编号
	QString m_strUnitName;                //组织名称

public:

	//设置连接消息MAP属性值
	void setConnectMsgMapProperty(MsgHead msgHead, bool bRecved);

	//初始化连接消息MAP
	void initConnectMsgMap();

	//添加消息
	void addConnectMsgProperty(MsgHead msgHead, bool bRecved);

	//获取消息属性
	bool getConnectMsg(MsgHead msgHead, bool& bRecved);

	//判断所有消息是否全都收到
	bool isAllMsgRecved(bool bNeedHasMsg);

	//清空消息MAP
	void clearConnectMsgMap();

	//查询门店信息
	void QueryShopInfo(bool bRefreshRead);

	//是否需要上传CrossLinkRun安装信息
	bool NeedUploadCrossLinkRunState();

private:

	bool GetAccessDataPath(QString& strDataPath);

private:
	static CMessageDataMediator* m_Instance;
	QMutex                       m_ConnectMsgMapMutex;    //上传信息MAP锁
	QMap<MsgHead, bool>          m_ConnectMsgMap;
};

#define MessageDataMediator CMessageDataMediator::Instance()

#endif // CMESSAGEDATAMEDIATOR_H
