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

	//启动日志系统
	bool Start(const char* strLogPath);

	//添加日志
	void AddLog(const char* lpszFormat, ...);

	//获取最后一条日志
	bool GetLastLog(char *strLog, int nLen);

protected:
	class CLogger* m_pLogger;
};

