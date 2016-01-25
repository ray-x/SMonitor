#pragma once
#include "IServer.h"
#include <winsock2.h>

#include <map>
using namespace std;

//ʵ��һЩͨ�����Եķ�����
class ServerMediator;
class _SERVERAPI_EXPORT_ IBasicServer : IServer
{
public:
	friend class ServerMediator;
	IBasicServer(void);
	virtual ~IBasicServer(void);

	//������־
	bool StartLogger(const char* strlogPath);

	//����������
	virtual bool StartServer();

	//�رշ�����
	virtual bool StopServer();

	//�ֶ��ͷ���Դ �û�����������������֪ͨ����ɾ��SocketContext��Դ
	virtual void RemoveSocket(SocketContext* clientSocket);

	//��������SocketContext��Դ
	virtual void ClearAllSocketContext();

	//����SocketContext��Դ
	virtual SocketContext* GetNewSocketContext();

	//�ͷŵ�������Դ
	virtual void DeInitialize();

	//���ñ���IP��ַ, ����Ϊnull,ϵͳ��������IP��ַ
	bool SetSvrIP(const char* chIP);

	//���ö˿�
	void SetSvrPort(int nPort);

	//���ù���������߳�����(-1 - ϵͳ�Լ�����)
	void SetWorkThreadMaxNums(long dwMaxThreadNums = -1);

	//ͳһ��������
	void SetOption(int nPort, long dwMaxBufferLen = 16384, long dwMaxPostAcceptNums = 10, long dwMaxThreadNums = -1);

	//��ȡ����������״̬
	bool IsServerStart();

	//��ȡ��ǰ�߳�Tls��
	int GetThreadTlsId();

	//���÷�������󲢷�������
	void SetMaxConnectClientsNum(int nMaxConnectClients);

public:
	bool    m_bServerIsStart;       //������������־
protected:

	//��������������ʵ��
	virtual bool _StartServer();

	//�رշ���������ʵ��
	virtual bool _StopServer();

	//�����׽���Ϊ�첽������ģʽ
	bool SetSocketAsync(SOCKET &socket, int nArgs = 1);

	//�����׽���Ϊ������
	bool SetSocketReused(SOCKET &socket, int nArgs = 1);

	//����ϵͳϵͳ��������С
	bool SetSysBufferSize(SOCKET &socket, int nBufferSize, bool bSendBuffer = true);

	//�������ر�socket��ʱ��,��ִ���������Ĳ����ֹرգ�����ִ��RESET
	bool SetSocketResetWhenClose(SOCKET &socket, int nArgs = 1);

	//���͵�ʱ��ر�Nagle�㷨,�ر�nagel�㷨�п��ܻ��������(ѹ��)
	bool SetSocketSendNoDelay(SOCKET &socket, char chOpt = 1);

	//�����û��Զ�������
	bool SetSocketUserInfo(SOCKET &socket, const char* strInfo, int nSize);

protected:
	int                        m_nPort;                //�������˿�
	char                       m_chUserSetIP[128];     //�û�ָ���ļ���������IP��ַ
	long                       m_dwMaxBufferLen;       //��󻺳�����С
	long                       m_dwMaxPostAcceptNums;  //ͬʱͶ�ݵ�Accept������������
	long                       m_dwMaxThreadNums;      //������߳�����
	HANDLE	                   m_hShutdownEvent;       //����֪ͨ�������رյ��¼�
	ServerMediator*            m_pMediator;            //�н���
	HANDLE                     m_hSocketContextMutex;  //��ɾSocketContext��
	map<SocketContext*, long>  m_mapSocketContexts;    //SocketContexts�б�     
	int                        m_nMaxConnectClients;   //������ӿͻ��˲����� 
};