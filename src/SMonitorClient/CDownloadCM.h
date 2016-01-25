#ifndef CDOWNLOADCM_H
#define CDOWNLOADCM_H

#include <QObject>
#include <QFile>

class CHTTPDownloader;

//该线程负责所有的下载任务，并对下载任务进行分类
class CDownloadCM : public QObject
{
	Q_OBJECT

public:
	CDownloadCM(QObject *parent = 0);
	~CDownloadCM();

	//添加一个下载任务
	void addDownload(const QString& key, const QString& srcPath, const QString& desPath, const QString& version);

	//关闭下载
	void stopDownload();

protected:
	//virtual void run();

Q_SIGNALS:
	void downloadFinished(const QString key);
	void downloadErrorOccured(const QString&key, const QString& msg);
	void downloadPercent(const QString& key, long lDownloadBytes, int totalBytes);
	void downloadQuit(const QString& key);

private Q_SLOTS:
	void downloadFinish();
	void downloadQuited();
	void downloadErrorOccur(const QString& msg);
	void downloadProcess(long lDownLoadByte, int totalBytes);

private:
	CHTTPDownloader*	m_pHTTPDownload;	//下载线程	
	QString				m_pSoftKey;
	QString				m_pLocalSaveFile;
	QString				m_pTempFile;
	QString             m_strVersionFile;   //该文件的版本记录文件
	QFile*				m_pRCFile;			//记录下载文件位置
	bool				m_bRCFileOpend;		//
};

#endif // CCOMMONDOWNLOAD_H
