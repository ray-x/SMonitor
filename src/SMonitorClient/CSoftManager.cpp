#include "CSoftManager.h"
#include "CDownloadCM.h"
#include "CDecompressThread.h"
#include "CMisc.h"

#include <QTimerEvent>
#include <QFileInfo>
#include <QDebug>
#include "CClientLog.h"
#include "CMisc.h"
#include "CSoftInstallResultSendTh.h"
#include <QtCore/QCoreApplication>
CSoftManager* CSoftManager::m_Instance = nullptr;

CSoftManager* CSoftManager::Instance()
{
	if(nullptr == m_Instance)
	{
		m_Instance = new CSoftManager();
		Q_ASSERT(m_Instance);
	}

	return m_Instance;
}

CSoftManager::CSoftManager(QObject *parent)
	: QThread(parent)
{
	//connect(this, &CSoftManager::downloadFinished, this, &CSoftManager::downloadFinish);
	//connect(this, &CSoftManager::downloadErrorOccured, this, &CSoftManager::downloadErrorOccer);

	m_bIsStart = false;
	m_bThreadExit = true;
	m_bIsInstalling = false;
}

CSoftManager::~CSoftManager()
{

}

void CSoftManager::Start()
{
	if(m_bIsStart) return;

	m_bIsStart = true;
	start();
	m_bThreadExit = false;
	return;
}

void CSoftManager::stop()
{
	if(!m_bIsStart) return;

	m_bThreadExit = true;
	msleep(100);
	if(isRunning())
	{
		msleep(900);
	}
	
	terminate();

	QMutexLocker locker(&m_DecpListMutex);
// 	foreach(CDecompressThread* pDecp, m_pDecpTaskList.values())
// 	{
// 		if(NULL != pDecp)
// 		{
// 			delete pDecp;
// 			pDecp = NULL;
// 		}
// 	}
	
	m_pDecpTaskList.clear();
}

void CSoftManager::AddTask(const Task& task)
{
	{
		//保存当前的任务
		QMutexLocker locker(&m_SrcListMutex);

		if(m_pSrcTask.contains(task.key))
			return;

		m_pSrcTask.insert(task.key, task);
	}

	if(IsSelfUpdate(task) || IsPacketDownload(task))
	{//已下载，直接跳转到安装流程
		emit downloadFinished(task.key);
		SoftInstall(task.key);
	}
	else
	{//未下载，或未下载完毕，添加下载任务
		CDownloadCM* download = new CDownloadCM(this);
		connect(download, &CDownloadCM::downloadErrorOccured, this, &CSoftManager::downloadErrorOccer, Qt::QueuedConnection);
		connect(download, &CDownloadCM::downloadPercent, this, &CSoftManager::downloadPercent, Qt::QueuedConnection);
		connect(download, &CDownloadCM::downloadFinished, this, &CSoftManager::downloadFinish, Qt::QueuedConnection);
		connect(download, &CDownloadCM::downloadQuit, this, &CSoftManager::downloadQuit);
		connect(download, &CDownloadCM::downloadPercent, this, &CSoftManager::downloadAlived);
		download->addDownload(task.key, task.srcPath, task.tempPath, task.version);
		
		{
			//将下载任务添加进任务列表
			QMutexLocker locker(&m_TaskListMutex);
			m_pDownloadTaskList.insert(task.key, download);
		}

		//这里首先提交一个信号，让客户端软件表格的cellwidget处于progress状态
		emit downloadPercent(task.key, 0, 1);
	}
}

void CSoftManager::StopDownload()
{
	{
		QMutexLocker locker(&m_TaskListMutex);
		foreach(auto key, m_pDownloadTaskList.keys())
		{
			CDownloadCM* d = m_pDownloadTaskList.value(key);
			d->stopDownload();
			delete d;
			d = nullptr;
			m_pDownloadTaskList.remove(key);
		}
	}
	
	stop();
}

