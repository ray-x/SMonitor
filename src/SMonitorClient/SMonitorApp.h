#pragma once

// CSMonitorApp
#include <QString>



class CSMonitorApp : public CWinApp
{
	DECLARE_DYNCREATE(CSMonitorApp)

public:
	CSMonitorApp();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CSMonitorApp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	void readConfigure();
	QString getId();

	QString m_strSvrIP;
	QString m_strHostName;
	QString m_strClientID;
	int     m_nSvrPort;

protected:
	DECLARE_MESSAGE_MAP()
};


