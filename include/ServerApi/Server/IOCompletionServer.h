#pragma once
#include "IServer.h"
#include "IOCompletionContext.h"
class _SERVERAPI_EXPORT_ IOCompletionServer
{
public:
	IOCompletionServer(void);
	virtual ~IOCompletionServer(void);

protected:

	//---------------------------------------------------------------------------
	//������ɶ˿�
	//---------------------------------------------------------------------------

	//�½�һ����ɶ˿�
	HANDLE CreateNewIOCompletionPort(DWORD dwNumOfConcurrentThreads);

	//---------------------------------------------------------------------------
	//����ɶ˿�
	//---------------------------------------------------------------------------

	//��һ���豸�������ɶ˿ڰ�(������һ��) ���Թ�������豸
	//hDevice�豸������� SOCKET���ļ����Ͳۣ��ܵ���
	//dwCompletionKey �Զ��� ���� ���߳���ȡ��ʹ��
	bool BindIOCompletionPort(HANDLE hDevice, HANDLE hIOCompletionPort, DWORD dwCompletionKey);

	//���׽�������ɶ˿ڰ�
	bool BindSocketToIOCompletionPort(SOCKET &socket, HANDLE hIOCompletionPort, DWORD dwCompletionKey);


protected:

	HANDLE   m_hIOCompletionPort;  //��ɶ˿ڵľ��
	HANDLE*  m_phWorkerThreads;    //�������̵߳ľ��ָ��
};

