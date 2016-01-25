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

	//��������
	virtual bool SendData(SocketContext* clientSocket, char* buf, unsigned long data_len) = 0;

	//�Ƿ�����ʱ���(�ͻ���һ��ʱ��û�������������, ����Ͽ�����) Ĭ���ǹرյ�
	//timeoutMilSeconds - ����
	bool OpenTimeoutCheck(bool bOpen, long timeoutMilSeconds = 1000);

	//���ûص�����, ���ڷǼ̳���
	void SetCallbackPointer(PTcpAcceptCallbackFun pTcpAcceptCallbackFun, PTcpReleaseCallbackFun pTcpReleaseCallbackFun, PTcpSendCallbackFun pTcpSendCompleteCallBack, PTcpReceiveCallbackFun pTcpReceiveCompleteCallBack);

protected:

	//�رշ���������ʵ��
	virtual bool _StopServer();

	//���ӽ������
	virtual void AcceptComplete(SocketContext* clientSocket, char* ip) = 0;

	//���ݷ������
	virtual void SendComplete(SocketContext* clientSocket, char* buf, unsigned long data_len) = 0;

	//���ݽ������
	virtual void ReceiveComplete(SocketContext* clientSocket, char* buf, unsigned long data_len) = 0;

	//�ͻ��˷����Ͽ�ʱ���ص��¼�(�Զ�)��֪ͨ�û���������������û�����������ֶ�����RemoveSocket��
	virtual void ReleaseClient(SocketContext* clientSocket) = 0;

	//���ͻȻ���ߵ�socket��ʱʱ��
	virtual void ConnectTimeOutCheck();

	// �Ͽͻ���Socket�Ƿ񱣳�����
	bool IsSocketAlive(const SOCKET& s, const char* test = "#sockIsAlive#");

	//����socket
	int  SaveSocketToMap(SOCKET socket); 

	//�ж�socket�Ƿ��ƺ��Ϸ���socket(�Ƿ��ڿͻ��˶�����)
	bool IsValidSocket(SOCKET socket); 

	//�رղ��Ӷ������Ƴ�socket
	void CloseSocket(SOCKET socket);

	//�ر�����socket
	void CloseAllSocket();

	//����������� 
	bool SetHeartBeatCheck(SOCKET socket, long keepalivetime = 5000, long keepaliveinterval = 1000);

protected:

	//��ʱ��⺯��
	static void PASCAL TimerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

protected:
	PTcpAcceptCallbackFun  m_pTcpAcceptCallbackFun;        //������ϻص�ָ��
	PTcpReleaseCallbackFun m_pTcpReleaseCallbackFun;       //�Ͽ��ͷŻص�ָ��
	PTcpSendCallbackFun    m_pTcpSendCompleteCallBack;     //������ϻص�ָ��
	PTcpReceiveCallbackFun m_pTcpReceiveCompleteCallBack;  //������ϻص�ָ��
	long                   m_lCurrentTime;                 //���ڳ�ʱ���(��ǰʱ��)
	long                   m_lTimeoutCheckTime;            //���̵����ʱʱ��
	bool                   m_bOpenTimeoutCheck;            //�Ƿ�����ʱ���
	CRITICAL_SECTION       m_csSocketMap;                  //Socket���еĻ�����
	map<SOCKET, long>      m_mapClientSock;                //�ͻ���Socket�����б�
	unsigned long          m_lTimerId;                     //��ʱ��ID
};

