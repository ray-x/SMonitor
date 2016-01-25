#pragma once
#include <QThread>
typedef struct _LOGINFO
{
	_LOGINFO()
	{
		strDataTime = "";
		strLogInfo  = "";
	}
	QString strDataTime;
	QString strLogInfo;
}LOGINFO,*PLOGINFO;

class CLogManager : public QThread
{
public:
	CLogManager(QObject *parent = 0);
	virtual ~CLogManager(void);

	//������־ϵͳ
	bool Start(const QString& strPath);

	//�����־
	void AddLog(const QString& strLog);

	//�ͷ���Դ
	void Clear();

	//�����־�б�
	bool CheckLogInfoList();

	//д��־
	void LoggerWrite();

	QList<LOGINFO> m_TempLogInfoList;       //��д����־�б�
	QMutex         m_TempLogInfoListMutex;  //��д����־�б������
	bool           m_bIsStart;              //�Ƿ��Ѿ�����
	QString        m_strLogInfo;            //��ǰ��Ҫ�������־
	QString        m_strPath;               //��־·��
	QString        m_strLastLog;            //���һ������¼����־��Ϣ
	bool           m_bThreadExit;           //�߳��˳���־

protected:
	virtual void run();
};

