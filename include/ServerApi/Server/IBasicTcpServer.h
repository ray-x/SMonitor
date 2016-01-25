#pragma once
#include "IBasicServer.h"
#include "SocketContext.h"

#include <map>
using namespace std;

typedef void tcp_receive_send_callback_fuc(SocketContext* clientSocket, char* buf, unsigned long data_len);
typedef tcp_receive_send_callback_fuc* PTcpSendCallbackFun;
typedef tcp_receive_send_callback_fuc* PTcpReceiveCallbackFun;

typedef void tcp_accept_callback_fuc(SocketContext* clientSocket, char* ip);
typedef tcp_accept_callback_fuc* PTcpAcceptCallbackFun;

typedef void tcp_release_callback_fuc(SocketContext* clientSocket);
typedef tcp_release_callback_fuc* PTcpReleaseCallbackFun;

class _SERVERAPI_EXPORT_ IBasicTcpServer : public IBasicServer
{
public:
	IBasicTcpServer(void);
	virtual ~IBasicTcpServer(void);
	friend class SocketContext;

	//发送数据
	virtual bool SendData(SocketContext* clientSocket, char* buf, unsigned long data_len) = 0;

	//是否开启超时检测(客户端一定时间没有与服务器交互, 将会断开连接) 默认是关闭的
	//timeoutMilSeconds - 毫秒
	bool OpenTimeoutCheck(bool bOpen, long timeoutMilSeconds = 1000);

	//设置回调函数, 用于非继承类
	void SetCallbackPointer(PTcpAcceptCallbackFun pTcpAcceptCallbackFun, PTcpReleaseCallbackFun pTcpReleaseCallbackFun, PTcpSendCallbackFun pTcpSendCompleteCallBack, PTcpReceiveCallbackFun pTcpReceiveCompleteCallBack);

protected:

	//关闭服务器具体实现
	virtual bool _StopServer();

	//连接建立完成
	virtual void AcceptComplete(SocketContext* clientSocket, char* ip) = 0;

	//数据发送完成
	virtual void SendComplete(SocketContext* clientSocket, char* buf, unsigned long data_len) = 0;

	//数据接收完成
	virtual void ReceiveComplete(SocketContext* clientSocket, char* buf, unsigned long data_len) = 0;

	//客户端发生断开时，回调事件(自动)，通知用户进行清理操作（用户必须在最后手动调用RemoveSocket）
	virtual void ReleaseClient(SocketContext* clientSocket) = 0;

	//检测突然掉线的socket超时时间
	virtual void ConnectTimeOutCheck();

	// 断客户端Socket是否保持连接
	bool IsSocketAlive(const SOCKET& s, const char* test = "#sockIsAlive#");

	//保存socket
	int  SaveSocketToMap(SOCKET socket); 

	//判断socket是否似乎合法的socket(是否在客户端队列里)
	bool IsValidSocket(SOCKET socket); 

	//关闭并从队列里移除socket
	void CloseSocket(SOCKET socket);

	//关闭所有socket
	void CloseAllSocket();

	//设置心跳检测 
	bool SetHeartBeatCheck(SOCKET socket, long keepalivetime = 5000, long keepaliveinterval = 1000);

protected:

	//超时检测函数
	static void PASCAL TimerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

protected:
	PTcpAcceptCallbackFun  m_pTcpAcceptCallbackFun;        //连接完毕回调指针
	PTcpReleaseCallbackFun m_pTcpReleaseCallbackFun;       //断开释放回调指针
	PTcpSendCallbackFun    m_pTcpSendCompleteCallBack;     //发送完毕回调指针
	PTcpReceiveCallbackFun m_pTcpReceiveCompleteCallBack;  //接收完毕回调指针
	long                   m_lCurrentTime;                 //用于超时检测(当前时间)
	long                   m_lTimeoutCheckTime;            //容忍的最大超时时间
	bool                   m_bOpenTimeoutCheck;            //是否开启超时检测
	CRITICAL_SECTION       m_csSocketMap;                  //Socket队列的互斥锁
	map<SOCKET, long>      m_mapClientSock;                //客户端Socket管理列表
	unsigned long          m_lTimerId;                     //定时器ID
};

