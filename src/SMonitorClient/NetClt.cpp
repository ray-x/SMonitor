/*
This file is part of SMonitor.
Copyright  2015~2016  by: rayx  email rayx.cn@gmail.com
SMonitor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SMonitor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDateTime>
#include "NetClt.h"
#include "CClientLog.h"
#include "common.h"
#include "../SMonitorServer/CNetMessage.h"
#include "CMessage.h"
#include "CMessageDataMediator.h"
#include "CMessageFactory.h"
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

long           g_dwTlsIndex;                //tls���ò�
long           g_dwThreadTlsId;             //tls��
unsigned short g_iWorkerThreadNumbers;      //�����߳�����

CNetClt::CNetClt(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: QObject(parent),
	m_strServerIP(DEFAULT_IP),
	m_nPort(DEFAULT_PORT),
	m_hShutdownEvent(NULL),
	m_bIsOnLine(false),
	m_hTcpClientSocket(NULL),
	m_hCompletionPort(NULL),
	m_phWorkerThreads(NULL),
	m_hConnectThread(NULL),
	m_hWorkerThread(NULL),
	m_hSendThread(NULL),
	m_bNeedConnect(false),
	m_bConnectMsgSended(false)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	g_iWorkerThreadNumbers = 2 * si.dwNumberOfProcessors + 2;
	m_pMessageEventMediator = pMessageEventMediator;
	m_pMessageFactory = new CMessageFactory(pMessageEventMediator, this);
}

CNetClt::~CNetClt()
{
	Stop();
	RELEASE(m_pMessageFactory);
}

// ��ʼ����
bool CNetClt::Start()
{
	if(m_bIsOnLine)
		return true;

	Stop();

	//��ʼ���̻߳�����
	InitializeCriticalSection(&m_csRecvData);
	InitializeCriticalSection(&m_csSocket);
	InitializeCriticalSection(&m_csSendData);
	InitializeCriticalSection(&m_csMemoryData);
	InitializeCriticalSection(&m_csSend);
	InitializeCriticalSection(&m_csConnect);

	// ����ϵͳ�˳����¼�֪ͨ
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(m_hCompletionPort == NULL)
		throw CSrvException(" CNetClt::Start ��ɶ˿ڶ��󴴽�ʧ��,�����޷�����.", GetLastError());

	//����IO
// 	if(!InitTcpClient())
// 	{
// 		QString strError = QString::fromLocal8Bit("�޷���������");
// 		QMessageBox::warning(NULL, "error", strError);
// 		return false;
// 	}

	// Ϊ�����̳߳�ʼ�����
	m_phWorkerThreads = new HANDLE[g_iWorkerThreadNumbers];
	if(NULL == m_phWorkerThreads)
	{
		QString strError = QString::fromLocal8Bit("�޷�����TCPIO�����߳̾������ȷ���ڴ��Ƿ�������");
		QMessageBox::warning(NULL, "error", strError);
		return false;
	}

	// ����ͨ���߳�
	DWORD dwThreadID = 0;
	for(int i = 0; i < g_iWorkerThreadNumbers; i++)
	{
		THREADPARAMS_WORKER* pThreadParam = new THREADPARAMS_WORKER;
		if(NULL == pThreadParam)
		{
			QString strError = QString::fromLocal8Bit("�޷�����TCPIO�����̲߳�������ȷ���ڴ��Ƿ�������");
			QMessageBox::warning(NULL, "error", strError);
			return false;
		}

		pThreadParam->pClient = this;
		pThreadParam->nThreadNo = (DWORD)i;
		m_phWorkerThreads[i] = ::CreateThread(NULL, 0, CommProc, pThreadParam, 0, &dwThreadID);
		if(!m_phWorkerThreads[i])
		{	
			SafeDelete(pThreadParam);
			QString strError = QString::fromLocal8Bit("�޷�����TCPIO�����߳�[%1]����ȷ���ڴ��Ƿ�������").arg(i);
			QMessageBox::warning(NULL, "error", strError);
			return false;
		}
	}	

	//�����������߳�
	DWORD nThreadID = 0;
	m_hWorkerThread = ::CreateThread(0, 0, WorkProc, this, 0, &nThreadID);
	if(NULL == m_hWorkerThread)
	{
		QString strError = QString::fromLocal8Bit("�������̴߳���ʧ�� ������[%1]").arg(GetLastError());
		ClientLogger->AddLog(strError);
		QMessageBox::warning(NULL, "error", strError);
		return false;
	}

	//�����첽���ݷ����߳�
	m_hSendThread = ::CreateThread(0, 0, SendProc, this, 0, &nThreadID);
	if(NULL == m_hSendThread)
	{
		QString strError = QString::fromLocal8Bit("�첽���ݷ����̴߳���ʧ�� ������[%1]").arg(GetLastError());
		ClientLogger->AddLog(strError);
		QMessageBox::warning(NULL, "error", strError);
		return false;
	}

	//�������ӷ������߳�
	m_hConnectThread = ::CreateThread(0, 0, ConnectProc, this, 0, &nThreadID);
	if(NULL == m_hSendThread)
	{
		QString strError = QString::fromLocal8Bit("���ӷ������̴߳���ʧ�� ������[%1]").arg(GetLastError());
		ClientLogger->AddLog(strError);
		QMessageBox::warning(NULL, "error", strError);
		return false;
	}

	m_bNeedConnect = true;
	return true;
}
//	ֹͣ����
void CNetClt::Stop()
{
	if(NULL == m_hShutdownEvent) return ;

	SetEvent(m_hShutdownEvent);

	for (int i = 0; i < g_iWorkerThreadNumbers; i++)
	{
		// ֪ͨ���е���ɶ˿ڲ����˳�
		PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, NULL);
		Sleep(100);
	}

	if(NULL != m_hTcpClientSocket && INVALID_SOCKET != m_hTcpClientSocket)
	{
		RELEASE_SOCKET(m_hTcpClientSocket);
	}	

	// �ȴ����еĿͻ�����Դ�˳�
	if(NULL != m_phWorkerThreads)
		WaitForMultipleObjects(g_iWorkerThreadNumbers, m_phWorkerThreads, TRUE, INFINITE);

	// �ȴ�Work�߳��˳�
	WaitForSingleObject(m_hWorkerThread, 5000);

	// �ȴ������߳��˳�
	WaitForSingleObject(m_hSendThread, 5000);

	//�ȴ����ӷ������߳��˳�
	TerminateThread(m_hConnectThread, 0);
	WaitForSingleObject(m_hConnectThread, 5000);

	// �����Դ
	CleanUp();
}

// ��������IP��ַ
void CNetClt::SetIP( LPCTSTR strIP ) 
{
	m_strServerIP = strIP; 
}
// ���ü����˿�
void CNetClt::SetPort( int nPort )   
{
	m_nPort = nPort; 
}

//������Ϣ
bool CNetClt::SendMsg(MsgHead msgHead, bool bSendAnsyc)
{
	if(!m_bIsOnLine)
	{
		return false;
	}

	return _SendMsg(msgHead, bSendAnsyc);
}

//��������
bool CNetClt::SendToSvr(char* buf, unsigned long len)
{
	if(!m_bIsOnLine || NULL == m_hTcpClientSocket || INVALID_SOCKET == m_hTcpClientSocket)
		return false;

	EnterCriticalSection(&m_csSocket);
	int nTotal = len;
	int nHasSend = 0;
	int nLeft = nTotal;

	char *p = buf;

	bool bRet = true;
	while(nHasSend < nTotal)
	{
		int nRes = send(m_hTcpClientSocket, p + nHasSend, nLeft, 0);
		if(SOCKET_ERROR == nRes)   //IOPENDING
		{//����ʧ��
			bRet = false;
			break;;
		}
		else
		{
			nLeft = nLeft - nRes;
			nHasSend += nRes;
		}
	}

	LeaveCriticalSection(&m_csSocket);
	return bRet;
}

//�첽�������� ������Ϸ���
bool CNetClt::SendToSvrAnsyc(Buffer buffer)
{
	if(buffer.nBufferSize >= MAX_BUFFER_LEN)
		return false;

	EnterCriticalSection(&m_csSendData);
	m_SendDataList.push_back(buffer);
	LeaveCriticalSection(&m_csSendData);
	return true;
}

//���Ͳ���ģʽ����
void CNetClt::SendTextMsg()
{
	if(MessageDataMediator->m_bTextMode && !m_bConnectMsgSended)
	{
// 		while(!MessageDataMediator->m_bClientStartOver)
// 		{
// 			Sleep(100);
// 		}

		if(MessageDataMediator->m_bClientStartOver && m_bIsOnLine)
		{
			SendMsg(EMSG_REQUESTXML, true);
			SendMsg(EMSG_SOFTUPLOAD, true);
			SendMsg(EMSG_REQUESTMSG, true);
			m_bConnectMsgSended = true;
		}
	}
}

//������Ϣ bSendAnsyc - �첽���� ���Ϊ�첽���ͣ�ͨ��sigSendResult���ط��ͽ��
bool CNetClt::_SendMsg(MsgHead msgHead, bool bSendAnsyc)
{
	if(NULL == m_pMessageFactory)
	{
		//emit sigSendResult(msgHead, false, QString::fromLocal8Bit("���ӷ�����ʧ�ܣ��޷�������Ϣ��������"));
		return false;
	}

	return m_pMessageFactory->sendMessage(msgHead, bSendAnsyc);
}

//��ʼ��Ϊtcp�ͻ���
bool CNetClt::InitTcpClient()
{
	bool rc = false;
	try
	{
		m_hTcpClientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if(m_hTcpClientSocket == INVALID_SOCKET)
		{
			throw CSrvException("CNetClt::InitTcpClient ��ʼ��tcp�׽���ʧ��.",WSAGetLastError());
		}

		rc = this->AssociateAsTcpClient(m_hTcpClientSocket);
	}
	catch(CSrvException& e)
	{
		ClientLogger->AddLog(e.GetExpDescription());
	}
	catch(...)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("InitTcpClient ����δ֪�쳣."));
	}

	return rc;
}

//��SOCKET��Ϊ�ͻ���SOCKET, �����ӷ�����
bool CNetClt::AssociateAsTcpClient(SOCKET& socket)
{
	bool rc = false;
	try
	{
		if(socket == NULL || socket == INVALID_SOCKET)
			throw  CSrvException("CNetClt::AssociateAsTcpClient socket == NULL || socket == INVALID_SOCKET", -1);

		//����Ϊsocket����,��ֹ������������˿��ܹ������ٴ�ʹ�û������Ľ���ʹ��
		int nOpt = 1;
		int errorCode = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&nOpt, sizeof(nOpt));
		if(SOCKET_ERROR == errorCode)
		{
			throw CSrvException("CNetClt::AssociateAsTcpClient Set client socket to SO_REUSEADDR mode error.", WSAGetLastError());
		}

		//����ϵͳ�Ľ����뷢�ͻ�����
		int nBufferSize = 0;
		setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&nBufferSize, sizeof(int));

		nBufferSize = 4096;
		setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&nBufferSize, sizeof(int)); 

		//��socket����ɶ˿ڰ
		if(NULL == CreateIoCompletionPort((HANDLE)socket, m_hCompletionPort, socket, 0))
		{
			throw CSrvException("Invalid completion port handle.",WSAGetLastError());
		}

		rc = true;
	}
	catch(CSrvException& e)
	{
		ClientLogger->AddLog(e.GetExpDescription());
	}
	catch(...)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("AssociateAsTcpClient ����δ֪�쳣."));
	}

	return rc;
}

//���ӷ�����
bool CNetClt::ConnectToServer()
{
	try
	{
		RELEASE_SOCKET(m_hTcpClientSocket);
		if(!InitTcpClient())
			return false;

		m_bIsOnLine = false;
		struct sockaddr_in serv_addr;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(m_strServerIP.toStdString().c_str());
		serv_addr.sin_port = htons(m_nPort);

		//��׽��ֺͷ�������ַ
		while(SOCKET_ERROR == ::connect(m_hTcpClientSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
		{                                                                                                                      
			if (WAIT_OBJECT_0 == WaitForSingleObject(m_hShutdownEvent, 0))
			{
				RELEASE_SOCKET(m_hTcpClientSocket);
				return false;
			}

			if(10056 == WSAGetLastError())
			{
				if(IsSocketAlive(m_hTcpClientSocket))
					break;
			}
			
			shutdown(m_hTcpClientSocket, SD_BOTH);
			Sleep(1000);
		}

		if(!IssueTcpReceiveOp(m_hTcpClientSocket, 1))
			throw CSrvException("AssociateAsClinetͶ���д���.", -1);

		return true;
	}
	catch(CSrvException& e)
	{
		ClientLogger->AddLog(e.GetExpDescription());
	}
	catch(...)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("AssociateAsTcpClient ����δ֪�쳣."));
	}

	return false;
}

bool CNetClt::IssueTcpReceiveOp(SOCKET socket, unsigned short times/*=1*/)
{  
	PER_IO_CONTEXT* pContext = NULL;
	try
	{
		for(int i = 0; i < times; i++)
		{
			pContext = GetIOContext();
			if(pContext == NULL)
			{
				throw CSrvException("CNetClt::IssueTcpReceiveOp pContext == NULL", -1);
			}

			pContext->m_OpType = NET_RECV;
			pContext->m_sockAccept = socket;

			if(!this->IssueTcpReceiveOp(pContext))
				throw CSrvException(QString::fromLocal8Bit("CNetClt::IssueTcpReceiveOp this->IssueTcpReceiveOp Ͷ��receive ����ʧ��."), -1);

		}
		return true;
	}
	catch(CSrvException& e)
	{
		if(NULL != pContext) FreeIOContext(pContext);
		ClientLogger->AddLog(e.GetExpDescription());
	}
	catch(...)
	{
		if(NULL != pContext) FreeIOContext(pContext);
		ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::IssueTcpReceiveOp ����δ֪�쳣."));
	}
	return false;
}

