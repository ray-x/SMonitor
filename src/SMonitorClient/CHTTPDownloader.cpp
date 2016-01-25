#include "CHTTPDownloader.h"
#include "HTTPComm.h"
#include <Windows.h>
#include "CClientLog.h"
DWORD WINAPI WorkerFunction(IN LPVOID pThreadParm)
{
	if(NULL == pThreadParm)
		return 1;

	try
	{
		CHTTPDownloader* pHttpDownload = (CHTTPDownloader*)pThreadParm;
		if(NULL != pHttpDownload)
		{
			if(!pHttpDownload->m_bStop)
			{
				pHttpDownload->OpenConnection();
			}
		}
	}
	catch (...)
	{//这里的出错无法通过sig_ErrorOccur返回，需要记录到总的错误日至里 yhb
		 
	}
	
	return 0;
}

CHTTPDownloader::CHTTPDownloader(QObject *parent)
	: QThread(parent)
{
	m_lDownLoadStartBytes = 0;
	m_bStop = false;
	m_bOverWrite = false;
	m_pHttpbase = new CHTTPComm();
	m_pHttpbase->SetRelativeDownloader(this);
	connect(this, &CHTTPDownloader::sig_ErrorOccur, this, &CHTTPDownloader::errorOccur);
}

CHTTPDownloader::~CHTTPDownloader()
{
	m_bStop = true;
	if(NULL != m_pHttpbase)
	{
		m_pHttpbase->StopDownload();
		m_pHttpbase->CloseConnection(true);
	}
}

//下载文件
//strFileUrl - 文件URL strLocalSavePath - 文件本地存储位置 lDownloadStartBytes - 文件下载开始位置 dwTransferBlockSize - 每次传输的期望速度 bOverWrite - 该次写入是否擦除文件
void CHTTPDownloader::downloadFileSync(const QString strFileUrl, const QString strLocalSavePath, long lDownloadStartBytes, bool bOverWrite, long dwTransferBlockSize/* = 51200*/)
{
	m_bOccurError = false;
	if(NULL == m_pHttpbase)
	{
		emit sig_ErrorOccur(QString::fromLocal8Bit("内存不足"));
		return;
	}

	m_strFileUrl = strFileUrl;
	m_strLocalSacePath = strLocalSavePath;
	m_lTransferBlockSize = dwTransferBlockSize;
	m_lDownLoadStartBytes = lDownloadStartBytes;
	m_bOverWrite = bOverWrite;

	start();
}

//停止下载
void CHTTPDownloader::stopDownload()
{
	m_bStop = true;
	int nTotalWait = 1000;
	while(isRunning() && nTotalWait > 0)
	{
		m_pHttpbase->StopDownload();
		msleep(100); //yhb 这里休眠的是哪个线程？
		nTotalWait -= 100;
	}

	if(isRunning())
	{
		terminate();
	}
}

void CHTTPDownloader::OpenConnection()
{
	if(NULL == m_pHttpbase)
	{
		emit sig_ErrorOccur(QString::fromLocal8Bit("内存不足"));
		return;
	}

	m_pHttpbase->OpenConnection(m_strFileUrl, m_strErrMsg, false);
}

void CHTTPDownloader::run()
{
	if(NULL == m_pHttpbase)
	{
		emit sig_ErrorOccur(QString::fromLocal8Bit("内存不足"));
		return;
	}

	//先关闭下载
	m_pHttpbase->StopDownload();

	//设置下载参数
	m_pHttpbase->SetDownloadPara(m_lTransferBlockSize, 0, 1000, m_lDownLoadStartBytes, m_bOverWrite);

	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, WorkerFunction, this, 0, &dwThreadID);    

	//超时连接5秒
	DWORD dwTimeout = 50000; // in milliseconds
	if(WaitForSingleObject(hThread, dwTimeout) == WAIT_TIMEOUT)
	{
		m_pHttpbase->CloseConnection(true);
		WaitForSingleObject(hThread, INFINITE);
		emit sig_ErrorOccur(QString::fromLocal8Bit("连接超时"));
		return ;
	}

	if(m_bOccurError)
		return;

	DWORD dwExitCode = 0;
	if (!GetExitCodeThread(hThread, &dwExitCode))
	{
		emit sig_ErrorOccur(QString::fromLocal8Bit("下载异常"));
		return ;
	}

	CloseHandle(hThread);
	if (dwExitCode)
	{//前面的连接线程并未退出，下面的下载必有问题
		emit sig_ErrorOccur(QString::fromLocal8Bit("下载异常"));
		return;
	}

	if(m_bOccurError)
		return;

	//下载
	m_pHttpbase->DownloadFile(m_strLocalSacePath, m_strErrMsg);
}


void CHTTPDownloader::errorOccur(const QString& errMsg)
{
	//ClientLogger->AddLog(errMsg);
	m_bOccurError = true;
}