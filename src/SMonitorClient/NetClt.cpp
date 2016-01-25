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

long           g_dwTlsIndex;                //tls公用槽
long           g_dwThreadTlsId;             //tls锁
unsigned short g_iWorkerThreadNumbers;      //工作线程数量

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

// 开始测试
bool CNetClt::Start()
{
	if(m_bIsOnLine)
		return true;

	Stop();

	//初始化线程互斥量
	InitializeCriticalSection(&m_csRecvData);
	InitializeCriticalSection(&m_csSocket);
	InitializeCriticalSection(&m_csSendData);
	InitializeCriticalSection(&m_csMemoryData);
	InitializeCriticalSection(&m_csSend);
	InitializeCriticalSection(&m_csConnect);

	// 建立系统退出的事件通知
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(m_hCompletionPort == NULL)
		throw CSrvException(" CNetClt::Start 完成端口对象创建失败,程序无法启动.", GetLastError());

	//建立IO
// 	if(!InitTcpClient())
// 	{
// 		QString strError = QString::fromLocal8Bit("无法启动服务！");
// 		QMessageBox::warning(NULL, "error", strError);
// 		return false;
// 	}

	// 为工作线程初始化句柄
	m_phWorkerThreads = new HANDLE[g_iWorkerThreadNumbers];
	if(NULL == m_phWorkerThreads)
	{
		QString strError = QString::fromLocal8Bit("无法创建TCPIO工作线程句柄，请确认内存是否已满！");
		QMessageBox::warning(NULL, "error", strError);
		return false;
	}

	// 建立通信线程
	DWORD dwThreadID = 0;
	for(int i = 0; i < g_iWorkerThreadNumbers; i++)
	{
		THREADPARAMS_WORKER* pThreadParam = new THREADPARAMS_WORKER;
		if(NULL == pThreadParam)
		{
			QString strError = QString::fromLocal8Bit("无法创建TCPIO工作线程参数，请确认内存是否已满！");
			QMessageBox::warning(NULL, "error", strError);
			return false;
		}

		pThreadParam->pClient = this;
		pThreadParam->nThreadNo = (DWORD)i;
		m_phWorkerThreads[i] = ::CreateThread(NULL, 0, CommProc, pThreadParam, 0, &dwThreadID);
		if(!m_phWorkerThreads[i])
		{	
			SafeDelete(pThreadParam);
			QString strError = QString::fromLocal8Bit("无法创建TCPIO工作线程[%1]，请确认内存是否已满！").arg(i);
			QMessageBox::warning(NULL, "error", strError);
			return false;
		}
	}	

	//启动任务处理线程
	DWORD nThreadID = 0;
	m_hWorkerThread = ::CreateThread(0, 0, WorkProc, this, 0, &nThreadID);
	if(NULL == m_hWorkerThread)
	{
		QString strError = QString::fromLocal8Bit("任务处理线程创建失败 错误码[%1]").arg(GetLastError());
		ClientLogger->AddLog(strError);
		QMessageBox::warning(NULL, "error", strError);
		return false;
	}

	//启动异步数据发送线程
	m_hSendThread = ::CreateThread(0, 0, SendProc, this, 0, &nThreadID);
	if(NULL == m_hSendThread)
	{
		QString strError = QString::fromLocal8Bit("异步数据发送线程创建失败 错误码[%1]").arg(GetLastError());
		ClientLogger->AddLog(strError);
		QMessageBox::warning(NULL, "error", strError);
		return false;
	}

	//启动连接服务器线程
	m_hConnectThread = ::CreateThread(0, 0, ConnectProc, this, 0, &nThreadID);
	if(NULL == m_hSendThread)
	{
		QString strError = QString::fromLocal8Bit("连接服务器线程创建失败 错误码[%1]").arg(GetLastError());
		ClientLogger->AddLog(strError);
		QMessageBox::warning(NULL, "error", strError);
		return false;
	}

	m_bNeedConnect = true;
	return true;
}
//	停止测试
void CNetClt::Stop()
{
	if(NULL == m_hShutdownEvent) return ;

	SetEvent(m_hShutdownEvent);

	for (int i = 0; i < g_iWorkerThreadNumbers; i++)
	{
		// 通知所有的完成端口操作退出
		PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, NULL);
		Sleep(100);
	}

	if(NULL != m_hTcpClientSocket && INVALID_SOCKET != m_hTcpClientSocket)
	{
		RELEASE_SOCKET(m_hTcpClientSocket);
	}	

	// 等待所有的客户端资源退出
	if(NULL != m_phWorkerThreads)
		WaitForMultipleObjects(g_iWorkerThreadNumbers, m_phWorkerThreads, TRUE, INFINITE);

	// 等待Work线程退出
	WaitForSingleObject(m_hWorkerThread, 5000);

	// 等待发送线程退出
	WaitForSingleObject(m_hSendThread, 5000);

	//等待连接服务器线程退出
	TerminateThread(m_hConnectThread, 0);
	WaitForSingleObject(m_hConnectThread, 5000);

	// 清空资源
	CleanUp();
}

