#pragma once
#include "IOCompletionServer.h"
#include "IBasicTcpServer.h"
class _SERVERAPI_EXPORT_ TcpIOCompletionServer : public IOCompletionServer, public IBasicTcpServer
{
public:
	TcpIOCompletionServer(void);
	virtual ~TcpIOCompletionServer(void);

	//��������
	virtual bool SendData(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//�ͷŵ�������Դ
	virtual void DeInitialize();

protected:

	//��������������ʵ��
	virtual bool _StartServer();

	//�رշ���������ʵ��
	virtual bool _StopServer();

	//���ӽ������
	virtual void AcceptComplete(SocketContext* clientSocket, char* ip);

	//���ݷ������
	virtual void SendComplete(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//���ݽ������
	virtual void ReceiveComplete(SocketContext* clientSocket, char* buf, unsigned long data_len);

	//�ͻ��˷����Ͽ�ʱ���ص��¼�(�Զ�)��֪ͨ�û���������������û�����������ֶ�����RemoveSocket��
	virtual void ReleaseClient(SocketContext* clientSocket);

	//���ͻȻ���ߵ�socket��ʱʱ��
	virtual void ConnectTimeOutCheck();

	//��ʼ��Tls
	bool InitializeTls();

	// ��ʼ��Socket
	bool InitializeListenSocket();

	//��ʼ���߳�
	bool InitializeThread();

	//��ȡ��չ����ָ��
	bool GetExtensionFuncPointers();

	//Ͷ��Accept����
	bool PostAccept(int nAcceptCount);

	//Ͷ��Accept����
	bool PostAccept(PER_IO_CONTEXT* pAcceptIoContext);

	//Ͷ��Recv����
	bool PostRecv(PER_IO_CONTEXT* pIoContext);

	// ������ɶ˿��ϵĴ���
	bool HandleError(IOSocketContext *pContext, PER_IO_CONTEXT* pIoContext);

	// ���пͻ��������ʱ�򣬽��д���
	bool DoAccpet(IOSocketContext* pSocketContext, PER_IO_CONTEXT* pIoContext);

	// ���н��յ����ݵ����ʱ�򣬽��д���
	bool DoRecv(IOSocketContext* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesRecved);

	// �������ݷ�����ɵ�ʱ�򣬽��д���
	bool DoSendComplete(IOSocketContext* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesSend);

	// ������󶨵���ɶ˿���
	bool AssociateWithIOCP(IOSocketContext *pContext);

	// �������������
	bool IsOverMaxConnectNums();

protected:

	//�����̺߳���
	static UINT WINAPI WorkThread(LPVOID lpParma);

	//�����̺߳���
	static UINT WINAPI ListenThread(LPVOID lpParam); 

public:
	HANDLE                       m_hOSPostSocketAcceptEvent; //������AcceptEx���ñ��ľ���ʱ�򣬾ͻᴥ��FD_ACCEPT��osϵͳ���������¼�

protected:
	IOSocketContext*             m_pListenContext;           //���ڼ�����Socket��Context��Ϣ
	HANDLE                       m_hListenThread;            //����߳̾��
	LPFN_ACCEPTEX                m_lpfnAcceptEx;             //AcceptEx����ָ��
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs; //GetAcceptExSockAddrs����ָ��
	HANDLE                       m_hMaxConnectMutex;         //���Ⲣ�������������
};