bool CNetClt::IssueTcpReceiveOp(PER_IO_CONTEXT* pContext)
{
	try
	{
		pContext->ResetBuffer();
		unsigned long bytes = 0;
		unsigned long flag = 0;
		int err = WSARecv(pContext->m_sockAccept, &pContext->m_wsaBuf, 1, &bytes, &flag, &(pContext->m_Overlapped), NULL);
		if (SOCKET_ERROR == err && WSA_IO_PENDING != WSAGetLastError())
		{
			throw CSrvException(QString::fromLocal8Bit("IssueTcpReceiveOp->WSARecv() error."), WSAGetLastError());
		}
		return true;
	}
	catch(CSrvException& e)
	{
		ClientLogger->AddLog(e.GetExpDescription());
	}
	catch(...)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::IssueTcpReceiveOp ����δ֪�쳣."));
	}
	return FALSE;
}

//	�����Դ
void CNetClt::CleanUp()
{
	if(m_hShutdownEvent == NULL) return;

	RELEASE_HANDLE(m_hShutdownEvent);

	// �ͷŹ����߳̾��ָ��
	if(NULL != m_phWorkerThreads)
	{
		for(int i = 0; i < g_iWorkerThreadNumbers; i++)
			RELEASE_HANDLE(m_phWorkerThreads[i]);

		RELEASE(m_phWorkerThreads);
	}


	RELEASE_HANDLE(m_hWorkerThread);

	RELEASE_HANDLE(m_hSendThread);

	RELEASE_HANDLE(m_hConnectThread);

	CloseHandle(m_hCompletionPort);

	m_RecvDataList.clear();
	m_SendDataList.clear();
}