void CSoftManager::StopDownload(const QString& key)
{
	//取消下载或安装的时候要尝试清除所有的存在于任务列表中的任务
	{
		QMutexLocker locker(&m_TaskListMutex);
		if(m_pDownloadTaskList.contains(key))
		{
			CDownloadCM* d = m_pDownloadTaskList.value(key);
			d->stopDownload();

			delete d;
			d = nullptr;
			m_pDownloadTaskList.remove(key);
		}
	}

	{
		QMutexLocker locker(&m_SrcListMutex);
		if(m_pSrcTask.contains(key))
		{
			m_pSrcTask.remove(key);
		}
	}
	
	{
		QMutexLocker locker(&m_reStartListMutex);
		if(m_reStartTask.contains(key))
		{
			killTimer(m_reStartTask.value(key).first);
			m_reStartTask.remove(key);
		}
	}
}

void CSoftManager::decompressFinish(const QString& key)
{
	{
		QMutexLocker locker(&m_DecpListMutex);
		if(m_pDecpTaskList.contains(key))
		{
			//delete m_pDecpTaskList.value(key);
			m_pDecpTaskList.remove(key);
		}
	}
	
	//将安装结果上报
	//SoftData softData;
	//softData.name = key;
	//softData.version = m_pSrcTask.value(key).version;
	//SoftInstallResultSendManager->AddSoftData(softData);
	{
		QMutexLocker locker(&m_SrcListMutex);
		m_pSrcTask.remove(key);
	}
	
	m_bIsInstalling = false;
	emit InstallFinished(key);
}

//下载完成，需要对下载对象资源进行释放
void CSoftManager::downloadFinish(const QString& key)
{
	{
		QMutexLocker locker(&m_TaskListMutex);
		if(m_pDownloadTaskList.contains(key))
		{
			CDownloadCM* d = m_pDownloadTaskList.value(key);
			delete d;
			d = nullptr;
			m_pDownloadTaskList.remove(key);
		}
	}
	
	{
		QMutexLocker locker(&m_reStartListMutex);
		m_reStartTask.remove(key);
	}

	emit downloadFinished(key);

	//执行软件安装
	SoftInstall(m_pSrcTask.value(key).key);
}

//下载发生错误，需要进行重试，重试三次，间隔时间分别是30s、60s、120s
void CSoftManager::downloadErrorOccer(const QString&key, const QString& msg)
{
	emit downloadErrorOccured(key, msg);
	{
		QMutexLocker locker(&m_TaskListMutex);
		//首先清除之前的下载对象
		if(m_pDownloadTaskList.contains(key))
		{
			delete m_pDownloadTaskList.value(key);
			m_pDownloadTaskList.remove(key);
		}
	}
	
	//首次下载重启时间为间隔30s
	int interval = 30;
	QMutexLocker locker(&m_reStartListMutex);
	if(m_reStartTask.contains(key))
		interval = m_reStartTask.value(key).second * 2;

	if(interval < 240)
	{//次数未达到3次
		int id = startTimer(interval * 1000);

		if(!m_reStartTask.contains(key))
			m_reStartTask.insert(key, qMakePair(id, interval));
		else
			m_reStartTask[key] = qMakePair(id, interval);

		qDebug() << QString::fromLocal8Bit("下载失败，下载任务将在%1秒之后重启").arg(interval);
		emit redownloadCountdown(0, key, interval);
	}
	else
	{//次数达到三次，直接删除任务，不再自动重试
		m_reStartTask.remove(key);

		int nState = -1;
		{
			QMutexLocker locker(&m_SrcListMutex);
			if(m_pSrcTask.contains(key))
				nState = m_pSrcTask.value(key).state;
		}

		deleteTask(key, false);
		
		//提交重试下载失败信号
		emit redownloadErrorOccered(key, nState);
	}
}

