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

	//����������
	virtual bool StartServer() = 0;

	//�رշ�����
	virtual bool StopServer() = 0;
};