// 设置连接IP地址
void CNetClt::SetIP( LPCTSTR strIP ) 
{
	m_strServerIP = strIP; 
}
// 设置监听端口
void CNetClt::SetPort( int nPort )   
{
	m_nPort = nPort; 
}

//发送消息
bool CNetClt::SendMsg(MsgHead msgHead, bool bSendAnsyc)
{
	if(!m_bIsOnLine)
	{
		return false;
	}

	return _SendMsg(msgHead, bSendAnsyc);
}

//发送数据
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
		{//发送失败
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

//异步发送数据 发送完毕发送
bool CNetClt::SendToSvrAnsyc(Buffer buffer)
{
	if(buffer.nBufferSize >= MAX_BUFFER_LEN)
		return false;

	EnterCriticalSection(&m_csSendData);
	m_SendDataList.push_back(buffer);
	LeaveCriticalSection(&m_csSendData);
	return true;
}

//发送测试模式数据
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

//发送消息 bSendAnsyc - 异步发送 如果为异步发送，通过sigSendResult返回发送结果
bool CNetClt::_SendMsg(MsgHead msgHead, bool bSendAnsyc)
{
	if(NULL == m_pMessageFactory)
	{
		//emit sigSendResult(msgHead, false, QString::fromLocal8Bit("连接服务器失败，无法发送消息到服务器"));
		return false;
	}

	return m_pMessageFactory->sendMessage(msgHead, bSendAnsyc);
}

//初始化为tcp客户端
bool CNetClt::InitTcpClient()
{
	bool rc = false;
	try
	{
		m_hTcpClientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if(m_hTcpClientSocket == INVALID_SOCKET)
		{
			throw CSrvException("CNetClt::InitTcpClient 初始化tcp套接字失败.",WSAGetLastError());
		}

		rc = this->AssociateAsTcpClient(m_hTcpClientSocket);
	}
	catch(CSrvException& e)
	{
		ClientLogger->AddLog(e.GetExpDescription());
	}
	catch(...)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("InitTcpClient 发生未知异常."));
	}

	return rc;
}

//将SOCKET作为客户端SOCKET, 并连接服务器
bool CNetClt::AssociateAsTcpClient(SOCKET& socket)
{
	bool rc = false;
	try
	{
		if(socket == NULL || socket == INVALID_SOCKET)
			throw  CSrvException("CNetClt::AssociateAsTcpClient socket == NULL || socket == INVALID_SOCKET", -1);

		//设置为socket重用,防止服务器崩溃后端口能够尽快再次使用或共其他的进程使用
		int nOpt = 1;
		int errorCode = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&nOpt, sizeof(nOpt));
		if(SOCKET_ERROR == errorCode)
		{
			throw CSrvException("CNetClt::AssociateAsTcpClient Set client socket to SO_REUSEADDR mode error.", WSAGetLastError());
		}

		//设置系统的接收与发送缓冲区
		int nBufferSize = 0;
		setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&nBufferSize, sizeof(int));

		nBufferSize = 4096;
		setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&nBufferSize, sizeof(int)); 

		//把socket和完成端口邦定
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
		ClientLogger->AddLog(QString::fromLocal8Bit("AssociateAsTcpClient 发生未知异常."));
	}

	return rc;
}

//连接服务器
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

		//邦定套接字和服务器地址
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
			throw CSrvException("AssociateAsClinet投递有错误.", -1);

		return true;
	}
	catch(CSrvException& e)
	{
		ClientLogger->AddLog(e.GetExpDescription());
	}
	catch(...)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("AssociateAsTcpClient 发生未知异常."));
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
				throw CSrvException(QString::fromLocal8Bit("CNetClt::IssueTcpReceiveOp this->IssueTcpReceiveOp 投递receive 操作失败."), -1);

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
		ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::IssueTcpReceiveOp 发生未知异常."));
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
		ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::IssueTcpReceiveOp 发生未知异常."));
	}
	return FALSE;
}