//ͨ���߳�
DWORD WINAPI CNetClt::CommProc(LPVOID lpParma)
{
	try
	{
		if(NULL == lpParma)
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::CommProc ����Ϊnull, �߳��޷���������ȷ��"));
			return 0;
		}

		THREADPARAMS_WORKER* pParam = (THREADPARAMS_WORKER*)lpParma;
		CNetClt* pClient = (CNetClt*)pParam->pClient;

		long threadId = InterlockedIncrement(&g_dwThreadTlsId);
		TlsSetValue(g_dwTlsIndex, (void*)threadId);

		OVERLAPPED	*pOverlapped = NULL;
		SOCKET socket = 0;
		DWORD  dwBytesTransfered = 0;

		// ѭ����������ֱ�����յ�Shutdown��ϢΪֹ
		while (WAIT_OBJECT_0 != WaitForSingleObject(pClient->m_hShutdownEvent, 0))
		{
			BOOL bReturn = GetQueuedCompletionStatus(
				pClient->m_hCompletionPort,
				&dwBytesTransfered,
				(PULONG_PTR)&socket,
				&pOverlapped,
				INFINITE);

			// ����յ������˳���־����ֱ���˳�
			if(EXIT_CODE == (DWORD)socket) break;

			if(NULL == pOverlapped)
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::CommProc ���� pOverlapped Ϊnull"));

				if(WAIT_TIMEOUT == GetLastError() && WAIT_OBJECT_0 == WaitForSingleObject(pClient->m_hShutdownEvent, 0))
				{
					ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::CommProc WAIT_TIMEOUT == GetLastError() && WAIT_OBJECT_0 == WaitForSingleObject(pIOCPNet->m_hShutdownEvent, 0)"));
					break;
				}

				continue;
			}

			// ��ȡ����Ĳ���
			PER_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_Overlapped);  

			// �ж��Ƿ�����˴���
			if(!bReturn)  
			{  
				// �������
				if(!pClient->HandleError(socket, pIoContext))
				{
					break;
				}

				continue;  
			}  
			else  
			{
				// �ж��Ƿ��пͻ��˶Ͽ���
				//if((0 == dwBytesTransfered) && ( NET_RECV == pIoContext->m_OpType || NET_SEND == pIoContext->m_OpType)) 
				if((0 == dwBytesTransfered) && ( NET_RECV == pIoContext->m_OpType)) 
				{  
					// �ͷŵ���Ӧ����Դ
					pClient->FreeIOContext(pIoContext);
					RELEASE_SOCKET(socket);
					//���� yhb
					pClient->m_bIsOnLine = false;
					EnterCriticalSection(&pClient->m_csConnect);
					pClient->m_bNeedConnect = true;
					LeaveCriticalSection(&pClient->m_csConnect);
					continue;  
				}  
				else
				{
					switch(pIoContext->m_OpType)  
					{
					case NET_ACCEPT:
						{ 
							int n = 2;			
						}
						break;
					case NET_RECV:
						{
							//��������
							if(false == pClient->DoRecv(&socket, pIoContext, (unsigned long)dwBytesTransfered))
							{
								if(!pClient->IsSocketAlive(socket))
								{
									pClient->FreeIOContext(pIoContext);
									RELEASE_SOCKET(socket);

									//���� yhb
									EnterCriticalSection(&pClient->m_csConnect);
									pClient->m_bNeedConnect = true;
									LeaveCriticalSection(&pClient->m_csConnect);
								}
								else
								{
									if(false == pClient->IssueTcpReceiveOp(pIoContext))
									{
										pClient->FreeIOContext(pIoContext);
										RELEASE_SOCKET(socket);

										//���� yhb
										EnterCriticalSection(&pClient->m_csConnect);
										pClient->m_bNeedConnect = true;
										LeaveCriticalSection(&pClient->m_csConnect);
									}
								}
							}
						}
						break;
					case NET_SEND:
						{
							//��������
							pClient->DoSendComplete(&socket, pIoContext, (unsigned long)dwBytesTransfered); //�Ƿ���Ҫ�жϷ���ֵ yhb
						}
						break;
					default:
						break;
					}//switch(pIoContext->m_OpType)  
				}//if((0 == dwBytesTransfered) && ( NET_RECV == pIoContext->m_OpType || NET_SEND == pIoContext->m_OpType))  
			}//if(!bReturn)  

		}//while(WAIT_OBJECT_0 != WaitForSingleObject(pIOCPNet->m_hShutdownEvent, 0))

		// �ͷ��̲߳���
		if(NULL != pParam)
		{
			delete pParam; 
			pParam = NULL;
		}
	}
	catch(...)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::CommProc �߳������˳� ������[%1]").arg(GetLastError()));
	}

	return 0;
}

