#pragma once
#include "SocketContext.h"
#include <winsock2.h>
#include <vector>
using namespace std;

//����ɶ˿���Ͷ�ݵ�I/O����������
typedef enum _NET_TYPE  
{  
	NET_ACCEPT,                      // Accept����
	NET_SEND,                        // ���Ͳ���
	NET_RECV,                        // ���ղ���
	NET_UNKNOWN                      // ��ʼ����δ֪����
}NET_TYPE;

//�����̵߳��̲߳���
class IOCompletionServer;
typedef struct _tagThreadParams_WORKER : public MEMPOOL_STU
{
	IOCompletionServer*	pIOCPNet;    // ��ָ��
	DWORD               dwThreadNo;  // �̱߳��

} THREADPARAMS_WORKER, *PTHREADPARAM_WORKER; 

//�ص�I0��������ṹ
typedef struct _PER_IO_CONTEXT : public MEMPOOL_STU
{
	OVERLAPPED	m_Overlapped;               // �ص��ṹ(���ÿһ��Socket��ÿһ����������Ҫ��һ��)              
	SOCKET      m_sockAccept;               // ������������ʹ�õ�Socket
	WSABUF      m_wsaBuf;                   // WSA���͵Ļ����������ڸ��ص�������������
	char		m_szBuffer[MAX_BUFFER_LEN]; // �����WSABUF�������ַ��Ļ�����
	NET_TYPE	m_OpType;                   // ��ʶ�������������(��Ӧ�����ö��)

	// ��ʼ��
	_PER_IO_CONTEXT();

	// ����
	~_PER_IO_CONTEXT();

	// ���û���������
	void ResetBuffer();

} PER_IO_CONTEXT, *PPER_IO_CONTEXT;

typedef class IOSocketContext : public SocketContext
{
public:
	//��ʼ��	
	IOSocketContext();

	//�ͷ���Դ
	~IOSocketContext();

	//��ȡһ���µ�IoContext
	_PER_IO_CONTEXT* GetNewIoContext(NET_TYPE net_Type);

	//���������Ƴ�һ��ָ����IoContext
	void RemoveContext(_PER_IO_CONTEXT* pContext, NET_TYPE net_Type);

	//������
	void ClearContext();

	//�ж�IO�Ƿ��Ѵ�����
	bool IsIOEmpty();
	bool IsIOEmpty(NET_TYPE net_Type);

private:
	vector<_PER_IO_CONTEXT*> m_arrayIoAcceptContext;       // �ͻ�������������������������ݣ�����Ͷ�ݶ��IO����
	vector<_PER_IO_CONTEXT*> m_arrayIoSendContext;         // �ͻ�����������ķ������������ݣ�����Ͷ�ݶ��IO����
	vector<_PER_IO_CONTEXT*> m_arrayIoRecvContext;         // �ͻ�����������Ľ������������ݣ�����Ͷ�ݶ��IO����
	HANDLE m_hAcceptMutex, m_hSendMutex, m_hRecvMutex;
	
}*PIOSocketContext;
