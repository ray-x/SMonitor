#ifndef CSOFTMANAGER_H
#define CSOFTMANAGER_H

#include <QObject>
#include <QHash>
#include <QMutex>
#include <QThread>
#include "common.h"

class CDownloadCM;
class CDecompressThread;

class CSoftManager : public QThread
{
	Q_OBJECT

public:

	static CSoftManager* Instance();
	virtual ~CSoftManager();

	void Start();
	void stop();

	void AddTask(const Task& task);

	void StopDownload();

	void StopDownload(const QString& key);

protected:
	virtual void timerEvent(QTimerEvent *);
	virtual void run();

Q_SIGNALS:
	void redownloadErrorOccered(const QString& key, int state);
	void downloadFinished(const QString key);
	void downloadErrorOccured(const QString&key, const QString& msg);
	void downloadPercent(const QString& key, long lDownloadBytes, int totalBytes);
	void redownloadCountdown(int type, const QString& key, int duration);
	void InstallFinished(const QString& key);
	void decompressErrorOccured(const QString& key);
	void SoftWareInstall(const QString& key);
	void quitApplication();
	void InstallCancled(const QString& key, int state);
	void downloadQuit(const QString& key);

private Q_SLOTS:
	void decompressFinish(const QString& key);
	void downloadFinish(const QString& key);
	void downloadErrorOccer(const QString&key, const QString& msg);
	void decompressError(const QString& key, bool bDeleteFile);
	void downloadAlived(const QString& key, long lDownloadBytes, int totalBytes);

private:
	//��װ�������
	bool SoftInstall(const QString& key);

	//�жϰ�װ���Ƿ��Ѿ�����
	bool IsPacketDownload(const Task& task);

	//ɾ������
	void deleteTask(const QString& key, bool bDeleteFile);

	//�ж�����Ƿ�Ϊmonitor���������
	bool IsSelfUpdate(const Task& task);

private:
	class PrivateFree
	{
	public:
		PrivateFree(){}
		~PrivateFree()
		{
			if(	CSoftManager::m_Instance)
			{
				delete CSoftManager::m_Instance;
				CSoftManager::m_Instance = nullptr;
			}
		}
	};
	static PrivateFree pFree;

	CSoftManager(QObject *parent = 0);

private:
	static CSoftManager* m_Instance;
	QHash<QString, CDownloadCM*> m_pDownloadTaskList;    //���ض���map
	QHash<QString, CDecompressThread*> m_pDecpTaskList;  //��ѹ����
	QHash<QString, Task> m_pSrcTask;				     //����ṹ��map
	QHash<QString, QPair<int, int> > m_reStartTask;      //��������map
	QHash<QString, QPair<int, int> > m_reDecompressTask; //��ѹ����

	QMutex  m_TaskListMutex; 
	QMutex  m_DecpListMutex; 
	QMutex  m_SrcListMutex; 
	QMutex  m_reStartListMutex; 
	QMutex  m_reDecompressListMutex; 
	bool    m_bIsStart;      //�Ƿ��Ѿ�����
	bool    m_bThreadExit;   //�߳��˳���־
	bool    m_bIsInstalling; //�Ƿ����ڰ�װ
};

#define SoftManager (CSoftManager::Instance())

#endif // CSOFTMANAGER_H
