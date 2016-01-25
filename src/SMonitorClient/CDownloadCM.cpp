#include "CDownloadCM.h"
#include "CHTTPDownloader.h"
#include "CMisc.h"
#include "CClientLog.h"
#include <Windows.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDataStream>

CDownloadCM::CDownloadCM(QObject *parent)
	: QObject(parent)
{
	m_bRCFileOpend = false;
	m_pRCFile = new QFile(this);
	m_pHTTPDownload = new CHTTPDownloader(this);

	connect(m_pHTTPDownload, &CHTTPDownloader::sig_DownFinished, this, &CDownloadCM::downloadFinish);
	connect(m_pHTTPDownload, &CHTTPDownloader::sig_ErrorOccur, this, &CDownloadCM::downloadErrorOccur);
	connect(m_pHTTPDownload, &CHTTPDownloader::sig_DownPercent, this, &CDownloadCM::downloadProcess, Qt::DirectConnection);
	connect(m_pHTTPDownload, &CHTTPDownloader::sig_DownQuit, this, &CDownloadCM::downloadQuited);
}

CDownloadCM::~CDownloadCM()
{
	qDebug() << "~CDownloadCM()";
	if(NULL != m_pHTTPDownload)
	{
		//m_pHTTPDownload->terminate();
		m_pHTTPDownload->stopDownload();
		delete m_pHTTPDownload;
		m_pHTTPDownload = nullptr;
	}

	if(NULL != m_pRCFile)
	{
		if(m_bRCFileOpend)
		{
			m_pRCFile->close();
			m_bRCFileOpend = false;
		}

		delete m_pRCFile;
		m_pRCFile = nullptr;
	}
}

void CDownloadCM::addDownload(const QString& key, const QString& srcPath, const QString& desPath, const QString& version)
{
	m_pSoftKey = key;
	m_pLocalSaveFile = desPath;
	m_pTempFile = QString("%1/~%2.temp").arg(desPath).arg(key);

	qlonglong downloadByte = 0L;
	bool bOverWrite = true;

	QFile tempFile(m_pTempFile);
	if(tempFile.exists())
	{//如果存在零时文件,则说明上一次下载没有下载完成，则从记录到的数据开始下载
		//此处需要注意，如果记录数据上次下载的实体文件不存在，那么就需要从头开始下载 
		if(CMisc::IsFileExist(m_pLocalSaveFile))
		{
			if(tempFile.open(QIODevice::ReadOnly))
			{
				QTextStream in(&tempFile);
				downloadByte = in.readLine().toLong();
				if(downloadByte > 0)
					bOverWrite = false;
				tempFile.close();
			}
		}
	}

	if(NULL == m_pRCFile)
	{
		m_pRCFile = new QFile(this);
	}

	m_pRCFile->setFileName(m_pTempFile);
	if(m_pRCFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		//设置文件为隐藏属性
		m_pRCFile->setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
		::SetFileAttributes(m_pTempFile.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
		m_bRCFileOpend = true;
	}

	m_strVersionFile = QString("%1/~%2.vn").arg(desPath).arg(key);
	QFile vnFile(m_strVersionFile);
	if(vnFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
	{
		vnFile.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
		vnFile.write(version.toLocal8Bit().constData());
		vnFile.close();
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("下载软件[%1] url[%2] 本地存储[%3]").arg(key).arg(srcPath).arg(desPath));

	//启动下载线程
	m_pHTTPDownload->downloadFileSync(srcPath, desPath, downloadByte, bOverWrite);
}

//关闭下载
void CDownloadCM::stopDownload()
{
	if(NULL != m_pHTTPDownload)
		m_pHTTPDownload->stopDownload();
}

void CDownloadCM::downloadFinish()
{
	if(NULL != m_pRCFile)
	{
		if(m_bRCFileOpend)
		{
			m_pRCFile->close();
			m_bRCFileOpend = false;
		}

		delete m_pRCFile;
		m_pRCFile = nullptr;
	}
	
	QString newName = m_pLocalSaveFile;
	QString sufix = QFileInfo(m_pLocalSaveFile).suffix();
	QFile::remove(m_pTempFile);
	QFile::rename(m_pLocalSaveFile, newName.remove("."+ sufix));

	emit downloadFinished(m_pSoftKey);
}

void CDownloadCM::downloadQuited()
{
	emit downloadQuit(m_pSoftKey);
}

void CDownloadCM::downloadProcess(long lDownLoadByte, int totalBytes)
{
	if(!m_bRCFileOpend)
	{
		m_pRCFile->setFileName(m_pTempFile);
		if(m_pRCFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			//设置文件为隐藏属性
			m_pRCFile->setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
			::SetFileAttributes(m_pTempFile.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
			m_bRCFileOpend = true;
		}
	}

	m_pRCFile->resize(0);
	m_pRCFile->write(QString::fromLocal8Bit("%1").arg(lDownLoadByte).toStdString().c_str());
	m_pRCFile->flush();
	qDebug() << "download bytes: " << lDownLoadByte;
	emit downloadPercent(m_pSoftKey, lDownLoadByte, totalBytes);
}

void CDownloadCM::downloadErrorOccur(const QString& msg)
{
	emit downloadErrorOccured(m_pSoftKey, msg);
}
