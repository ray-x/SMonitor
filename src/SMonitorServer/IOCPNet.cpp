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

//�ͷŵ�������Դ
void CIOCPNet::DeInitialize()
{
	__super::DeInitialize();
}

//����ͻ���SocketMap
void CIOCPNet::ClearClientSocketMap()
{
	QMutexLocker locker(&m_SocketMutex);
	m_SocketMap.clear();
}

//���ӽ������
void CIOCPNet::AcceptComplete(SocketContext* clientSocket, char* ip)
{
	QMutexLocker locker(&m_SocketMutex);
	QString strKey = inet_ntoa(clientSocket->m_ClientAddr.sin_addr);
	m_SocketMap.remove(strKey);
	m_SocketMap.insert(strKey, clientSocket);
}

//���ݷ������
void CIOCPNet::SendComplete(SocketContext* clientSocket, char* buf, unsigned long data_len)
{
}

//���ݽ������
void CIOCPNet::ReceiveComplete(SocketContext* clientSocket, char* buf, unsigned long data_len)
{
	if(NULL != buf && 0 == strcmp(buf, "#sockIsAlive#"))
	{
		return;
	}

	NetMessage netMessage;
	if(!CNetMessageWrapper::GetNetMessage(buf, data_len, netMessage))
	{
		//ServerLogger->AddLog("�յ��������޷�����");
		//ServerLogger->AddLog("����[%s]", buf);
		//SendData(clientSocket, buf, data_len);
		return;
	}

	//��Ϊ��10�����߳�ͬʱ������Ϣ���̣����Ҫע������Ļ�����
	int nThreadNum = GetThreadTlsId();
	if(nThreadNum <0 || nThreadNum > m_nThreadNum)
		nThreadNum = 0;

	CMessageFactory* pMessageFactory = m_vecMessageFactory[nThreadNum];

	if(NULL != pMessageFactory)
	{
		pMessageFactory->treatMessage(clientSocket, netMessage);
	}

	//���ԣ����յ������ݷ���ȥ
	//SendData(clientSocket, buf, data_len);
}

//�ͻ��˷����Ͽ�ʱ���ص��¼�(�Զ�)��֪ͨ�û���������������û�����������ֶ�����RemoveSocket��
void CIOCPNet::ReleaseClient(SocketContext* clientSocket)
{
	QMutexLocker locker(&m_SocketMutex);
	QString strClientIP = inet_ntoa(clientSocket->m_ClientAddr.sin_addr);
	m_SocketMap.remove(strClientIP);
	
	RemoveSocket(clientSocket);
}

//������Ϣ bSendAnsyc - �첽���� ���Ϊ�첽���ͣ�ͨ��sigSendResult���ط��ͽ��
bool CIOCPNet::SendMsg(MsgHead msgHead, bool bSendAnsyc)
{
	CMessageFactory* pMessageFactory = m_vecMessageFactory[0];

	if(!IsServerStart() || NULL == pMessageFactory)
	{
		return false;
	}

	return pMessageFactory->sendMessageToAll(msgHead, bSendAnsyc);
}

//��������
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