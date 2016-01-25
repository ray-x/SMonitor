#pragma once
#include "SocketContext.h"
#include <winsock2.h>
#include <vector>
using namespace std;

//在完成端口上投递的I/O操作的类型
typedef enum _NET_TYPE  
{  
	NET_ACCEPT,                      // Accept操作
	NET_SEND,                        // 发送操作
	NET_RECV,                        // 接收操作
	NET_UNKNOWN                      // 初始化，未知类型
}NET_TYPE;

//工作线程的线程参数
class IOCompletionServer;
typedef struct _tagThreadParams_WORKER : public MEMPOOL_STU
{
	IOCompletionServer*	pIOCPNet;    // 类指针
	DWORD               dwThreadNo;  // 线程编号

} THREADPARAMS_WORKER, *PTHREADPARAM_WORKER; 

//重叠I0网络操作结构
typedef struct _PER_IO_CONTEXT : public MEMPOOL_STU
{
	OVERLAPPED	m_Overlapped;               // 重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	SOCKET      m_sockAccept;               // 这个网络操作所使用的Socket
	WSABUF      m_wsaBuf;                   // WSA类型的缓冲区，用于给重叠操作传参数的
	char		m_szBuffer[MAX_BUFFER_LEN]; // 这个是WSABUF里具体存字符的缓冲区
	NET_TYPE	m_OpType;                   // 标识网络操作的类型(对应上面的枚举)

	// 初始化
	_PER_IO_CONTEXT();

	// 析构
	~_PER_IO_CONTEXT();

	// 重置缓冲区内容
	void ResetBuffer();

} PER_IO_CONTEXT, *PPER_IO_CONTEXT;

typedef class IOSocketContext : public SocketContext
{
public:
	//初始化	
	IOSocketContext();

	//释放资源
	~IOSocketContext();

	//获取一个新的IoContext
	_PER_IO_CONTEXT* GetNewIoContext(NET_TYPE net_Type);

	//从数组中移除一个指定的IoContext
	void RemoveContext(_PER_IO_CONTEXT* pContext, NET_TYPE net_Type);

	//清理缓存
	void ClearContext();

	//判断IO是否都已处理完
	bool IsIOEmpty();
	bool IsIOEmpty(NET_TYPE net_Type);

private:
	vector<_PER_IO_CONTEXT*> m_arrayIoAcceptContext;       // 客户端网络操作的连接上下文数据，可以投递多个IO操作
	vector<_PER_IO_CONTEXT*> m_arrayIoSendContext;         // 客户端网络操作的发送上下文数据，可以投递多个IO操作
	vector<_PER_IO_CONTEXT*> m_arrayIoRecvContext;         // 客户端网络操作的接收上下文数据，可以投递多个IO操作
	HANDLE m_hAcceptMutex, m_hSendMutex, m_hRecvMutex;
	
}*PIOSocketContext;
