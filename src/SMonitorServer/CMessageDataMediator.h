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

	//������������Ϣ
	QString m_strUpdateXMLUrl;            //����XML·��
	QString m_strHelpXMLUrl;              //������ϢXML
	QString m_strClientVersionUrl;        //�ͻ��˰汾�Ƚ��ļ�
	QString m_strDownloadStatusFileUrl;   //���ط�����״̬�ļ�·��
	int     m_nListenPort;                //���������˿�
	int     m_nMaxClientsNum;             //�ͻ��˲���������
	bool    m_bTextMode;                  //����ģʽ

	//������Ϣ
	HelpInfoMap m_HelpInfoMap; 

	MessageMap m_MessageMap;			//������������Ϣ

	//��Ϣ֪ͨ
	QString m_strDistributeMsg;           //��Ϣ
	QString m_strDistributeTime;          //����ʱ��

	//EXCEL
	QString m_strExcelSavePath;
	long    m_lExcelSavePerTime;

private:
	static CMessageDataMediator* m_Instance;
	
};

#define MessageDataMediator CMessageDataMediator::Instance()

#endif // CMESSAGEDATAMEDIATOR_H