//���������߳�
DWORD WINAPI CNetClt::WorkProc(LPVOID pParam)
{
	CNetClt *pThis = (CNetClt *)pParam;
	while(WAIT_OBJECT_0 != WaitForSingleObject(pThis->m_hShutdownEvent, 0))
	{
		if(pThis->m_RecvDataList.size() > 0)
		{
			//��һ����ȫ����������һ��ʱ���ٴ���
			//��������
			EnterCriticalSection(&pThis->m_csRecvData);
			Buffer buffer = pThis->m_RecvDataList[0];
			LeaveCriticalSection(&pThis->m_csRecvData);

			if(pThis->TreatRecvData(buffer))
			{//������ϣ�ɾ����Ϣ
				EnterCriticalSection(&pThis->m_csRecvData);
				pThis->m_RecvDataList.removeAt(0);
				LeaveCriticalSection(&pThis->m_csRecvData);
			}
		}

		Sleep(100);
	}

	return 1;
}

//�����߳�
DWORD WINAPI CNetClt::SendProc(LPVOID pParam)
{
	CNetClt *pThis = (CNetClt *)pParam;
	while(WAIT_OBJECT_0 != WaitForSingleObject(pThis->m_hShutdownEvent, 0))
	{
		if(pThis->m_SendDataList.size() > 0)
		{
			//��һ����ȫ����������һ��ʱ���ٴ���
			EnterCriticalSection(&pThis->m_csSendData);
			Buffer &data = pThis->m_SendDataList[0];
			LeaveCriticalSection(&pThis->m_csSendData);

			//��������
			if(pThis->SendToSvr(data.szBuf, data.nBufferSize))
			{//������ϣ�ɾ����Ϣ
				emit pThis->sigSendResult(data.msgHead, true, "");
				EnterCriticalSection(&pThis->m_csSendData);
				pThis->m_SendDataList.removeAt(0);
				LeaveCriticalSection(&pThis->m_csSendData);
			}
			else
			{
				QString strErrorLog = QString::fromLocal8Bit("����ʧ�ܣ� ������[%1]").arg(WSAGetLastError());
				ClientLogger->AddLog(strErrorLog);
				emit pThis->sigSendResult(data.msgHead, false, strErrorLog);

				if(!data.bRetry || (data.bRetry && data.nHasRetryNum >= data.nRetryNum))
				{
					EnterCriticalSection(&pThis->m_csSendData);
					pThis->m_SendDataList.removeAt(0);
					LeaveCriticalSection(&pThis->m_csSendData);
				}

				data.nHasRetryNum++;
			}
		}

		Sleep(100);
	}

	return 1;
}