void CSoftManager::timerEvent(QTimerEvent *event)
{
	int id = event->timerId();

	{
		QMutexLocker locker(&m_reStartListMutex);
		QHashIterator<QString, QPair<int, int> > iter(m_reStartTask);
		while(iter.hasNext())
		{
			iter.next();
			if(iter.value().first == id)
			{
				//一个重启timer只需要执行一次
				killTimer(id);

				qDebug() << QString::fromLocal8Bit("%1 %2下载重启")
					.arg(iter.key()).arg(iter.value().second);

				Task task;
				{
					QMutexLocker locker(&m_SrcListMutex);
					task = m_pSrcTask.value(iter.key());
					m_pSrcTask.remove(iter.key());
				}
				
				AddTask(task);

				return;
			}
		}
	}
	
	{
		QMutexLocker locker(&m_reDecompressListMutex);
		QHashIterator<QString, QPair<int, int> > iter2(m_reDecompressTask);
		while(iter2.hasNext())
		{
			iter2.next();
			if(iter2.value().first == id)
			{
				//一个重启timer只需要执行一次
				killTimer(id);

				qDebug() << QString::fromLocal8Bit("%1 %2解压重启")
					.arg(iter2.key()).arg(iter2.value().second);

				SoftInstall(iter2.key());

				return;
			}
		}
	}
}

void CSoftManager::run()
{
	while(!m_bThreadExit)
	{
		//检查安装队列
		if(m_pDecpTaskList.count() > 0 && !m_bIsInstalling)
		{
			QMutexLocker locker(&m_DecpListMutex);
			if(m_pDecpTaskList.count() <= 0)
				continue;

			QString strKey = m_pDecpTaskList.keys().first();
			CDecompressThread* pDecompressThread = m_pDecpTaskList.value(strKey);
			if(NULL != pDecompressThread)
			{
				pDecompressThread->Start(m_pSrcTask.value(strKey));
				m_bIsInstalling = true;
			}
			else
			{
				m_pDecpTaskList.remove(strKey);
			}
		}

		msleep(10);
	}

	m_bThreadExit = true;
}

//yhb 软件安装流程
bool CSoftManager::SoftInstall(const QString& key)
{
	if(m_pDecpTaskList.contains(key))
		return true;

	QMutexLocker locker(&m_DecpListMutex);
	//调用解压函数进行自动安装解压
	CDecompressThread* decompress = new CDecompressThread(this);
	connect(decompress, &CDecompressThread::decompressFinished, this, &CSoftManager::decompressFinish);
	connect(decompress, &CDecompressThread::decompressError, this, &CSoftManager::decompressError);
	connect(decompress, &CDecompressThread::quitAppliction, this, &CSoftManager::quitApplication);
	m_pDecpTaskList.insert(key, decompress);
	return true;
}

//判断安装包是否已经下载
bool CSoftManager::IsPacketDownload(const Task& task)
{
	if(task.srcPath.isEmpty() || task.tempPath.isEmpty())
		return false;

	//得到URL文件名
	QString strFileName = QFileInfo(task.srcPath).baseName();

	QString strExt = QFileInfo(task.srcPath).completeSuffix(); //得到完整后缀 
	if(!strExt.isEmpty())
	{
		strFileName += "." + strExt;
	}

	//首先判断缓存目录里是否有文件
	strFileName = task.tempPath + "\\" + strFileName;
	if(!CMisc::IsFileExist(strFileName))
		return false;

	//判断temp进度文件
	QString strTempFile = QString("%1/~%2.temp").arg(task.tempPath).arg(task.key);
	if(CMisc::IsFileExist(strTempFile))
		return false; //缓存文件存在说明下载未完成

	//读取version版本文件
	QString strVnFile = QString("%1/~%2.vn").arg(task.tempPath).arg(task.key);
	if(!CMisc::GetFileContent(strVnFile, strVnFile))
		return false;

	//比较版本
	if(strVnFile != task.version)
		return false;

	//判断目标文件是否为空，如果不为空则表示已下载完成
	return QFileInfo(strFileName).size() > 0;
}

