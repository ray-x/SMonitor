#pragma once

#ifdef _SERVERAPI_DLL_
#define _LOG_EXPORT_ __declspec(dllexport)	
#else
#define _LOG_EXPORT_ __declspec(dllimport)	
#endif

class _LOG_EXPORT_ CCommonLog
{
public:
	CCommonLog(void);
	virtual ~CCommonLog(void);

	//������־ϵͳ
	bool Start(const char* strLogPath);

	//�����־
	void AddLog(const char* lpszFormat, ...);

	//��ȡ���һ����־
	bool GetLastLog(char *strLog, int nLen);

protected:
	class CLogger* m_pLogger;
};

