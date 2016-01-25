#pragma once

#ifdef _SERVERAPI_DLL_
#define _SERVERAPI_EXPORT_ __declspec(dllexport)	
#else
#define _SERVERAPI_EXPORT_ __declspec(dllimport)	
#endif

class _SERVERAPI_EXPORT_ IServer
{
public:
	IServer(void);
	virtual ~IServer(void);

	//开启服务器
	virtual bool StartServer() = 0;

	//关闭服务器
	virtual bool StopServer() = 0;
};