//删除任务
void CSoftManager::deleteTask(const QString& key, bool bDeleteFile)
{
	QMutexLocker locker(&m_SrcListMutex);
	if(!m_pSrcTask.contains(key))
		return;

	if(bDeleteFile)
	{//删除文件

		const Task& task = m_pSrcTask.value(key);

		//得到URL文件名
		QString strFileName = QFileInfo(task.srcPath).baseName();
		QString strExt = QFileInfo(task.srcPath).completeSuffix(); //得到完整后缀 
		if(!strExt.isEmpty())
		{
			strFileName += "." + strExt;
		}

		//首先判断缓存目录里是否有文件
		strFileName = task.tempPath + "\\" + strFileName;

		if(CMisc::IsFileExist(strFileName))
		{
			if(!QFile::remove(strFileName))
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("删除文件[%1]失败").arg(strFileName));
			}
		}
	}

	m_pSrcTask.remove(key);
}

//判断软件是否为monitor自身的升级
bool CSoftManager::IsSelfUpdate(const Task& task)
{
	if(task.category == INDEPENDENT)
	{
		return true;
	}

	return false;
}

void CSoftManager::decompressError(const QString& key, bool bDeleteFile)
{
	m_bIsInstalling = false;
	ClientLogger->AddLog(QString::fromLocal8Bit("[%1]解压失败").arg(key));
	//清除之前解压线程
	{
		QMutexLocker locker(&m_DecpListMutex);
		if(m_pDecpTaskList.contains(key))
		{
			//delete m_pDecpTaskList.value(key);
			m_pDecpTaskList.remove(key);
		}
	}

	int nState = -1;
	{
		QMutexLocker locker(&m_SrcListMutex);
		if(m_pSrcTask.contains(key))
			nState = m_pSrcTask.value(key).state;
	}

	deleteTask(key, bDeleteFile);
	//提交重试下载失败信号
	emit redownloadErrorOccered(key, nState);
	return;
	
	qDebug() << QString::fromLocal8Bit("解压失败");
	emit decompressErrorOccured(key);

	//如果已下载文件还存在，则重新解压，如果不存在，则重新下载
	QString fileName = QFileInfo(m_pSrcTask.value(key).srcPath).fileName();
	if(CMisc::IsFileExist(m_pSrcTask.value(key).tempPath + "/" + fileName))
	{
		QMutexLocker locker(&m_reDecompressListMutex);
		//首次下载重启时间为间隔30s
		int interval = 30;
		if(m_reDecompressTask.contains(key))
			interval = m_reDecompressTask.value(key).second * 2;

		if(interval < 240)
		{//次数未达到3次
			int id = startTimer(interval * 1000);
			m_reDecompressTask.insert(key, qMakePair(id, interval));
			qDebug() << QString::fromLocal8Bit("解压失败，解压任务将在%1秒之后重启").arg(interval);
			emit redownloadCountdown(1, key, interval);
		}
		else
		{//次数达到三次，直接删除任务，不再自动重试

			ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1] 重试安装次数到三次 不再自动重试").arg(key));
			m_reDecompressTask.remove(key);

			int nState = -1;
			{
				QMutexLocker locker(&m_SrcListMutex);
				if(m_pSrcTask.contains(key))
					nState = m_pSrcTask.value(key).state;
			}

			deleteTask(key, true);
			m_bIsInstalling = false;

			//提交重试下载失败信号
			emit redownloadErrorOccered(key, nState);
		}
	}
	else
	{
		deleteTask(key, false);
		m_bIsInstalling = false;

		//开启下载流程
		AddTask(m_pSrcTask.value(key));
	}
}

//当下载失败之后重试，下载链接到服务器并成功下载时，需要处理之前的计数
void CSoftManager::downloadAlived(const QString& key, long lDownloadBytes, int totalBytes)
{
	{
		QMutexLocker locker(&m_reStartListMutex);
		if(m_reStartTask.contains(key))
		{

			killTimer(m_reStartTask.value(key).first);
			m_reStartTask.remove(key);
		}
	}
}