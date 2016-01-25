#ifndef CNetClt_H
#define CNetClt_H
#include "CNetStruct.h"
#include "../SMonitorServer/CNetMessage.h"
#include "SrvException.h"
#include <QStringList>
#include <QList>
#include <QObject>

class CMessageEventMediator;
class CMessageFactory;
class CNetClt : public QObject
{
	Q_OBJECT

public:
	CNetClt(CMessageEventMediator* pMessageEventMediator, QObject *parent = 0);
	~CNetClt();

	bool m_bIsOnLine;                                  //是否连上服务器

	// 开启
	bool Start();

	//	停止
	void Stop();

	// 设置连接IP地址
	void SetIP( LPCTSTR strIP );

	// 设置监听端口
	void SetPort(int nPort );

	//发送消息 bSendAnsyc - 异步发送 如果为异步发送，通过sigSendResult返回发送结果
	bool SendMsg(MsgHead msgHead, bool bSendAnsyc);

	//发送数据
	bool SendToSvr(char* buf, unsigned long len);

	//异步发送数据 发送完毕发送
	bool SendToSvrAnsyc(Buffer buffer);

	//发送测试模式数据
	void SendTextMsg();

Q_SIGNALS:
	void sigSendResult(int nMsgHead, bool bResult, const QString& strInfo);

protected:

	//发送消息 bSendAnsyc - 异步发送 如果为异步发送，通过sigSendResult返回发送结果
	bool _SendMsg(MsgHead msgHead, bool bSendAnsyc);

	//初始化为tcp客户端
	bool InitTcpClient(); 

	//将SOCKET作为客户端SOCKET
	bool AssociateAsTcpClient(SOCKET& socket);

	//连接服务器
	bool ConnectToServer();

	bool IssueTcpReceiveOp(SOCKET socket, unsigned short times = 1);
	bool IssueTcpReceiveOp(PER_IO_CONTEXT* pContext);

	//	清空资源
	void CleanUp();

	//网络通信线程
	static DWORD WINAPI CommProc(LPVOID lpParma);

	//工作处理线程
	static DWORD WINAPI WorkProc(LPVOID pParam);

	//发送线程
	static DWORD WINAPI SendProc(LPVOID pParam);

	//连接服务器线程
	static DWORD WINAPI ConnectProc(LPVOID pParam);

	//解析服务器发过来的内容
	bool TreatRecvData(const Buffer& recvBuffer);

	// 处理完成端口上的错误
	bool HandleError(SOCKET socket, PER_IO_CONTEXT* pIoContext);

	// 断客户端Socket是否保持连接
	bool IsSocketAlive(const SOCKET& s, const char* test = "#sockIsAlive#");

	PER_IO_CONTEXT* GetIOContext();
	void FreeIOContext(PER_IO_CONTEXT*& pIOContext);

	// 在有接收的数据到达的时候，进行处理
	bool DoRecv(SOCKET* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesRecved);

	// 在有数据发送完成的时候，进行处理
	bool DoSendComplete(SOCKET* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesSend);

public:

	//发送连接消息
	bool SendConnectMsg();

protected:

	//从注册表读取下一次连接服务器时间
	bool ReadNextConnectTime(QDateTime& connectTime);

	//生成下一次连接服务器时间(4分 0 - 24)
	void GetNextConnectTimeQuartFour(QDateTime& connectTime);

	//生成下一次连接服务器时间(5分 7 - 22)
	void GetNextConnectTimeQuartFive(QDateTime& connectTime);

	//将连接时间写入注册表
	void WriteNextConnectTime(QDateTime& connectTime);

private:
	SOCKET m_hTcpClientSocket;
	HANDLE m_hCompletionPort;
	HANDLE m_hShutdownEvent;                          // 用来通知线程系统退出的事件，为了能够更好的退出线程
	HANDLE m_hWorkerThread;                           // 任务处理的线程句柄
	HANDLE m_hSendThread;                             // 发送线程
	HANDLE m_hConnectThread;                          // 连接线程

	CResMemThreadManager<PER_IO_CONTEXT> m_TcpReceiveContextManager;
	QString   m_strServerIP;                          // 服务器端的IP地址
	int       m_nPort;                                // 监听端口
	CMessageFactory*       m_pMessageFactory;         // 消息工厂
	CMessageEventMediator* m_pMessageEventMediator;
	CRITICAL_SECTION          m_csRecvData;           // 用于接收数据的互斥量
	CRITICAL_SECTION          m_csSocket;             // 用于SOCKET的互斥量
	CRITICAL_SECTION          m_csSendData;           // 用于发送数据的互斥量
	CRITICAL_SECTION          m_csMemoryData;         // 用于申请内存池操作
	CRITICAL_SECTION          m_csSend;               // 发送SOCKET互斥
	CRITICAL_SECTION          m_csConnect;            // 用于连接状态判断的互斥量
	QList<Buffer>             m_RecvDataList;         // 保存接收到的数据
	QList<Buffer>             m_SendDataList;         // 异步发送数据列表
	HANDLE*                   m_phWorkerThreads;      // 工作者线程的句柄指针
	bool                      m_bNeedConnect;         // 是否需要连接服务器
	bool                      m_bConnectMsgSended;    // 连接信息是否已发送
	//CResMemThreadManager<PER_IO_CONTEXT> m_TcpSendContextManager;
};

#endif // CNetClt_H