//���ӷ������߳�
DWORD WINAPI CNetClt::ConnectProc(LPVOID pParam)
{
	CNetClt *pThis = (CNetClt *)pParam;
	QDateTime nextConnectTime;
	pThis->ReadNextConnectTime(nextConnectTime);

	int nRetryTimes = 1;
	int nWaitConnectMsgTime = 0;
	while(WAIT_OBJECT_0 != WaitForSingleObject(pThis->m_hShutdownEvent, 0))
	{
		if(MessageDataMediator->m_bTextMode)
		{//����ģʽ������������

			MessageDataMediator->m_bIsCanConnectedTime = MessageDataMediator->m_bTextMode;
			if(pThis->m_bNeedConnect)
			{
				if(!pThis->IsSocketAlive(pThis->m_hTcpClientSocket))
				{
					if(!pThis->ConnectToServer())
						return 1;
				}

				//�ͻ������Ϸ���������������һ����Ϣ
				EnterCriticalSection(&pThis->m_csConnect);
				pThis->m_bNeedConnect = SOCKET_ERROR == send(pThis->m_hTcpClientSocket, "smonitorclient", sizeof("smonitorclient") + 1, 0);
				if(!pThis->m_bNeedConnect)
				{
					Sleep(500);
					pThis->m_bNeedConnect = !pThis->IsSocketAlive(pThis->m_hTcpClientSocket);
				}
				LeaveCriticalSection(&pThis->m_csConnect);
			}

			pThis->SendTextMsg();

			Sleep(1000);
			continue;
		}

		if(!MessageDataMediator->m_bIsCanConnectedTime)
		{
			MessageDataMediator->m_bIsCanConnectedTime = !nextConnectTime.isValid() || QDateTime::currentDateTime() >= nextConnectTime;
			if(MessageDataMediator->m_bIsCanConnectedTime)
			{
				MessageDataMediator->clearConnectMsgMap();
				nWaitConnectMsgTime = 0;
				pThis->m_bConnectMsgSended = false;

				ClientLogger->AddLog(QString::fromLocal8Bit("�ﵽ����ʱ��[%1]�����ӷ�����").arg(nextConnectTime.toString("yyyy-MM-dd HH:mm:ss")));
			}
			else if(MessageDataMediator->NeedUploadCrossLinkRunState())
			{//�����װ�ˣ���Ҫ��������
				MessageDataMediator->m_bIsCanConnectedTime = true;
				MessageDataMediator->clearConnectMsgMap();
				MessageDataMediator->addConnectMsgProperty(EMSG_REQUESTXML, true);
				MessageDataMediator->addConnectMsgProperty(EMSG_REQUESTMSG, true);
				MessageDataMediator->addConnectMsgProperty(EMSG_SOFTUPLOAD, false);
				nWaitConnectMsgTime = 0;
				pThis->m_bConnectMsgSended = false;

				ClientLogger->AddLog(QString::fromLocal8Bit("��⵽CrossLinkRun��װ�ɹ�����δ�ϴ������ӷ�����"));
			}
		}

		if(MessageDataMediator->m_bIsCanConnectedTime)
		{
			if(pThis->m_bNeedConnect)
			{
				if(!pThis->IsSocketAlive(pThis->m_hTcpClientSocket))
				{
					if(!pThis->ConnectToServer())
						return 1;
				}

				//�ͻ������Ϸ���������������һ����Ϣ
				EnterCriticalSection(&pThis->m_csConnect);
				pThis->m_bNeedConnect = SOCKET_ERROR == send(pThis->m_hTcpClientSocket, "smonitorclient", sizeof("smonitorclient") + 1, 0);
				if(!pThis->m_bNeedConnect)
				{
					Sleep(500);
					pThis->m_bNeedConnect = !pThis->IsSocketAlive(pThis->m_hTcpClientSocket);
				}
				LeaveCriticalSection(&pThis->m_csConnect);
			}

			if(pThis->m_bIsOnLine)
			{
				Sleep(1000);
				nWaitConnectMsgTime += 1000;
				nRetryTimes = 1;

				if(MessageDataMediator->isAllMsgRecved(true))
				{//��ȡ������Ϣ���
					
					ClientLogger->AddLog(QString::fromLocal8Bit("��ȡ������Ϣ��ϣ��Ͽ�������"));
					//������һ������ʱ��
					pThis->GetNextConnectTimeQuartFive(nextConnectTime);
					pThis->WriteNextConnectTime(nextConnectTime);
					ClientLogger->AddLog(QString::fromLocal8Bit("������һ������ʱ��[%1]").arg(nextConnectTime.toString("yyyy-MM-dd HH:mm:ss")));
					MessageDataMediator->m_bIsCanConnectedTime = false;
					nWaitConnectMsgTime = 0;
					pThis->m_bConnectMsgSended = false;

					//�Ͽ��Լ�
					RELEASE_SOCKET(pThis->m_hTcpClientSocket);
					pThis->m_bIsOnLine = false;
				}

				if(pThis->m_bConnectMsgSended)
				{
					if(nWaitConnectMsgTime > 60 * 1000)
					{
						pThis->SendConnectMsg();
						nWaitConnectMsgTime = 0;
					}
				}
				else
				{
					pThis->SendConnectMsg();
					nWaitConnectMsgTime = 0;
				}
			}
			else
			{
				Sleep(1000 * nRetryTimes);
				nRetryTimes += 10;
				nRetryTimes = nRetryTimes > 60 ? 60 : nRetryTimes;
				nWaitConnectMsgTime = 0;
			}
		}
		else
		{
			Sleep(10000);
			nWaitConnectMsgTime = 0;
			nRetryTimes = 1;
		}
	}

	return 1;
}

