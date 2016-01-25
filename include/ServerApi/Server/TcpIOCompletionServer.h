#pragma once
#include "IOCompletionServer.h"
#include "IBasicTcpServer.h"
class _SERVERAPI_EXPORT_ TcpIOCompletionServer : public IOCompletionServer, public IBasicTcpServer
{
public:
	TcpIOCompletionServer(void);
	virtual ~TcpIOCompletionServer(void);

	//发送数据
	virtual bool SendData(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//释放掉所有资源
	virtual void DeInitialize();

protected:

	//开启服务器具体实现
	virtual bool _StartServer();

	//关闭服务器具体实现
	virtual bool _StopServer();

	//连接建立完成
	virtual void AcceptComplete(SocketContext* clientSocket, char* ip);

	//数据发送完成
	virtual void SendComplete(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//数据接收完成
	virtual void ReceiveComplete(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//客户端发生断开时，回调事件(自动)，通知用户进行清理操作（用户必须在最后手动调用RemoveSocket）
	virtual void ReleaseClient(SocketContext* clientSocket);

	//检测突然掉线的socket超时时间
	virtual void ConnectTimeOutCheck();

	//初始化Tls
	bool InitializeTls();

	// 初始化Socket
	bool InitializeListenSocket();

	//初始化线程
	bool InitializeThread();

	//获取扩展函数指针
	bool GetExtensionFuncPointers();

	//投递Accept请求
	bool PostAccept(int nAcceptCount);

	//投递Accept请求
	bool PostAccept(PER_IO_CONTEXT* pAcceptIoContext);

	//投递Recv请求
	bool PostRecv(PER_IO_CONTEXT* pIoContext);

	// 处理完成端口上的错误
	bool HandleError(IOSocketContext *pContext, PER_IO_CONTEXT* pIoContext);

	// 在有客户端连入的时候，进行处理
	bool DoAccpet(IOSocketContext* pSocketContext, PER_IO_CONTEXT* pIoContext);

	// 在有接收的数据到达的时候，进行处理
	bool DoRecv(IOSocketContext* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesRecved);

	// 在有数据发送完成的时候，进行处理
	bool DoSendComplete(IOSocketContext* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesSend);

	// 将句柄绑定到完成端口中
	bool AssociateWithIOCP(IOSocketContext *pContext);

	// 并发连接数检测
	bool IsOverMaxConnectNums();

protected:

	//工作线程函数
	static UINT WINAPI WorkThread(LPVOID lpParma);

	//监听线程函数
	static UINT WINAPI ListenThread(LPVOID lpParam); 

public:
	HANDLE                       m_hOSPostSocketAcceptEvent; //当可用AcceptEx调用被耗尽的时候，就会触发FD_ACCEPT，os系统将激发此事件

protected:
	IOSocketContext*             m_pListenContext;           //用于监听的Socket的Context信息
	HANDLE                       m_hListenThread;            //监控线程句柄
	LPFN_ACCEPTEX                m_lpfnAcceptEx;             //AcceptEx函数指针
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs; //GetAcceptExSockAddrs函数指针
	HANDLE                       m_hMaxConnectMutex;         //互斥并发连接数检测锁
};