//	清空资源
void CNetClt::CleanUp()
{
	if(m_hShutdownEvent == NULL) return;

	RELEASE_HANDLE(m_hShutdownEvent);

	// 释放工作线程句柄指针
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

//通信线程
DWORD WINAPI CNetClt::CommProc(LPVOID lpParma)
{
	try
	{
		if(NULL == lpParma)
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::CommProc 参数为null, 线程无法启动，请确认"));
			return 0;
		}

		THREADPARAMS_WORKER* pParam = (THREADPARAMS_WORKER*)lpParma;
		CNetClt* pClient = (CNetClt*)pParam->pClient;

		long threadId = InterlockedIncrement(&g_dwThreadTlsId);
		TlsSetValue(g_dwTlsIndex, (void*)threadId);

		OVERLAPPED	*pOverlapped = NULL;
		SOCKET socket = 0;
		DWORD  dwBytesTransfered = 0;

		// 循环处理请求，直到接收到Shutdown信息为止
		while (WAIT_OBJECT_0 != WaitForSingleObject(pClient->m_hShutdownEvent, 0))
		{
			BOOL bReturn = GetQueuedCompletionStatus(
				pClient->m_hCompletionPort,
				&dwBytesTransfered,
				(PULONG_PTR)&socket,
				&pOverlapped,
				INFINITE);

			// 如果收到的是退出标志，则直接退出
			if(EXIT_CODE == (DWORD)socket) break;

			if(NULL == pOverlapped)
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::CommProc 发现 pOverlapped 为null"));

				if(WAIT_TIMEOUT == GetLastError() && WAIT_OBJECT_0 == WaitForSingleObject(pClient->m_hShutdownEvent, 0))
				{
					ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::CommProc WAIT_TIMEOUT == GetLastError() && WAIT_OBJECT_0 == WaitForSingleObject(pIOCPNet->m_hShutdownEvent, 0)"));
					break;
				}

				continue;
			}

			// 读取传入的参数
			PER_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_Overlapped);  

			// 判断是否出现了错误
			if(!bReturn)  
			{  
				// 处理错误
				if(!pClient->HandleError(socket, pIoContext))
				{
					break;
				}

				continue;  
			}  
			else  
			{
				// 判断是否有客户端断开了
				//if((0 == dwBytesTransfered) && ( NET_RECV == pIoContext->m_OpType || NET_SEND == pIoContext->m_OpType)) 
				if((0 == dwBytesTransfered) && ( NET_RECV == pIoContext->m_OpType)) 
				{  
					// 释放掉对应的资源
					pClient->FreeIOContext(pIoContext);
					RELEASE_SOCKET(socket);
					//重连 yhb
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
							//接收数据
							if(false == pClient->DoRecv(&socket, pIoContext, (unsigned long)dwBytesTransfered))
							{
								if(!pClient->IsSocketAlive(socket))
								{
									pClient->FreeIOContext(pIoContext);
									RELEASE_SOCKET(socket);

									//重连 yhb
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

										//重连 yhb
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
							//发送数据
							pClient->DoSendComplete(&socket, pIoContext, (unsigned long)dwBytesTransfered); //是否需要判断返回值 yhb
						}
						break;
					default:
						break;
					}//switch(pIoContext->m_OpType)  
				}//if((0 == dwBytesTransfered) && ( NET_RECV == pIoContext->m_OpType || NET_SEND == pIoContext->m_OpType))  
			}//if(!bReturn)  

		}//while(WAIT_OBJECT_0 != WaitForSingleObject(pIOCPNet->m_hShutdownEvent, 0))

		// 释放线程参数
		if(NULL != pParam)
		{
			delete pParam; 
			pParam = NULL;
		}
	}
	catch(...)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("CNetClt::CommProc 线程意外退出 错误码[%1]").arg(GetLastError()));
	}

	return 0;
}

//工作处理线程
DWORD WINAPI CNetClt::WorkProc(LPVOID pParam)
{
	CNetClt *pThis = (CNetClt *)pParam;
	while(WAIT_OBJECT_0 != WaitForSingleObject(pThis->m_hShutdownEvent, 0))
	{
		if(pThis->m_RecvDataList.size() > 0)
		{
			//不一次性全部处理，隔开一段时间再处理
			//处理数据
			EnterCriticalSection(&pThis->m_csRecvData);
			Buffer buffer = pThis->m_RecvDataList[0];
			LeaveCriticalSection(&pThis->m_csRecvData);

			if(pThis->TreatRecvData(buffer))
			{//处理完毕，删掉信息
				EnterCriticalSection(&pThis->m_csRecvData);
				pThis->m_RecvDataList.removeAt(0);
				LeaveCriticalSection(&pThis->m_csRecvData);
			}
		}

		Sleep(100);
	}

	return 1;
}

