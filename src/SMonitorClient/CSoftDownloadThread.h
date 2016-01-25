#ifndef CSOFTDOWNLOADTHREAD_H
#define CSOFTDOWNLOADTHREAD_H

#include <QThread>

class CHTTPDownloader;

//该线程负责所有的下载任务，并对下载任务进行分类
class CSoftDownloadThread : public QThread
{
	Q_OBJECT

public:
	CSoftDownloadThread(const QString& key, const QString& srcPath, const QString& desPath, 
		long startBytes, bool truncate, QObject *parent);
	~CSoftDownloadThread();

protected:
	virtual void run();

Q_SIGNALS:
	void downloadFinished(const QString& key);
	void downloadErrorOccured(const QString&key, const QString& msg);
	void downloadPercent(const QString& key, long lDownloadBytes, int totalBytes);

private Q_SLOTS:
	void downloadFinish();
	void downloadErrorOccur(const QString& msg);
	void loadPercent(long lDownLoadByte, int totalBytes);

private:
	CHTTPDownloader*	m_pHTTPDownload;	//
	QString				m_pSrcPath;			//下载位置(http资源位置)
	QString				m_pDesPath;			//下载到目标位置
	QString				m_pSoftKey;			//下载软件的标识
	long				m_pStartBytes;		//开始下载文件的位置
	bool				m_pTruncate;		//是否需要覆盖文件
};

#endif // CSOFTWAREDOWNLOADTHREAD_H
