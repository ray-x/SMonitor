#pragma once
#include "stdafx.h"
#include "../SMonitorServer/CNetMessage.h"
#include "FixedMemoryPool.h"
#include "ResMemThreadManager.h"
// 缓冲区长度(8*1024字节)
#define MAX_BUFFER_LEN        8196    
#define DEFAULT_PORT          14640                      // 默认主通信端口
#define DEFAULT_IP            _T("127.0.0.1")            // 默认IP地址
#define DEFAULT_THREADS       100                        // 默认并发线程数
#define EXIT_CODE             NULL

// 释放指针宏
#define RELEASE(x)                      {if(NULL != x ){delete x; x = NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(NULL != x && INVALID_HANDLE_VALUE != x){ CloseHandle(x); x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(INVALID_SOCKET != x) { closesocket(x); x = INVALID_SOCKET;}}
// 释放指针宏
#define RELEASEONLY(x)                  { if(NULL != (x) ){delete (x);} }

const int      DEFAULT_TCP_RECEIVE_BUFFER_SIZE = 4096;    //TCP接受缓存
const int      FIX_MEMINITCOUNT = 10;                     //公共内存池初始大小
const int      TIMERURESOLUTION = 1000;                   //定时器延时精度，数值越小定时器事件分辨率越高
const int      DEFAULT_TCP_SEND_BUFFER_SIZE = 1024 * 4;   //TCP发送单次最大字节限制

class CNetClt;
// 用于发送数据的线程参数
typedef struct _tagThreadParams_WORKER
{
	CNetClt * pClient;                              // 类指针，用于调用类中的函数
	SOCKET   sock;                                  // 每个线程使用的Socket
	int      nThreadNo;                             // 线程编号
	char     szBuffer[MAX_BUFFER_LEN];

}THREADPARAMS_WORKER,*PTHREADPARAM_WORKER;  

// 产生Socket连接的线程
typedef struct _tagThreadParams_CONNECTION
{
	CNetClt * pClient;                               // 类指针，用于调用类中的函数
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

//在完成端口上投递的I/O操作的类型
typedef enum _NET_TYPE  
{  
	NET_ACCEPT,                      // Accept操作
	NET_SEND,                        // 发送操作
	NET_RECV,                        // 接收操作
	NET_UNKNOWN                      // 初始化，未知类型
}NET_TYPE;

typedef struct _PER_IO_CONTEXT : public MEMPOOL_STU
{
	OVERLAPPED	m_Overlapped;               // 重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	SOCKET      m_sockAccept;               // 这个网络操作所使用的Socket
	WSABUF      m_wsaBuf;                   // WSA类型的缓冲区，用于给重叠操作传参数的
	char		m_szBuffer[MAX_BUFFER_LEN]; // 这个是WSABUF里具体存字符的缓冲区
	NET_TYPE	m_OpType;                   // 标识网络操作的类型(对应上面的枚举)

	// 初始化
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));  
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_OpType     = NET_UNKNOWN;
	}

	// 析构
	~_PER_IO_CONTEXT()
	{
		//释放SOCKET
		//RELEASE_SOCKET(m_sockAccept);
// 		if(INVALID_SOCKET != m_sockAccept) 
// 		{ 
// 			CancelIo((HANDLE)m_sockAccept); 
// 			shutdown(m_sockAccept, SD_BOTH); 
// 			closesocket(m_sockAccept); 
// 			m_sockAccept = INVALID_SOCKET;
// 		}
	}

	// 重置缓冲区内容
	void ResetBuffer()
	{
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
	}

}PER_IO_CONTEXT, *PPER_IO_CONTEXT;