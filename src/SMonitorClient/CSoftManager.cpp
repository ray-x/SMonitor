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
		//���浱ǰ������
		QMutexLocker locker(&m_SrcListMutex);

		if(m_pSrcTask.contains(task.key))
			return;

		m_pSrcTask.insert(task.key, task);
	}

	if(IsSelfUpdate(task) || IsPacketDownload(task))
	{//�����أ�ֱ����ת����װ����
		emit downloadFinished(task.key);
		SoftInstall(task.key);
	}
	else
	{//δ���أ���δ������ϣ������������
		CDownloadCM* download = new CDownloadCM(this);
		connect(download, &CDownloadCM::downloadErrorOccured, this, &CSoftManager::downloadErrorOccer, Qt::QueuedConnection);
		connect(download, &CDownloadCM::downloadPercent, this, &CSoftManager::downloadPercent, Qt::QueuedConnection);
		connect(download, &CDownloadCM::downloadFinished, this, &CSoftManager::downloadFinish, Qt::QueuedConnection);
		connect(download, &CDownloadCM::downloadQuit, this, &CSoftManager::downloadQuit);
		connect(download, &CDownloadCM::downloadPercent, this, &CSoftManager::downloadAlived);
		download->addDownload(task.key, task.srcPath, task.tempPath, task.version);
		
		{
			//������������ӽ������б�
			QMutexLocker locker(&m_TaskListMutex);
			m_pDownloadTaskList.insert(task.key, download);
		}

		//���������ύһ���źţ��ÿͻ����������cellwidget����progress״̬
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
	//ȡ�����ػ�װ��ʱ��Ҫ����������еĴ����������б��е�����
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
	
	//����װ����ϱ�
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

//������ɣ���Ҫ�����ض�����Դ�����ͷ�
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

	//ִ�������װ
	SoftInstall(m_pSrcTask.value(key).key);
}

//���ط���������Ҫ�������ԣ��������Σ����ʱ��ֱ���30s��60s��120s
void CSoftManager::downloadErrorOccer(const QString&key, const QString& msg)
{
	emit downloadErrorOccured(key, msg);
	{
		QMutexLocker locker(&m_TaskListMutex);
		//�������֮ǰ�����ض���
		if(m_pDownloadTaskList.contains(key))
		{
			delete m_pDownloadTaskList.value(key);
			m_pDownloadTaskList.remove(key);
		}
	}
	
	//�״���������ʱ��Ϊ���30s
	int interval = 30;
	QMutexLocker locker(&m_reStartListMutex);
	if(m_reStartTask.contains(key))
		interval = m_reStartTask.value(key).second * 2;

	if(interval < 240)
	{//����δ�ﵽ3��
		int id = startTimer(interval * 1000);

		if(!m_reStartTask.contains(key))
			m_reStartTask.insert(key, qMakePair(id, interval));
		else
			m_reStartTask[key] = qMakePair(id, interval);

		qDebug() << QString::fromLocal8Bit("����ʧ�ܣ�����������%1��֮������").arg(interval);
		emit redownloadCountdown(0, key, interval);
	}
	else
	{//�����ﵽ���Σ�ֱ��ɾ�����񣬲����Զ�����
		m_reStartTask.remove(key);

		int nState = -1;
		{
			QMutexLocker locker(&m_SrcListMutex);
			if(m_pSrcTask.contains(key))
				nState = m_pSrcTask.value(key).state;
		}

		deleteTask(key, false);
		
		//�ύ��������ʧ���ź�
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
				//һ������timerֻ��Ҫִ��һ��
				killTimer(id);

				qDebug() << QString::fromLocal8Bit("%1 %2��������")
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
				//һ������timerֻ��Ҫִ��һ��
				killTimer(id);

				qDebug() << QString::fromLocal8Bit("%1 %2��ѹ����")
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
		//��鰲װ����
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

//yhb �����װ����
bool CSoftManager::SoftInstall(const QString& key)
{
	if(m_pDecpTaskList.contains(key))
		return true;

	QMutexLocker locker(&m_DecpListMutex);
	//���ý�ѹ���������Զ���װ��ѹ
	CDecompressThread* decompress = new CDecompressThread(this);
	connect(decompress, &CDecompressThread::decompressFinished, this, &CSoftManager::decompressFinish);
	connect(decompress, &CDecompressThread::decompressError, this, &CSoftManager::decompressError);
	connect(decompress, &CDecompressThread::quitAppliction, this, &CSoftManager::quitApplication);
	m_pDecpTaskList.insert(key, decompress);
	return true;
}

//�жϰ�װ���Ƿ��Ѿ�����
bool CSoftManager::IsPacketDownload(const Task& task)
{
	if(task.srcPath.isEmpty() || task.tempPath.isEmpty())
		return false;

	//�õ�URL�ļ���
	QString strFileName = QFileInfo(task.srcPath).baseName();

	QString strExt = QFileInfo(task.srcPath).completeSuffix(); //�õ�������׺ 
	if(!strExt.isEmpty())
	{
		strFileName += "." + strExt;
	}

	//�����жϻ���Ŀ¼���Ƿ����ļ�
	strFileName = task.tempPath + "\\" + strFileName;
	if(!CMisc::IsFileExist(strFileName))
		return false;

	//�ж�temp�����ļ�
	QString strTempFile = QString("%1/~%2.temp").arg(task.tempPath).arg(task.key);
	if(CMisc::IsFileExist(strTempFile))
		return false; //�����ļ�����˵������δ���

	//��ȡversion�汾�ļ�
	QString strVnFile = QString("%1/~%2.vn").arg(task.tempPath).arg(task.key);
	if(!CMisc::GetFileContent(strVnFile, strVnFile))
		return false;

	//�Ƚϰ汾
	if(strVnFile != task.version)
		return false;

	//�ж�Ŀ���ļ��Ƿ�Ϊ�գ������Ϊ�����ʾ���������
	return QFileInfo(strFileName).size() > 0;
}

//ɾ������
void CSoftManager::deleteTask(const QString& key, bool bDeleteFile)
{
	QMutexLocker locker(&m_SrcListMutex);
	if(!m_pSrcTask.contains(key))
		return;

	if(bDeleteFile)
	{//ɾ���ļ�

		const Task& task = m_pSrcTask.value(key);

		//�õ�URL�ļ���
		QString strFileName = QFileInfo(task.srcPath).baseName();
		QString strExt = QFileInfo(task.srcPath).completeSuffix(); //�õ�������׺ 
		if(!strExt.isEmpty())
		{
			strFileName += "." + strExt;
		}

		//�����жϻ���Ŀ¼���Ƿ����ļ�
		strFileName = task.tempPath + "\\" + strFileName;

		if(CMisc::IsFileExist(strFileName))
		{
			if(!QFile::remove(strFileName))
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("ɾ���ļ�[%1]ʧ��").arg(strFileName));
			}
		}
	}

	m_pSrcTask.remove(key);
}

