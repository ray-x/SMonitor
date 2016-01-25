#ifndef CSOFTINSTALLRESULTSENDTH_H
#define CSOFTINSTALLRESULTSENDTH_H

#include <QThread>
#include "common.h"

class CSoftInstallResultSendThread : public QThread
{
	Q_OBJECT

public:
	CSoftInstallResultSendThread(QObject *parent = 0);
	~CSoftInstallResultSendThread();

	static CSoftInstallResultSendThread* Instance();

	//����
	bool Start(const QString& strPath);

	//���
	void AddSoftData(const SoftData& softData);

public Q_SLOTS:
	void serverHasGetSoftInstall(const QString& name, const QString& version);

protected:

	//�ͷ���Դ
	void Clear();

	//����б�
	bool CheckSoftDataList();

	//�ϴ�
	void Update();

	//���ļ��л�ԭ�ϴ����е�list
	bool ReloadFromFile();

	//��¼���ļ�
	bool WriteToFile(const SoftData& softData);

	//�ı���Ϣ
	bool ChangeInfoInFile(const SoftData& softData);

	//ɾ��һ����Ϣ
	bool DeleteInfoInFile(const SoftData& softData);

	virtual void run();

private:

	static CSoftInstallResultSendThread* m_Instance;
	QList<SoftData>    m_TempSoftDataList;      //���ϴ��б�
	QMutex             m_TempSoftDataListMutex; //���ϴ��б������
	bool               m_bIsStart;              //�Ƿ��Ѿ�����
	QString            m_strLogInfo;            //��ǰ��Ҫ�������־
	QString            m_strPath;               //��־·��
	bool               m_bThreadExit;           //�߳��˳���־
	int                m_nHasWaitSendTime;      //�Ѿ��ȴ����´η���ʱ�� 
	int                m_nSendWaitTime;         //���ͳ�ʱ
	SoftData           m_nCurSendSoftData;      //���ڷ��͵������Ϣ
	bool               m_bIsTreatSendReceive;   //�Ƿ����ڴ����ͽ���
};

#define SoftInstallResultSendManager CSoftInstallResultSendThread::Instance()

#endif // CSOFTINSTALLRESULTSENDTH_H
