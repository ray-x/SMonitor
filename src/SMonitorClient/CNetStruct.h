#pragma once
#include "stdafx.h"
#include "../SMonitorServer/CNetMessage.h"
#include "FixedMemoryPool.h"
#include "ResMemThreadManager.h"
// ����������(8*1024�ֽ�)
#define MAX_BUFFER_LEN        8196    
#define DEFAULT_PORT          14640                      // Ĭ����ͨ�Ŷ˿�
#define DEFAULT_IP            _T("127.0.0.1")            // Ĭ��IP��ַ
#define DEFAULT_THREADS       100                        // Ĭ�ϲ����߳���
#define EXIT_CODE             NULL

// �ͷ�ָ���
#define RELEASE(x)                      {if(NULL != x ){delete x; x = NULL;}}
// �ͷž����
#define RELEASE_HANDLE(x)               {if(NULL != x && INVALID_HANDLE_VALUE != x){ CloseHandle(x); x = NULL;}}
// �ͷ�Socket��
#define RELEASE_SOCKET(x)               {if(INVALID_SOCKET != x) { closesocket(x); x = INVALID_SOCKET;}}
// �ͷ�ָ���
#define RELEASEONLY(x)                  { if(NULL != (x) ){delete (x);} }

const int      DEFAULT_TCP_RECEIVE_BUFFER_SIZE = 4096;    //TCP���ܻ���
const int      FIX_MEMINITCOUNT = 10;                     //�����ڴ�س�ʼ��С
const int      TIMERURESOLUTION = 1000;                   //��ʱ����ʱ���ȣ���ֵԽС��ʱ���¼��ֱ���Խ��
const int      DEFAULT_TCP_SEND_BUFFER_SIZE = 1024 * 4;   //TCP���͵�������ֽ�����

class CNetClt;
// ���ڷ������ݵ��̲߳���
typedef struct _tagThreadParams_WORKER
{
	CNetClt * pClient;                              // ��ָ�룬���ڵ������еĺ���
	SOCKET   sock;                                  // ÿ���߳�ʹ�õ�Socket
	int      nThreadNo;                             // �̱߳��
	char     szBuffer[MAX_BUFFER_LEN];

}THREADPARAMS_WORKER,*PTHREADPARAM_WORKER;  

// ����Socket���ӵ��߳�
typedef struct _tagThreadParams_CONNECTION
{
	CNetClt * pClient;                               // ��ָ�룬���ڵ������еĺ���
}THREADPARAMS_CONNECTION,*PTHREADPARAM_CONNECTION; 

typedef struct _Buffer
{
	MsgHead msgHead;
	char szBuf[MAX_BUFFER_LEN];
	int  nBufferSize;
	bool bRetry;
	int  nRetryNum;
	int  nHasRetryNum;

	_Buffer()
	{
		memset(szBuf, 0, MAX_BUFFER_LEN);
		nBufferSize = 0;
		bRetry = false;
		nRetryNum = 0;
		nHasRetryNum = 0;
		msgHead = EMSG_UNKNOWN;
	}
}Buffer;

//����ɶ˿���Ͷ�ݵ�I/O����������
typedef enum _NET_TYPE  
{  
	NET_ACCEPT,                      // Accept����
	NET_SEND,                        // ���Ͳ���
	NET_RECV,                        // ���ղ���
	NET_UNKNOWN                      // ��ʼ����δ֪����
}NET_TYPE;

typedef struct _PER_IO_CONTEXT : public MEMPOOL_STU
{
	OVERLAPPED	m_Overlapped;               // �ص��ṹ(���ÿһ��Socket��ÿһ����������Ҫ��һ��)              
	SOCKET      m_sockAccept;               // ������������ʹ�õ�Socket
	WSABUF      m_wsaBuf;                   // WSA���͵Ļ����������ڸ��ص�������������
	char		m_szBuffer[MAX_BUFFER_LEN]; // �����WSABUF�������ַ��Ļ�����
	NET_TYPE	m_OpType;                   // ��ʶ�������������(��Ӧ�����ö��)

	// ��ʼ��
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));  
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_OpType     = NET_UNKNOWN;
	}

	// ����
	~_PER_IO_CONTEXT()
	{
		//�ͷ�SOCKET
		//RELEASE_SOCKET(m_sockAccept);
// 		if(INVALID_SOCKET != m_sockAccept) 
// 		{ 
// 			CancelIo((HANDLE)m_sockAccept); 
// 			shutdown(m_sockAccept, SD_BOTH); 
// 			closesocket(m_sockAccept); 
// 			m_sockAccept = INVALID_SOCKET;
// 		}
	}

	// ���û���������
	void ResetBuffer()
	{
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
	}

}PER_IO_CONTEXT, *PPER_IO_CONTEXT;