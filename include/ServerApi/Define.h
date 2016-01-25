#pragma once

// 缓冲区大小，16KB
#define MAX_BUFFER_LEN         16384 

// 默认端口
#define DEFAULT_PORT           14640  

// 默认IP地址
#define DEFAULT_IP             _T("127.0.0.1")

// 同时投递的Accept请求的数量
#define MAX_POST_ACCEPT        32 //32 

// 传递给Worker线程的退出信号
#define EXIT_CODE              NULL

// 释放指针宏
#define RELEASE(x)             { if(NULL != (x) ){delete (x); (x) = NULL;} }

// 释放指针宏
#define RELEASEONLY(x)             { if(NULL != (x) ){delete (x);} }

// 释放句柄宏
#define RELEASE_HANDLE(x)      { if(NULL != (x) && INVALID_HANDLE_VALUE != (x)){ CloseHandle(x); (x) = NULL;} }

// 释放Socket宏
#define RELEASE_SOCKET(x)      { if(INVALID_SOCKET != (x)) { CancelIo((HANDLE)(x)); shutdown((x), SD_BOTH); closesocket(x); (x) = INVALID_SOCKET;} }

//删除数组
#define SafeDeleteArray(pData) { try { delete [] (pData); } catch (...) {  } (pData) = NULL; } 

//接口释放
#define SafeRelease(pObject)   { if ((pObject) != NULL) { (pObject)->Release(); (pObject) = NULL; } }

//删除指针
#define SafeDelete(pData)      { try { delete (pData); } catch (...) { } (pData) = NULL; } 

//打开链接
#define OpenLink(String)	   ShellExecute(NULL, TEXT("open"), String, NULL, NULL, SW_SHOWNORMAL);

//数组维数
#define CountArray(Array)      (sizeof(Array)/sizeof((Array)[0]))

const int      DEFAULT_TCP_RECEIVE_BUFFER_SIZE = 4096;    //TCP接受缓存
const int      FIX_MEMINITCOUNT = 50;                     //公共内存池初始大小
const int      TIMERURESOLUTION = 1000;                   //定时器延时精度，数值越小定时器事件分辨率越高
const int      DEFAULT_TCP_SEND_BUFFER_SIZE = 1024 * 4;   //TCP发送单次最大字节限制