// ������ɶ˿��ϵĴ���
bool CNetClt::HandleError(SOCKET socket, PER_IO_CONTEXT* pIoContext)
{
	bool bRet = true;
	bool bDeleteClient = true;
	
	DWORD errorCode = GetLastError();
	if(WAIT_TIMEOUT == errorCode)  
	{//��ʱ 

		// ȷ�Ͽͻ����Ƿ�������...
		if(IsSocketAlive(socket))
		{
			//���������ʱ������
			IssueTcpReceiveOp(pIoContext);
			return true;
		}
		else
		{
			bDeleteClient = true;
		}
	}  
	else if(ERROR_INVALID_HANDLE == errorCode)   
	{   
		//p->throw string("��ɶ˿ڱ��ر�,�˳�\n",errorCode);
		bRet = false;
	}   
	else if(ERROR_NETNAME_DELETED == errorCode || ERROR_OPERATION_ABORTED == errorCode)   
	{   
		//p->throw string("socket���ر� ���� ������ȡ��\n",errorCode);   
	}   
	else if(ERROR_MORE_DATA == errorCode)//�˴������ĸ��ʽϸߣ���������MTUֵ��̫���UDP�����������ϱ�����
	{
		//if(pOverlapped->operate_type == IO_RUDP_RECEIVE)
		//{
		//p->IssueRudpReceiveOp((RUDP_CONTEXT*)pOverlapped);
		//}
		//p->throw string("Udp ���ݰ����ض�,���С���ݰ��ĳ���.");
	}
	else if(ERROR_PORT_UNREACHABLE == errorCode)//Զ��ϵͳ�쳣��ֹ
	{
		//p->throw string("�ͻ�����ֹ���ݷ���,�������:%d.\n",errorCode);
	}
	else
	{
		//p->throw string("δ�������:%d.\n",errorCode);
	}

	if(NULL != pIoContext)
	{
		FreeIOContext(pIoContext);
	}

	if(bDeleteClient)
	{
		if(SOCKET_ERROR == send(socket, "IsOnline", strlen("IsOnLine"), 0))
		{
			RELEASE_SOCKET(socket);
			EnterCriticalSection(&m_csConnect);
			m_bNeedConnect = true;
			LeaveCriticalSection(&m_csConnect);
		}
	}

	return bRet;
}

