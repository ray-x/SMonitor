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

	bool m_bIsOnLine;                                  //�Ƿ����Ϸ�����

	// ����
	bool Start();

	//	ֹͣ
	void Stop();

	// ��������IP��ַ
	void SetIP( LPCTSTR strIP );

	// ���ü����˿�
	void SetPort(int nPort );

	//������Ϣ bSendAnsyc - �첽���� ���Ϊ�첽���ͣ�ͨ��sigSendResult���ط��ͽ��
	bool SendMsg(MsgHead msgHead, bool bSendAnsyc);

	//��������
	bool SendToSvr(char* buf, unsigned long len);

	//�첽�������� ������Ϸ���
	bool SendToSvrAnsyc(Buffer buffer);

	//���Ͳ���ģʽ����
	void SendTextMsg();

Q_SIGNALS:
	void sigSendResult(int nMsgHead, bool bResult, const QString& strInfo);

protected:

	//������Ϣ bSendAnsyc - �첽���� ���Ϊ�첽���ͣ�ͨ��sigSendResult���ط��ͽ��
	bool _SendMsg(MsgHead msgHead, bool bSendAnsyc);

	//��ʼ��Ϊtcp�ͻ���
	bool InitTcpClient(); 

	//��SOCKET��Ϊ�ͻ���SOCKET
	bool AssociateAsTcpClient(SOCKET& socket);

	//���ӷ�����
	bool ConnectToServer();

	bool IssueTcpReceiveOp(SOCKET socket, unsigned short times = 1);
	bool IssueTcpReceiveOp(PER_IO_CONTEXT* pContext);

	//	�����Դ
	void CleanUp();

	//����ͨ���߳�
	static DWORD WINAPI CommProc(LPVOID lpParma);

	//���������߳�
	static DWORD WINAPI WorkProc(LPVOID pParam);

	//�����߳�
	static DWORD WINAPI SendProc(LPVOID pParam);

	//���ӷ������߳�
	static DWORD WINAPI ConnectProc(LPVOID pParam);

	//����������������������
	bool TreatRecvData(const Buffer& recvBuffer);

	// ������ɶ˿��ϵĴ���
	bool HandleError(SOCKET socket, PER_IO_CONTEXT* pIoContext);

	// �Ͽͻ���Socket�Ƿ񱣳�����
	bool IsSocketAlive(const SOCKET& s, const char* test = "#sockIsAlive#");

	PER_IO_CONTEXT* GetIOContext();
	void FreeIOContext(PER_IO_CONTEXT*& pIOContext);

	// ���н��յ����ݵ����ʱ�򣬽��д���
	bool DoRecv(SOCKET* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesRecved);

	// �������ݷ�����ɵ�ʱ�򣬽��д���
	bool DoSendComplete(SOCKET* pSocketContext, PER_IO_CONTEXT* pIoContext, unsigned long dwBytesSend);

public:

	//����������Ϣ
	bool SendConnectMsg();

protected:

	//��ע����ȡ��һ�����ӷ�����ʱ��
	bool ReadNextConnectTime(QDateTime& connectTime);

	//������һ�����ӷ�����ʱ��(4�� 0 - 24)
	void GetNextConnectTimeQuartFour(QDateTime& connectTime);

	//������һ�����ӷ�����ʱ��(5�� 7 - 22)
	void GetNextConnectTimeQuartFive(QDateTime& connectTime);

	//������ʱ��д��ע���
	void WriteNextConnectTime(QDateTime& connectTime);

private:
	SOCKET m_hTcpClientSocket;
	HANDLE m_hCompletionPort;
	HANDLE m_hShutdownEvent;                          // ����֪ͨ�߳�ϵͳ�˳����¼���Ϊ���ܹ����õ��˳��߳�
	HANDLE m_hWorkerThread;                           // ��������߳̾��
	HANDLE m_hSendThread;                             // �����߳�
	HANDLE m_hConnectThread;                          // �����߳�

	CResMemThreadManager<PER_IO_CONTEXT> m_TcpReceiveContextManager;
	QString   m_strServerIP;                          // �������˵�IP��ַ
	int       m_nPort;                                // �����˿�
	CMessageFactory*       m_pMessageFactory;         // ��Ϣ����
	CMessageEventMediator* m_pMessageEventMediator;
	CRITICAL_SECTION          m_csRecvData;           // ���ڽ������ݵĻ�����
	CRITICAL_SECTION          m_csSocket;             // ����SOCKET�Ļ�����
	CRITICAL_SECTION          m_csSendData;           // ���ڷ������ݵĻ�����
	CRITICAL_SECTION          m_csMemoryData;         // ���������ڴ�ز���
	CRITICAL_SECTION          m_csSend;               // ����SOCKET����
	CRITICAL_SECTION          m_csConnect;            // ��������״̬�жϵĻ�����
	QList<Buffer>             m_RecvDataList;         // ������յ�������
	QList<Buffer>             m_SendDataList;         // �첽���������б�
	HANDLE*                   m_phWorkerThreads;      // �������̵߳ľ��ָ��
	bool                      m_bNeedConnect;         // �Ƿ���Ҫ���ӷ�����
	bool                      m_bConnectMsgSended;    // ������Ϣ�Ƿ��ѷ���
	//CResMemThreadManager<PER_IO_CONTEXT> m_TcpSendContextManager;
};

#endif // CNetClt_H
