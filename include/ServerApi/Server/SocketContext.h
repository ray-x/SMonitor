#pragma once
#include "../Misc/FixedMemoryPool.h"

//套接字上下文结构
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
		//释放SOCKET
		if(INVALID_SOCKET != m_ClientSocket)
		{
			CancelIo((HANDLE)m_ClientSocket); 
			shutdown(m_ClientSocket, SD_BOTH); 
			closesocket(m_ClientSocket);
			m_ClientSocket = INVALID_SOCKET;
		}
	}

	SOCKET	    m_ClientSocket;   //客户端Socket
	SOCKADDR_IN m_ClientAddr;     //客户端的地址
	bool        m_bIsOnline;      //是否在线
	void        *m_pUserParam;    //用户自定义数据
};

