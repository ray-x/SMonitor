#pragma once
#include "../Misc/FixedMemoryPool.h"

//�׽��������Ľṹ
class SocketContext : public CMempool_cls
{
public:
	SocketContext::SocketContext(void)
	{
		m_ClientSocket = INVALID_SOCKET;
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr)); 
		m_bIsOnline = FALSE;
		m_pUserParam = NULL;
	}


	SocketContext::~SocketContext(void)
	{
		//�ͷ�SOCKET
		if(INVALID_SOCKET != m_ClientSocket)
		{
			CancelIo((HANDLE)m_ClientSocket); 
			shutdown(m_ClientSocket, SD_BOTH); 
			closesocket(m_ClientSocket);
			m_ClientSocket = INVALID_SOCKET;
		}
	}

	SOCKET	    m_ClientSocket;   //�ͻ���Socket
	SOCKADDR_IN m_ClientAddr;     //�ͻ��˵ĵ�ַ
	bool        m_bIsOnline;      //�Ƿ�����
	void        *m_pUserParam;    //�û��Զ�������
};

