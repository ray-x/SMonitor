#include "CSoftDownloadThread.h"
#include "CHTTPDownloader.h"
#include "CMisc.h"

#include <QTemporaryFile>
#include <QCryptographicHash>

CSoftDownloadThread::CSoftDownloadThread(const QString& key, const QString& srcPath, const QString& desPath, 
	long startBytes, bool truncate, QObject *parent)
	: QThread(parent)
	, m_pSrcPath(m_pSrcPath)
	, m_pDesPath(m_pDesPath)
	, m_pStartBytes(startBytes)
	, m_pTruncate(truncate)
	, m_pSoftKey(key)
{
	m_pHTTPDownload = new CHTTPDownloader(this);
	connect(m_pHTTPDownload, &CHTTPDownloader::sig_DownFinished, this, &CSoftDownloadThread::downloadFinish);
	connect(m_pHTTPDownload, &CHTTPDownloader::sig_ErrorOccur, this, &CSoftDownloadThread::downloadErrorOccur);
	connect(m_pHTTPDownload, &CHTTPDownloader::sig_DownPercent, this, &CSoftDownloadThread::loadPercent);
}

CSoftDownloadThread::~CSoftDownloadThread()
{

}

void CSoftDownloadThread::run()
{
	//此处建立零时文件，记录当前下载文件的基础信息
	m_pHTTPDownload->downloadFileSync(m_pSrcPath, m_pDesPath, m_pStartBytes, m_pTruncate);
}

void CSoftDownloadThread::downloadFinish()
{
	emit downloadFinished(m_pSoftKey);
}

void CSoftDownloadThread::loadPercent(long lDownLoadByte, int totalBytes)
{
	//创建零时文件，记录当前下载数据
	QString temp = CMisc::getFileTemporary(m_pDesPath);
	QTemporaryFile tempFile(temp);
	tempFile.setAutoRemove(false);
	if(tempFile.open())
	{
		QDataStream out(&tempFile);
		out.setVersion(QDataStream::Qt_5_3);
		out << m_pSrcPath << lDownLoadByte << totalBytes;
		tempFile.close();
	}
	emit downloadPercent(m_pSoftKey, lDownLoadByte, totalBytes);
}

void CSoftDownloadThread::downloadErrorOccur(const QString& msg)
{
	emit downloadErrorOccured(m_pSoftKey, msg);
}