// �Ͽͻ���Socket�Ƿ񱣳�����
bool CNetClt::IsSocketAlive(const SOCKET& s, const char* test/* = "#sockIsAlive#"*/)
{
	int nByteSent = send(s, test, strlen(test), 0);
	if(0 == nByteSent)
	{
		int n = WSAGetLastError();
		n = n;
	}

	return -1 != nByteSent;
}

PER_IO_CONTEXT* CNetClt::GetIOContext()
{
	PER_IO_CONTEXT* pIOContext = NULL;
	EnterCriticalSection(&m_csMemoryData);
	pIOContext = m_TcpReceiveContextManager.GetMemory();
	LeaveCriticalSection(&m_csMemoryData);
	return pIOContext;
}

void CNetClt::FreeIOContext(PER_IO_CONTEXT*& pIOContext)
{
	if(NULL == pIOContext)
		return;

	EnterCriticalSection(&m_csMemoryData);
	m_TcpReceiveContextManager.FreeMemory(pIOContext, pIOContext->managerIndex);
	LeaveCriticalSection(&m_csMemoryData);
}

// ���н��յ����ݵ����ʱ�򣬽��д���
bool CNetClt::DoRecv(SOCKET* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesRecved)
{
	if(dwBytesRecved <= 0)
		return false;

	//��ȡ����
	EnterCriticalSection(&m_csRecvData);
	//ClientLogger->AddLog(QString::fromLocal8Bit("�յ����� [%1] ����[%2]").arg(pIoContext->m_wsaBuf.buf).arg(dwBytesRecved));
	ClientLogger->AddLog(QString::fromLocal8Bit("�յ����� ����[%1]").arg(dwBytesRecved));
	Buffer recvBuffer;
	recvBuffer.nBufferSize = dwBytesRecved;
	memcpy(recvBuffer.szBuf, pIoContext->m_wsaBuf.buf, dwBytesRecved);
	m_RecvDataList.push_back(recvBuffer);
	LeaveCriticalSection(&m_csRecvData);

	//Ȼ��ʼͶ����һ��WSARecv����
	return IssueTcpReceiveOp(pIoContext);
}

// �������ݷ�����ɵ�ʱ�򣬽��д���
bool CNetClt::DoSendComplete(SOCKET* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesSend)
{
	if(NULL == pIoContext)
		return false;

	//�����������

	//�ͷŷ�����ϵ�IO
	FreeIOContext(pIoContext);
	return true;
}

//����������������������
bool CNetClt::TreatRecvData(const Buffer& recvBuffer)
{
	if(!m_bIsOnLine)
	{
		m_bIsOnLine = true;
	}

	if(0 == strcmp(recvBuffer.szBuf, "IsOnline"))
		return true;

	NetMessage netMessage;
	if(!CNetMessageWrapper::GetNetMessage(recvBuffer.szBuf, recvBuffer.nBufferSize, netMessage))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("�յ������޷����� [%1] ����[%2]").arg(recvBuffer.szBuf).arg(recvBuffer.nBufferSize));
		return true; //����true �����潫��ɾ��
	}

	if(NULL != m_pMessageFactory)
	{
		return m_pMessageFactory->treatMessage(netMessage);
	}
	else
	{
		RELEASE(netMessage.msgBuffer);
	}

	return true;
}

