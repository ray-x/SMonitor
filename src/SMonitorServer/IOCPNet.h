#pragma once

#include "../../include/ServerApi/Server/TcpIOCompletionServer.h"
#include <QMap>
#include <QObject>
#include <QMutex>
#include <QVector>

class CMessageEventMediator;
class CMessageFactory;
class CIOCPNet : public TcpIOCompletionServer
{
public:
	CIOCPNet(CMessageEventMediator* pMessageEventMediator);
	virtual ~CIOCPNet(void);

	//释放掉所有资源
	virtual void DeInitialize();

	//清理客户端SocketMap
	void ClearClientSocketMap();

	//发送消息 bSendAnsyc - 异步发送 如果为异步发送，通过sigSendResult返回发送结果
	bool SendMsg(MsgHead msgHead, bool bSendAnsyc);

	//发送数据
	bool SendToClients(char* buf, unsigned long len);

Q_SIGNALS:
	void sigSendResult(int nMsgHead, bool bResult, const QString& strInfo);

protected:

	//连接建立完成
	virtual void AcceptComplete(SocketContext* clientSocket, char* ip);

	//数据发送完成
	virtual void SendComplete(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//数据接收完成
	virtual void ReceiveComplete(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//客户端发生断开时，回调事件(自动)，通知用户进行清理操作（用户必须在最后手动调用RemoveSocket）
	virtual void ReleaseClient(SocketContext* clientSocket);

protected:
	QMap<QString, SocketContext*>  m_SocketMap;
	QVector<CMessageFactory*>      m_vecMessageFactory;         //分布式消息工厂
	CMessageEventMediator*         m_pMessageEventMediator;
	QMutex                         m_SocketMutex;
	int                            m_nThreadNum;
};

