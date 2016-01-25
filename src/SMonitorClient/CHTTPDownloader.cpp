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
	{//����ĳ����޷�ͨ��sig_ErrorOccur���أ���Ҫ��¼���ܵĴ��������� yhb
		 
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

//�����ļ�
//strFileUrl - �ļ�URL strLocalSavePath - �ļ����ش洢λ�� lDownloadStartBytes - �ļ����ؿ�ʼλ�� dwTransferBlockSize - ÿ�δ���������ٶ� bOverWrite - �ô�д���Ƿ�����ļ�
void CHTTPDownloader::downloadFileSync(const QString strFileUrl, const QString strLocalSavePath, long lDownloadStartBytes, bool bOverWrite, long dwTransferBlockSize/* = 51200*/)
{
	m_bOccurError = false;
	if(NULL == m_pHttpbase)
	{
		emit sig_ErrorOccur(QString::fromLocal8Bit("�ڴ治��"));
		return;
	}

	m_strFileUrl = strFileUrl;
	m_strLocalSacePath = strLocalSavePath;
	m_lTransferBlockSize = dwTransferBlockSize;
	m_lDownLoadStartBytes = lDownloadStartBytes;
	m_bOverWrite = bOverWrite;

	start();
}

//ֹͣ����
void CHTTPDownloader::stopDownload()
{
	m_bStop = true;
	int nTotalWait = 1000;
	while(isRunning() && nTotalWait > 0)
	{
		m_pHttpbase->StopDownload();
		msleep(100); //yhb �������ߵ����ĸ��̣߳�
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
		emit sig_ErrorOccur(QString::fromLocal8Bit("�ڴ治��"));
		return;
	}

	m_pHttpbase->OpenConnection(m_strFileUrl, m_strErrMsg, false);
}

void CHTTPDownloader::run()
{
	if(NULL == m_pHttpbase)
	{
		emit sig_ErrorOccur(QString::fromLocal8Bit("�ڴ治��"));
		return;
	}

	//�ȹر�����
	m_pHttpbase->StopDownload();

	//�������ز���
	m_pHttpbase->SetDownloadPara(m_lTransferBlockSize, 0, 1000, m_lDownLoadStartBytes, m_bOverWrite);

	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, WorkerFunction, this, 0, &dwThreadID);    

	//��ʱ����5��
	DWORD dwTimeout = 50000; // in milliseconds
	if(WaitForSingleObject(hThread, dwTimeout) == WAIT_TIMEOUT)
	{
		m_pHttpbase->CloseConnection(true);
		WaitForSingleObject(hThread, INFINITE);
		emit sig_ErrorOccur(QString::fromLocal8Bit("���ӳ�ʱ"));
		return ;
	}

	if(m_bOccurError)
		return;

	DWORD dwExitCode = 0;
	if (!GetExitCodeThread(hThread, &dwExitCode))
	{
		emit sig_ErrorOccur(QString::fromLocal8Bit("�����쳣"));
		return ;
	}

	CloseHandle(hThread);
	if (dwExitCode)
	{//ǰ��������̲߳�δ�˳�����������ر�������
		emit sig_ErrorOccur(QString::fromLocal8Bit("�����쳣"));
		return;
	}

	if(m_bOccurError)
		return;

	//����
	m_pHttpbase->DownloadFile(m_strLocalSacePath, m_strErrMsg);
}


void CHTTPDownloader::errorOccur(const QString& errMsg)
{
	//ClientLogger->AddLog(errMsg);
	m_bOccurError = true;
}