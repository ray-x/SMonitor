#pragma once
#include "IServer.h"
#include <winsock2.h>

#include <map>
using namespace std;

//实现一些通用属性的服务器
class ServerMediator;
class _SERVERAPI_EXPORT_ IBasicServer : IServer
{
public:
	friend class ServerMediator;
	IBasicServer(void);
	virtual ~IBasicServer(void);

	//启动日志
	bool StartLogger(const char* strlogPath);

	//开启服务器
	virtual bool StartServer();

	//关闭服务器
	virtual bool StopServer();

	//手动释放资源 用户进行相关清理操作后，通知程序，删除SocketContext资源
	virtual void RemoveSocket(SocketContext* clientSocket);

	//清理所有SocketContext资源
	virtual void ClearAllSocketContext();

	//申请SocketContext资源
	virtual SocketContext* GetNewSocketContext();

	//释放掉所有资源
	virtual void DeInitialize();

	//设置本机IP地址, 可以为null,系统监听所有IP地址
	bool SetSvrIP(const char* chIP);

	//设置端口
	void SetSvrPort(int nPort);

	//设置工作的最大线程数量(-1 - 系统自己管理)
	void SetWorkThreadMaxNums(long dwMaxThreadNums = -1);

	//统一参数设置
	void SetOption(int nPort, long dwMaxBufferLen = 16384, long dwMaxPostAcceptNums = 10, long dwMaxThreadNums = -1);

	//获取服务器运行状态
	bool IsServerStart();

	//获取当前线程Tls号
	int GetThreadTlsId();

	//设置服务器最大并发连接数
	void SetMaxConnectClientsNum(int nMaxConnectClients);

public:
	bool    m_bServerIsStart;       //服务器启动标志
protected:

	//开启服务器具体实现
	virtual bool _StartServer();

	//关闭服务器具体实现
	virtual bool _StopServer();

	//设置套接字为异步非阻塞模式
	bool SetSocketAsync(SOCKET &socket, int nArgs = 1);

	//设置套接字为可重用
	bool SetSocketReused(SOCKET &socket, int nArgs = 1);

	//设置系统系统缓冲区大小
	bool SetSysBufferSize(SOCKET &socket, int nBufferSize, bool bSendBuffer = true);

	//服务器关闭socket的时候,不执行正常的四步握手关闭，而是执行RESET
	bool SetSocketResetWhenClose(SOCKET &socket, int nArgs = 1);

	//发送的时候关闭Nagle算法,关闭nagel算法有可能会引起断流(压缩)
	bool SetSocketSendNoDelay(SOCKET &socket, char chOpt = 1);

	//设置用户自定义数据
	bool SetSocketUserInfo(SOCKET &socket, const char* strInfo, int nSize);

protected:
	int                        m_nPort;                //服务器端口
	char                       m_chUserSetIP[128];     //用户指定的监听服务器IP地址
	long                       m_dwMaxBufferLen;       //最大缓冲区大小
	long                       m_dwMaxPostAcceptNums;  //同时投递的Accept请求的最大数量
	long                       m_dwMaxThreadNums;      //最大工作线程数量
	HANDLE	                   m_hShutdownEvent;       //用来通知服务器关闭的事件
	ServerMediator*            m_pMediator;            //中介者
	HANDLE                     m_hSocketContextMutex;  //增删SocketContext锁
	map<SocketContext*, long>  m_mapSocketContexts;    //SocketContexts列表     
	int                        m_nMaxConnectClients;   //最大连接客户端并发数 
};