#ifndef CMESSAGEDATAMEDIATOR_H
#define CMESSAGEDATAMEDIATOR_H

#include <QObject>

class CMessageDataMediator : public QObject
{
	Q_OBJECT

public:
	CMessageDataMediator(QObject *parent = 0);
	~CMessageDataMediator();

	static CMessageDataMediator* Instance();

	//服务器配置信息
	QString m_strUpdateXMLUrl;            //升级XML路径
	QString m_strHelpXMLUrl;              //帮助信息XML
	QString m_strClientVersionUrl;        //客户端版本比较文件
	QString m_strDownloadStatusFileUrl;   //下载服务器状态文件路径
	int     m_nListenPort;                //本机监听端口
	int     m_nMaxClientsNum;             //客户端并发连接数
	bool    m_bTextMode;                  //测试模式

	//帮助信息
	HelpInfoMap m_HelpInfoMap; 

	MessageMap m_MessageMap;			//服务器推送消息

	//消息通知
	QString m_strDistributeMsg;           //消息
	QString m_strDistributeTime;          //推送时间

	//EXCEL
	QString m_strExcelSavePath;
	long    m_lExcelSavePerTime;

private:
	static CMessageDataMediator* m_Instance;
	
};

#define MessageDataMediator CMessageDataMediator::Instance()

#endif // CMESSAGEDATAMEDIATOR_H
