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

	//�ͷŵ�������Դ
	virtual void DeInitialize();

	//����ͻ���SocketMap
	void ClearClientSocketMap();

	//������Ϣ bSendAnsyc - �첽���� ���Ϊ�첽���ͣ�ͨ��sigSendResult���ط��ͽ��
	bool SendMsg(MsgHead msgHead, bool bSendAnsyc);

	//��������
	bool SendToClients(char* buf, unsigned long len);

Q_SIGNALS:
	void sigSendResult(int nMsgHead, bool bResult, const QString& strInfo);

protected:

	//���ӽ������
	virtual void AcceptComplete(SocketContext* clientSocket, char* ip);

	//���ݷ������
	virtual void SendComplete(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//���ݽ������
	virtual void ReceiveComplete(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//�ͻ��˷����Ͽ�ʱ���ص��¼�(�Զ�)��֪ͨ�û���������������û�����������ֶ�����RemoveSocket��
	virtual void ReleaseClient(SocketContext* clientSocket);

protected:
	QMap<QString, SocketContext*>  m_SocketMap;
	QVector<CMessageFactory*>      m_vecMessageFactory;         //�ֲ�ʽ��Ϣ����
	CMessageEventMediator*         m_pMessageEventMediator;
	QMutex                         m_SocketMutex;
	int                            m_nThreadNum;
};

