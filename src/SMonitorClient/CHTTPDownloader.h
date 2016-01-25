#ifndef CHTTPDOWNLOADER_H
#define CHTTPDOWNLOADER_H

#include <QThread>
class CHTTPBase;
class CHTTPDownloader : public QThread
{
	Q_OBJECT

public:
	friend class CHTTPBase;
	CHTTPDownloader(QObject *parent = 0);
	~CHTTPDownloader();
	bool       m_bStop;   // 用来通知线程系统退出的事件，为了能够更好的退出线程

	//下载文件
	//strFileUrl - 文件URL strLocalSavePath - 文件本地存储位置 lDownloadStartBytes - 文件下载开始位置 dwTransferBlockSize - 每次传输的期望速度 bOverWrite - 该次写入是否擦除文件
	void downloadFileSync(const QString strFileUrl, const QString strLocalSavePath, long lDownloadStartBytes, bool bOverWrite, long lTransferBlockSize = 51200);

	//停止下载
	void stopDownload();

Q_SIGNALS:
	void sig_DownPercent(long lDownloadBytes, int totalBytes);
	void sig_DownFinished();
	void sig_DownQuit();
	void sig_ErrorOccur(const QString& errMsg); //发生错误， 下载会自行中断，界面需要提示

public slots:
	void errorOccur(const QString& errMsg);

public:
	void OpenConnection();

protected:
	virtual void run();

	QString    m_strFileUrl;
	QString    m_strLocalSacePath;
	long       m_lDownLoadStartBytes;
	long       m_lTransferBlockSize;
	bool       m_bOverWrite;
	CHTTPBase* m_pHttpbase;
	QString    m_strErrMsg;
	bool       m_bOccurError;
};

#endif // CHTTPDOWNLOADER_H