//����������Ϣ
bool CNetClt::SendConnectMsg()
{
	if(!MessageDataMediator->m_bClientStartOver) //�ͻ���δ������ϣ��Ȳ�����Ϣ
		return false;

	//��ȡ��Ϣ״̬
	bool bRecvedXML = false;
	if(!MessageDataMediator->getConnectMsg(EMSG_REQUESTXML, bRecvedXML))
	{
		MessageDataMediator->addConnectMsgProperty(EMSG_REQUESTXML, false);
	}

	//�����Ϣ�ϱ�
	bool bRecvedUpdateInfo = false;
	if(!MessageDataMediator->getConnectMsg(EMSG_SOFTUPLOAD, bRecvedUpdateInfo))
	{
		MessageDataMediator->addConnectMsgProperty(EMSG_SOFTUPLOAD, false);
	}
	
	if(bRecvedXML)
	{
		if(!bRecvedUpdateInfo)
		{
			if(!SendMsg(EMSG_SOFTUPLOAD, true))
				return false;
		}
	}

	//����֪ͨ��Ϣ
	bool bRecvedReqMsg = false;
	if(!MessageDataMediator->getConnectMsg(EMSG_REQUESTMSG, bRecvedReqMsg))
	{
		MessageDataMediator->addConnectMsgProperty(EMSG_REQUESTMSG, false);
	}

	if(!bRecvedReqMsg)
	{
		if(!SendMsg(EMSG_REQUESTMSG, true))
			return false;
	}

	if(!bRecvedXML)
	{
		if(!SendMsg(EMSG_REQUESTXML, true))
			return false;
	}
	
	m_bConnectMsgSended = true;
	return true;
}

//��ע����ȡ��һ�����ӷ�����ʱ��
bool CNetClt::ReadNextConnectTime(QDateTime& connectTime)
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	QString strConnectTime = settings.value("Connect/NextTime").toString();
	if(!strConnectTime.isEmpty())
	{
		connectTime = QDateTime::fromString(strConnectTime, "yyyy-MM-dd HH:mm:ss");
		return connectTime.isValid();
	}

	return false;
}

//������һ�����ӷ�����ʱ��(4�� 0 - 24)
void CNetClt::GetNextConnectTimeQuartFour(QDateTime& connectTime)
{
	connectTime = QDateTime::currentDateTime();
	int nHour = connectTime.time().hour();

	if(nHour >= 18)
	{//����18�㣬����һ�����ӷ�����ʱ��Ϊ����
		connectTime = connectTime.addDays(1);
	}

	//�õ�����ʱ���Сʱ��Χ
	int nMinNextHour = nHour >= 18 ? 0 : ((nHour / 6 + 1) * 6);
	int nMaxNextHour = nMinNextHour + 5;

	//�õ���һ�������һ��Сʱ��
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	nHour = nMinNextHour + qrand() % (nMaxNextHour - nMinNextHour);

	//�õ���һ�������һ��������
	int nMinute = qrand() % 59;

	//�õ���һ�������һ������
	int nSecond = qrand() % 59;

	QTime time(nHour, nMinute, nSecond);
	connectTime.setTime(time);
}

//������һ�����ӷ�����ʱ��(5�� 7 - 22)
void CNetClt::GetNextConnectTimeQuartFive(QDateTime& connectTime)
{
	connectTime = QDateTime::currentDateTime();
	int nHour = connectTime.time().hour();

	if(nHour >= 19)
	{//����19�㣬����һ�����ӷ�����ʱ��Ϊ����
		connectTime = connectTime.addDays(1);
	}

	//�õ�����ʱ���Сʱ��Χ
	int nMinNextHour = 0;
	if(nHour >= 19)
	{
		nMinNextHour = 7;
	}
	else if(nHour < 7)
	{
		nMinNextHour = 7;
	}
	else
	{
		nMinNextHour = nHour + 1;
		int nMod = nHour % 3;
		if(nMod > 0)
		{
			nMinNextHour += 3 - nMod;
		}
	}

	int nMaxNextHour = nMinNextHour + 2;

	//�õ���һ�������һ��Сʱ��
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	nHour = nMinNextHour + qrand() % (nMaxNextHour - nMinNextHour);

	//�õ���һ�������һ��������
	int nMinute = qrand() % 59;

	//�õ���һ�������һ������
	int nSecond = qrand() % 59;

	QTime time(nHour, nMinute, nSecond);
	connectTime.setTime(time);
}

//������ʱ��д��ע���
void CNetClt::WriteNextConnectTime(QDateTime& connectTime)
{
	if(!connectTime.isValid())
		return;

	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	settings.setValue("Connect/NextTime", connectTime.toString("yyyy-MM-dd HH:mm:ss"));
}