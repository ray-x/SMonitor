#pragma once

// ��������С��16KB
#define MAX_BUFFER_LEN         16384 

// Ĭ�϶˿�
#define DEFAULT_PORT           14640  

// Ĭ��IP��ַ
#define DEFAULT_IP             _T("127.0.0.1")

// ͬʱͶ�ݵ�Accept���������
#define MAX_POST_ACCEPT        32 //32 

// ���ݸ�Worker�̵߳��˳��ź�
#define EXIT_CODE              NULL

// �ͷ�ָ���
#define RELEASE(x)             { if(NULL != (x) ){delete (x); (x) = NULL;} }

// �ͷ�ָ���
#define RELEASEONLY(x)             { if(NULL != (x) ){delete (x);} }

// �ͷž����
#define RELEASE_HANDLE(x)      { if(NULL != (x) && INVALID_HANDLE_VALUE != (x)){ CloseHandle(x); (x) = NULL;} }

// �ͷ�Socket��
#define RELEASE_SOCKET(x)      { if(INVALID_SOCKET != (x)) { CancelIo((HANDLE)(x)); shutdown((x), SD_BOTH); closesocket(x); (x) = INVALID_SOCKET;} }

//ɾ������
#define SafeDeleteArray(pData) { try { delete [] (pData); } catch (...) {  } (pData) = NULL; } 

//�ӿ��ͷ�
#define SafeRelease(pObject)   { if ((pObject) != NULL) { (pObject)->Release(); (pObject) = NULL; } }

//ɾ��ָ��
#define SafeDelete(pData)      { try { delete (pData); } catch (...) { } (pData) = NULL; } 

//������
#define OpenLink(String)	   ShellExecute(NULL, TEXT("open"), String, NULL, NULL, SW_SHOWNORMAL);

//����ά��
#define CountArray(Array)      (sizeof(Array)/sizeof((Array)[0]))

const int      DEFAULT_TCP_RECEIVE_BUFFER_SIZE = 4096;    //TCP���ܻ���
const int      FIX_MEMINITCOUNT = 50;                     //�����ڴ�س�ʼ��С
const int      TIMERURESOLUTION = 1000;                   //��ʱ����ʱ���ȣ���ֵԽС��ʱ���¼��ֱ���Խ��
const int      DEFAULT_TCP_SEND_BUFFER_SIZE = 1024 * 4;   //TCP���͵�������ֽ�����