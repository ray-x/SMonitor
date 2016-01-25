#include "stdafx.h"
#include "IOCPNet.h"
#include <QMessageBox>
#include "CMessageEventMediator.h"
#include "CMessageFactory.h"
#include <QDebug>
CIOCPNet::CIOCPNet(CMessageEventMediator* pMessageEventMediator)
{
	m_pMessageEventMediator = pMessageEventMediator;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_nThreadNum = 2 * si.dwNumberOfProcessors + 2;

	for (int i = 0; i < m_nThreadNum + 1; i++)
	{
		m_vecMessageFactory.append(new CMessageFactory(pMessageEventMediator, this));
	}
}


CIOCPNet::~CIOCPNet(void)
{
	for (int i = 0; i < m_vecMessageFactory.count(); i++)
	{
		RELEASE(m_vecMessageFactory[i]);
	}

    //RELEASE(m_pMessageFactory);
}

//释放掉所有资源
void CIOCPNet::DeInitialize()
{
	__super::DeInitialize();
}

//清理客户端SocketMap
void CIOCPNet::ClearClientSocketMap()
{
	QMutexLocker locker(&m_SocketMutex);
	m_SocketMap.clear();
}

//连接建立完成
void CIOCPNet::AcceptComplete(SocketContext* clientSocket, char* ip)
{
	QMutexLocker locker(&m_SocketMutex);
	QString strKey = inet_ntoa(clientSocket->m_ClientAddr.sin_addr);
	m_SocketMap.remove(strKey);
	m_SocketMap.insert(strKey, clientSocket);
}

//数据发送完成
void CIOCPNet::SendComplete(SocketContext* clientSocket, char* buf, unsigned long data_len)
{
}

//数据接收完成
void CIOCPNet::ReceiveComplete(SocketContext* clientSocket, char* buf, unsigned long data_len)
{
	if(NULL != buf && 0 == strcmp(buf, "#sockIsAlive#"))
	{
		return;
	}

	NetMessage netMessage;
	if(!CNetMessageWrapper::GetNetMessage(buf, data_len, netMessage))
	{
		//ServerLogger->AddLog("收到的数据无法解析");
		//ServerLogger->AddLog("数据[%s]", buf);
		//SendData(clientSocket, buf, data_len);
		return;
	}

	//因为是10几个线程同时访问消息工程，因此要注意变量的互斥性
	int nThreadNum = GetThreadTlsId();
	if(nThreadNum <0 || nThreadNum > m_nThreadNum)
		nThreadNum = 0;

	CMessageFactory* pMessageFactory = m_vecMessageFactory[nThreadNum];

	if(NULL != pMessageFactory)
	{
		pMessageFactory->treatMessage(clientSocket, netMessage);
	}

	//测试，将收到的数据发回去
	//SendData(clientSocket, buf, data_len);
}

//客户端发生断开时，回调事件(自动)，通知用户进行清理操作（用户必须在最后手动调用RemoveSocket）
void CIOCPNet::ReleaseClient(SocketContext* clientSocket)
{
	QMutexLocker locker(&m_SocketMutex);
	QString strClientIP = inet_ntoa(clientSocket->m_ClientAddr.sin_addr);
	m_SocketMap.remove(strClientIP);
	
	RemoveSocket(clientSocket);
}

//发送消息 bSendAnsyc - 异步发送 如果为异步发送，通过sigSendResult返回发送结果
bool CIOCPNet::SendMsg(MsgHead msgHead, bool bSendAnsyc)
{
	CMessageFactory* pMessageFactory = m_vecMessageFactory[0];

	if(!IsServerStart() || NULL == pMessageFactory)
	{
		return false;
	}

	return pMessageFactory->sendMessageToAll(msgHead, bSendAnsyc);
}

//发送数据
bool CIOCPNet::SendToClients(char* buf, unsigned long len)
{
	QMutexLocker locker(&m_SocketMutex);
	QMapIterator<QString, SocketContext*> iter(m_SocketMap);
	while(iter.hasNext())
	{
		iter.next();
		SendData(iter.value(), buf, len);
	}

	return true;
}