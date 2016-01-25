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
	//创建完成端口
	//---------------------------------------------------------------------------

	//新建一个完成端口
	HANDLE CreateNewIOCompletionPort(DWORD dwNumOfConcurrentThreads);

	//---------------------------------------------------------------------------
	//绑定完成端口
	//---------------------------------------------------------------------------

	//将一个设备句柄与完成端口绑定(关联在一起) 可以关联多个设备
	//hDevice设备句柄包括 SOCKET，文件，油槽，管道等
	//dwCompletionKey 自定义 参数 在线程里取出使用
	bool BindIOCompletionPort(HANDLE hDevice, HANDLE hIOCompletionPort, DWORD dwCompletionKey);

	//将套接字与完成端口绑定
	bool BindSocketToIOCompletionPort(SOCKET &socket, HANDLE hIOCompletionPort, DWORD dwCompletionKey);


protected:

	HANDLE   m_hIOCompletionPort;  //完成端口的句柄
	HANDLE*  m_phWorkerThreads;    //工作者线程的句柄指针
};

