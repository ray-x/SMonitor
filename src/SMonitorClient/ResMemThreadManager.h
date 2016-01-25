#pragma once
#include <vector>
using namespace std;

extern long           g_dwTlsIndex;                //tls���ò�
extern long           g_dwThreadTlsId;             //tls��
extern unsigned short g_iWorkerThreadNumbers;      //�����߳�����

//�ֲ�ʽ�ڴ������
template <class InnerInfoType>
class CResMemThreadManager 
{
public:
	CResMemThreadManager() 
	{
		m_hMutex = CreateMutex(NULL, FALSE, "");
		Reset(FIX_MEMINITCOUNT, g_iWorkerThreadNumbers);
	}

	virtual ~CResMemThreadManager()
	{
		Clear();
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
	}

public:

	virtual InnerInfoType* GetMemory()
	{
		WaitForSingleObject(m_hMutex, INFINITE);
		InnerInfoType* pRetInfo = NULL;
		FixedMemoryPool<InnerInfoType>* pInfoMemoryPool = GetCurThreadInfo();
		if(NULL != pInfoMemoryPool)
		{
			void* pInfo = pInfoMemoryPool->GetMemory();
			if(NULL != pInfo)
			{
				pRetInfo = (InnerInfoType*)pInfo;
			}
		}
		ReleaseMutex(m_hMutex);
		return pRetInfo;
	}

	virtual bool FreeMemory(InnerInfoType* &pInfo, int nThreadID)
	{
// 		FixedMemoryPool<InnerInfoType>* pInfoMemoryPool = GetCurThreadInfo();
// 		if(NULL == pInfoMemoryPool)
// 			return false;
// 
// 		bool bRet = false;
// 		if(pInfoMemoryPool->FreeMemory(pInfo))
// 		{
// 			pInfo = NULL;
// 			bRet = true;
// 		}

		WaitForSingleObject(m_hMutex, INFINITE);
		
		bool bRet = false;
		FixedMemoryPool<InnerInfoType>* pInfoMemoryPool = GetThreadInfo(nThreadID);
		if(NULL != pInfoMemoryPool)
		{
			if(pInfoMemoryPool->FreeMemory(pInfo))
			{
				pInfo = NULL;
				bRet = true;
			}
		}

		ReleaseMutex(m_hMutex);

		return bRet;
	}

	//����
	void Reset(int nFixMemInitCount, int nThreadNum)
	{
		Clear();
		m_nThreadNum = nThreadNum;
		for (int i = 0; i < m_nThreadNum + 1; i++)
		{
			FixedMemoryPool<InnerInfoType>* pInfoMemoryPool = new FixedMemoryPool<InnerInfoType>(nFixMemInitCount, i);
			m_vecpInfoMemoryPool.push_back(pInfoMemoryPool);
		}
	}

	//���û���ڴ�ռ䣬������������
	void ApplySpace()
	{
		if(m_vecpInfoMemoryPool.size() <= 0)
		{
			Clear();
			Reset(FIX_MEMINITCOUNT, g_iWorkerThreadNumbers);
		}
	}

	//���
	void Clear()
	{
		for (vector<FixedMemoryPool<InnerInfoType>*>::iterator iter = m_vecpInfoMemoryPool.begin(); iter != m_vecpInfoMemoryPool.end(); iter++)
		{
			FixedMemoryPool<InnerInfoType>* pInfo = *iter;
			//SafeDeleteArray(pInfo);
			SafeDelete(pInfo); //yhb
		}

		m_vecpInfoMemoryPool.clear();
	}

	//��ȡ�̱߳�ţ��ֲ�ʽ���к�
	int GetThreadId()
	{
		int threadId = (int)TlsGetValue(g_dwTlsIndex);
		if(threadId <0 || threadId > m_nThreadNum)
			threadId = 0;

		return threadId;
	}

	void GetUsedCounter(DWORD& busy_counter)
	{
		FixedMemoryPool<InnerInfoType>* pInfoMemoryPool = GetCurThreadInfo();
		if(NULL == pInfoMemoryPool)
			return;

		pInfoMemoryPool->GetUsedCounter(busy_counter);
	}

protected:

	FixedMemoryPool<InnerInfoType>* GetCurThreadInfo()
	{
		int nThreadID = GetThreadId();
		int nSize = m_vecpInfoMemoryPool.size();
		if(nThreadID < 0 || nThreadID >= nSize)
			return NULL;

		return m_vecpInfoMemoryPool.at(nThreadID);
	}

	FixedMemoryPool<InnerInfoType>* GetThreadInfo(int nThreadID)
	{
		int nSize = m_vecpInfoMemoryPool.size();
		if(nThreadID < 0 || nThreadID >= nSize)
			return NULL;

		return m_vecpInfoMemoryPool.at(nThreadID);
	}

protected:
	vector<FixedMemoryPool<InnerInfoType>*> m_vecpInfoMemoryPool; //�ֲ�ʽ���������ڴ��
	int                                     m_nThreadNum;         //�߳���
	HANDLE                                  m_hMutex;             //�����ȡ��Դ
};