//发送线程
DWORD WINAPI CNetClt::SendProc(LPVOID pParam)
{
	CNetClt *pThis = (CNetClt *)pParam;
	while(WAIT_OBJECT_0 != WaitForSingleObject(pThis->m_hShutdownEvent, 0))
	{
		if(pThis->m_SendDataList.size() > 0)
		{
			//不一次性全部处理，隔开一段时间再处理
			EnterCriticalSection(&pThis->m_csSendData);
			Buffer &data = pThis->m_SendDataList[0];
			LeaveCriticalSection(&pThis->m_csSendData);

			//处理数据
			if(pThis->SendToSvr(data.szBuf, data.nBufferSize))
			{//处理完毕，删掉信息
				emit pThis->sigSendResult(data.msgHead, true, "");
				EnterCriticalSection(&pThis->m_csSendData);
				pThis->m_SendDataList.removeAt(0);
				LeaveCriticalSection(&pThis->m_csSendData);
			}
			else
			{
				QString strErrorLog = QString::fromLocal8Bit("发送失败， 错误码[%1]").arg(WSAGetLastError());
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

//连接服务器线程
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
		{//测试模式，长连服务器

			MessageDataMediator->m_bIsCanConnectedTime = MessageDataMediator->m_bTextMode;
			if(pThis->m_bNeedConnect)
			{
				if(!pThis->IsSocketAlive(pThis->m_hTcpClientSocket))
				{
					if(!pThis->ConnectToServer())
						return 1;
				}

				//客户端连上服务器后，立即发送一个消息
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

				ClientLogger->AddLog(QString::fromLocal8Bit("达到连接时间[%1]，连接服务器").arg(nextConnectTime.toString("yyyy-MM-dd HH:mm:ss")));
			}
			else if(MessageDataMediator->NeedUploadCrossLinkRunState())
			{//软件安装了，需要立即发送
				MessageDataMediator->m_bIsCanConnectedTime = true;
				MessageDataMediator->clearConnectMsgMap();
				MessageDataMediator->addConnectMsgProperty(EMSG_REQUESTXML, true);
				MessageDataMediator->addConnectMsgProperty(EMSG_REQUESTMSG, true);
				MessageDataMediator->addConnectMsgProperty(EMSG_SOFTUPLOAD, false);
				nWaitConnectMsgTime = 0;
				pThis->m_bConnectMsgSended = false;

				ClientLogger->AddLog(QString::fromLocal8Bit("检测到CrossLinkRun安装成功，还未上传，连接服务器"));
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

				//客户端连上服务器后，立即发送一个消息
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
				{//获取所有消息完毕
					
					ClientLogger->AddLog(QString::fromLocal8Bit("获取所有消息完毕，断开服务器"));
					//生成下一次连接时间
					pThis->GetNextConnectTimeQuartFive(nextConnectTime);
					pThis->WriteNextConnectTime(nextConnectTime);
					ClientLogger->AddLog(QString::fromLocal8Bit("生成下一次连接时间[%1]").arg(nextConnectTime.toString("yyyy-MM-dd HH:mm:ss")));
					MessageDataMediator->m_bIsCanConnectedTime = false;
					nWaitConnectMsgTime = 0;
					pThis->m_bConnectMsgSended = false;

					//断开自己
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

// 处理完成端口上的错误
bool CNetClt::HandleError(SOCKET socket, PER_IO_CONTEXT* pIoContext)
{
	bool bRet = true;
	bool bDeleteClient = true;
	
	DWORD errorCode = GetLastError();
	if(WAIT_TIMEOUT == errorCode)  
	{//超时 

		// 确认客户端是否还连接着...
		if(IsSocketAlive(socket))
		{
			//网络操作超时！重试
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
		//p->throw string("完成端口被关闭,退出\n",errorCode);
		bRet = false;
	}   
	else if(ERROR_NETNAME_DELETED == errorCode || ERROR_OPERATION_ABORTED == errorCode)   
	{   
		//p->throw string("socket被关闭 或者 操作被取消\n",errorCode);   
	}   
	else if(ERROR_MORE_DATA == errorCode)//此错误发生的概率较高，但视网络MTU值，太大的UDP包会在网络上被丢弃
	{
		//if(pOverlapped->operate_type == IO_RUDP_RECEIVE)
		//{
		//p->IssueRudpReceiveOp((RUDP_CONTEXT*)pOverlapped);
		//}
		//p->throw string("Udp 数据包被截断,请减小数据包的长度.");
	}
	else if(ERROR_PORT_UNREACHABLE == errorCode)//远程系统异常终止
	{
		//p->throw string("客户端终止数据发送,错误代码:%d.\n",errorCode);
	}
	else
	{
		//p->throw string("未定义错误:%d.\n",errorCode);
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

// 断客户端Socket是否保持连接
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

// 在有接收的数据到达的时候，进行处理
bool CNetClt::DoRecv(SOCKET* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesRecved)
{
	if(dwBytesRecved <= 0)
		return false;

	//收取数据
	EnterCriticalSection(&m_csRecvData);
	//ClientLogger->AddLog(QString::fromLocal8Bit("收到数据 [%1] 长度[%2]").arg(pIoContext->m_wsaBuf.buf).arg(dwBytesRecved));
	ClientLogger->AddLog(QString::fromLocal8Bit("收到数据 长度[%1]").arg(dwBytesRecved));
	Buffer recvBuffer;
	recvBuffer.nBufferSize = dwBytesRecved;
	memcpy(recvBuffer.szBuf, pIoContext->m_wsaBuf.buf, dwBytesRecved);
	m_RecvDataList.push_back(recvBuffer);
	LeaveCriticalSection(&m_csRecvData);

	//然后开始投递下一个WSARecv请求
	return IssueTcpReceiveOp(pIoContext);
}

// 在有数据发送完成的时候，进行处理
bool CNetClt::DoSendComplete(SOCKET* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesSend)
{
	if(NULL == pIoContext)
		return false;

	//发送数据完成

	//释放发送完毕的IO
	FreeIOContext(pIoContext);
	return true;
}

//解析服务器发过来的内容
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
		ClientLogger->AddLog(QString::fromLocal8Bit("收到数据无法解析 [%1] 长度[%2]").arg(recvBuffer.szBuf).arg(recvBuffer.nBufferSize));
		return true; //返回true 让外面将其删掉
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

//发送连接消息
bool CNetClt::SendConnectMsg()
{
	if(!MessageDataMediator->m_bClientStartOver) //客户端未启动完毕，先不发消息
		return false;

	//获取消息状态
	bool bRecvedXML = false;
	if(!MessageDataMediator->getConnectMsg(EMSG_REQUESTXML, bRecvedXML))
	{
		MessageDataMediator->addConnectMsgProperty(EMSG_REQUESTXML, false);
	}

	//软件信息上报
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

	//请求通知信息
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

//从注册表读取下一次连接服务器时间
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

//生成下一次连接服务器时间(4分 0 - 24)
void CNetClt::GetNextConnectTimeQuartFour(QDateTime& connectTime)
{
	connectTime = QDateTime::currentDateTime();
	int nHour = connectTime.time().hour();

	if(nHour >= 18)
	{//超过18点，则下一次连接服务器时间为明天
		connectTime = connectTime.addDays(1);
	}

	//得到连接时间的小时范围
	int nMinNextHour = nHour >= 18 ? 0 : ((nHour / 6 + 1) * 6);
	int nMaxNextHour = nMinNextHour + 5;

	//得到下一次随机的一个小时数
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	nHour = nMinNextHour + qrand() % (nMaxNextHour - nMinNextHour);

	//得到下一次随机的一个分钟数
	int nMinute = qrand() % 59;

	//得到下一次随机的一个秒数
	int nSecond = qrand() % 59;

	QTime time(nHour, nMinute, nSecond);
	connectTime.setTime(time);
}

//生成下一次连接服务器时间(5分 7 - 22)
void CNetClt::GetNextConnectTimeQuartFive(QDateTime& connectTime)
{
	connectTime = QDateTime::currentDateTime();
	int nHour = connectTime.time().hour();

	if(nHour >= 19)
	{//超过19点，则下一次连接服务器时间为明天
		connectTime = connectTime.addDays(1);
	}

	//得到连接时间的小时范围
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

	//得到下一次随机的一个小时数
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	nHour = nMinNextHour + qrand() % (nMaxNextHour - nMinNextHour);

	//得到下一次随机的一个分钟数
	int nMinute = qrand() % 59;

	//得到下一次随机的一个秒数
	int nSecond = qrand() % 59;

	QTime time(nHour, nMinute, nSecond);
	connectTime.setTime(time);
}

//将连接时间写入注册表
void CNetClt::WriteNextConnectTime(QDateTime& connectTime)
{
	if(!connectTime.isValid())
		return;

	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	settings.setValue("Connect/NextTime", connectTime.toString("yyyy-MM-dd HH:mm:ss"));
}