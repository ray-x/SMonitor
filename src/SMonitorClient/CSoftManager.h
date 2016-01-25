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
	//安装软件流程
	bool SoftInstall(const QString& key);

	//判断安装包是否已经下载
	bool IsPacketDownload(const Task& task);

	//删除任务
	void deleteTask(const QString& key, bool bDeleteFile);

	//判断软件是否为monitor自身的升级
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
	QHash<QString, CDownloadCM*> m_pDownloadTaskList;    //下载对象map
	QHash<QString, CDecompressThread*> m_pDecpTaskList;  //解压任务
	QHash<QString, Task> m_pSrcTask;				     //任务结构体map
	QHash<QString, QPair<int, int> > m_reStartTask;      //下载重试map
	QHash<QString, QPair<int, int> > m_reDecompressTask; //解压重试

	QMutex  m_TaskListMutex; 
	QMutex  m_DecpListMutex; 
	QMutex  m_SrcListMutex; 
	QMutex  m_reStartListMutex; 
	QMutex  m_reDecompressListMutex; 
	bool    m_bIsStart;      //是否已经启动
	bool    m_bThreadExit;   //线程退出标志
	bool    m_bIsInstalling; //是否正在安装
};

#define SoftManager (CSoftManager::Instance())

#endif // CSOFTMANAGER_H