//�ж�����Ƿ�Ϊmonitor���������
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
	ClientLogger->AddLog(QString::fromLocal8Bit("[%1]��ѹʧ��").arg(key));
	//���֮ǰ��ѹ�߳�
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
	//�ύ��������ʧ���ź�
	emit redownloadErrorOccered(key, nState);
	return;
	
	qDebug() << QString::fromLocal8Bit("��ѹʧ��");
	emit decompressErrorOccured(key);

	//����������ļ������ڣ������½�ѹ����������ڣ�����������
	QString fileName = QFileInfo(m_pSrcTask.value(key).srcPath).fileName();
	if(CMisc::IsFileExist(m_pSrcTask.value(key).tempPath + "/" + fileName))
	{
		QMutexLocker locker(&m_reDecompressListMutex);
		//�״���������ʱ��Ϊ���30s
		int interval = 30;
		if(m_reDecompressTask.contains(key))
			interval = m_reDecompressTask.value(key).second * 2;

		if(interval < 240)
		{//����δ�ﵽ3��
			int id = startTimer(interval * 1000);
			m_reDecompressTask.insert(key, qMakePair(id, interval));
			qDebug() << QString::fromLocal8Bit("��ѹʧ�ܣ���ѹ������%1��֮������").arg(interval);
			emit redownloadCountdown(1, key, interval);
		}
		else
		{//�����ﵽ���Σ�ֱ��ɾ�����񣬲����Զ�����

			ClientLogger->AddLog(QString::fromLocal8Bit("���[%1] ���԰�װ���������� �����Զ�����").arg(key));
			m_reDecompressTask.remove(key);

			int nState = -1;
			{
				QMutexLocker locker(&m_SrcListMutex);
				if(m_pSrcTask.contains(key))
					nState = m_pSrcTask.value(key).state;
			}

			deleteTask(key, true);
			m_bIsInstalling = false;

			//�ύ��������ʧ���ź�
			emit redownloadErrorOccered(key, nState);
		}
	}
	else
	{
		deleteTask(key, false);
		m_bIsInstalling = false;

		//������������
		AddTask(m_pSrcTask.value(key));
	}
}

//������ʧ��֮�����ԣ��������ӵ����������ɹ�����ʱ����Ҫ����֮ǰ�ļ���
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