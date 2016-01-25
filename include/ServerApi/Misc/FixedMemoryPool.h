#pragma once
#include <stdlib.h>

//ɾ��ָ��
#define SafeDelete(pData)      { try { delete (pData); } catch (...) { } (pData) = NULL; } 
const DWORD MIN_FREEMEMORY_SIZE = 1048576;//��С�ͷ��ڴ��ٽ�ֵ 1M
const DWORD MAX_MEMORY_SIZE = 10485760;   //�ڴ��������� 10M
const float MAX_IDLE_PERCENT = 0.5;       //���IDLE��
const int   FFIX_MEMINITCOUNT = 50;       //�����ڴ�س�ʼ��С

template <class T>
class IMemoryPool
{
public:
	virtual ~IMemoryPool(){};
	virtual T* GetMemory() = 0;
	virtual bool FreeMemory(T* ptrMemoryStu) = 0;
};

const int NFS_LINE_SIZE = 128;
typedef size_t uintptr;

class CMempool_cls
{
public:
	int managerIndex;  //��������index,�ڷֲ�ʽ��������

	//���ڴ��ַ��cpu cache �ж���,�������α����
	void* operator new(size_t size)  
	{
		unsigned char* p = (unsigned char*)malloc(size + NFS_LINE_SIZE);
		unsigned char* result = (unsigned char*)((uintptr)(p+NFS_LINE_SIZE)&-NFS_LINE_SIZE);
		((uintptr*)result)[-1] = (uintptr)p;  //������ʵ����ʼ��ַ
		return result;
	}

	void operator delete(void* p)
	{
		unsigned char* base = ((unsigned char**)p)[-1];  //�ͷ��������ڴ�
		free(base);
	}
};

typedef struct _MEMPOOL_STU
{
	int managerIndex;  //��������index,�ڷֲ�ʽ��������
	
	//���ڴ��ַ��cpu cache �ж���,�������α����
	void* operator new(size_t size)  
	{
		unsigned char* p = (unsigned char*)malloc(size + NFS_LINE_SIZE);
        unsigned char* result = (unsigned char*)((uintptr)(p+NFS_LINE_SIZE)&-NFS_LINE_SIZE);
		((uintptr*)result)[-1] = (uintptr)p;  //������ʵ����ʼ��ַ
		return result;
	}

	void operator delete(void* p)
	{
		unsigned char* base = ((unsigned char**)p)[-1];  //�ͷ��������ڴ�
		free(base);
	}
    
}MEMPOOL_STU;

//�Լ�ʵ�ִ����Ŀ���Ǹ�����

const unsigned long MAX_STACK_CAPACITY = 32768 * 4;  //ջ���������

template <class T>
class CStack
{

public:
	struct STACK_ARRAY{T* pContext;};
	CStack(long stackCapacity = MAX_STACK_CAPACITY);
	~CStack(void);
public:
	bool Push(T* pContext);
	bool Pop(T*& pContext);
	T* Pop();
	bool IsEmpty();
	bool IsFull();
	long Size();
public:
   long capacity;//ջ�������
   long size;//��ǰ��ջ��С;
   struct STACK_ARRAY* stack_unit;
};

template <class T>
CStack<T>::CStack(long stackCapacity)
{
	if(stackCapacity > MAX_STACK_CAPACITY)
		capacity = MAX_STACK_CAPACITY; 
	else
		capacity = stackCapacity;
   size = -1;
   stack_unit = new STACK_ARRAY[capacity];//����ָ������
}

template <class T>
CStack<T>::~CStack(void)
{
	delete []stack_unit;
    stack_unit = NULL;
	size = -1;
	capacity = 0;
}
template <class T>
bool CStack<T>::Push(T* pContext)
{
	if(pContext==NULL || stack_unit==NULL)
		return false;

	if(size+1>=capacity)//ջ��
		return false;

	stack_unit[++size].pContext = pContext;
	return true;
}
template <class T>
bool CStack<T>::Pop(T*& pContext)
{
	if(size>=0 && stack_unit)//ջ�ǿ�
	{  
		pContext = stack_unit[size--].pContext;

		if(NULL == pContext)
			int n = 3;

		stack_unit[size+1].pContext=NULL;
		return true;
	}
	return false;
}
template <class T>
T* CStack<T>::Pop()
{
	T* rc=NULL;
	if(size>=0 && stack_unit)//ջ�ǿ�
	{  
		rc = stack_unit[size--].pContext;
		stack_unit[size+1].pContext=NULL;
	}

	if(NULL == rc)
		int n = 2;

	return rc;
}
template <class T>
bool CStack<T>::IsEmpty()
{
	return size<0?true:false;
}
template <class T>
bool CStack<T>::IsFull()
{
	if(size+1==capacity)
		 return true;
	else
		 return false;
}
template <class T>
long CStack<T>::Size()
{
    return size+1;
}

template <class T>
class FixedMemoryPool : public IMemoryPool<T>
{
public:
	FixedMemoryPool(DWORD size, int index);

	virtual ~FixedMemoryPool(void);

	//�õ��ڴ�
	virtual T* GetMemory();

	//�ͷ��ڴ�
	virtual bool FreeMemory(T* ptrMemoryStu);

	//�õ��ڴ��������
	void GetSize(DWORD& size);

	//�õ������ڴ������
	void GetIdleCounter(DWORD& idle_counter);
	
	//�õ�ʹ���е��ڴ������
	void GetUsedCounter(DWORD& busy_counter);

public:

	//�Ƿ���Ҫ�ͷ��ڴ�
	virtual bool NeedToDeleteMemory();

private:
	CStack<T>* m_pStack;                    //�ڴ���ջ
    //CRITICAL_SECTION critical_section;    //�ڴ����ٽ���
	DWORD m_dwSize;                         //�ڴ���С
	DWORD m_dwUsedCount;                    //��ǰʹ�õ�����
	int m_iIndex;                           //�ֲ�ʽ����ʹ�ã���ʾ��ǰ���ĸ��̵߳ı�ʾ
	bool m_bDelete_flag;                    //�ڴ�Խ�ʱ��ɾ�������ڴ�ı�־
	//unsigned long m_iTimerId;             //��ʱ��ID
};

template <class T>
FixedMemoryPool<T>::FixedMemoryPool(DWORD size, int index)
{
	m_bDelete_flag = true;
	m_dwSize = size;
	m_iIndex = index;
	m_dwUsedCount = 0;
	//InitializeCriticalSectionAndSpinCount(&critical_section, 4000);
	
    m_pStack = NULL;
	m_pStack = new CStack<T>();
	if(NULL != m_pStack)
	{
		for(DWORD i = 0; i < m_dwSize; i++)
		{
			T* ptrMemoryStu = new T();
			ptrMemoryStu->managerIndex = m_iIndex;
			m_pStack->Push(ptrMemoryStu);
		}
	}
}

template <class T>
FixedMemoryPool<T>::~FixedMemoryPool()
{
	if(NULL != m_pStack)
	{
		for(DWORD i = 0; i < m_dwSize;i++)
		{
			if(m_pStack->Size() <= 0)
				break;

			T* ptrMemoryStu = m_pStack->Pop();
			if(ptrMemoryStu)
				delete ptrMemoryStu;
		}
	}
	
	m_bDelete_flag = true;
	m_dwSize = 0;

	//::DeleteCriticalSection(&critical_section);

	SafeDelete(m_pStack);
}

//�õ��ڴ�
template <class T>
T* FixedMemoryPool<T>::GetMemory()
{
	//EnterCriticalSection(&critical_section);
	//if(TryEnterCriticalSection(&critical_section)) //�˴��п�������ڴ�����������
	//{

	T *pMemoryStu = NULL;
	if(m_pStack)
	{
		if(m_pStack->IsEmpty())
		{
			pMemoryStu = new T();
			m_dwSize++;
		}
		else
		{
			m_pStack->Pop(pMemoryStu);
			if(NULL == pMemoryStu)
			{
				pMemoryStu = new T();
				m_dwSize++;
			}
		}
		
		pMemoryStu->managerIndex = m_iIndex;  //�ֲ�ʽ����ʹ��
		m_dwUsedCount++;
	}
	//	LeaveCriticalSection(&critical_section);
	//}
	//else
	//{
	//	ptrMemoryStu = new T();  //�������Զ�ɾ��
	//	m_dwSize++;
	//}

	return pMemoryStu;
}

template <class T>
bool FixedMemoryPool<T>::FreeMemory(T* ptrMemoryStu) //û�п�����ջʧ������
{
	//EnterCriticalSection(&critical_section);
	//if(TryEnterCriticalSection(&critical_section)) //�˴��п�������ڴ�����������
	//{
	
	bool bRet = true;
	if(m_pStack)
	{
		if(NeedToDeleteMemory() || m_pStack->IsFull())
		{
			SafeDelete(ptrMemoryStu);
			m_dwSize--;
// 			delete_counter++;
// 			if(delete_counter >= delete_stop_val)
// 			{
// 				delete_flag = false;
// 				delete_counter = 0;
// 			}
		}
		else
		{
			bRet = m_pStack->Push(ptrMemoryStu); //��ջʧ��
			if(!bRet)
			{
				SafeDelete(ptrMemoryStu);
				m_dwSize--;
				bRet = true;
			}
		}

		m_dwUsedCount--;
	}
	else
	{
		SafeDelete(ptrMemoryStu);
		m_dwSize = 0;
		m_dwUsedCount = 0;
	}
	//	LeaveCriticalSection(&critical_section);
	//}
	//else
	//{
	//	delete ptrMemoryStu;
	//	m_iSize--;
	//}

	return bRet;
}

//�õ�pool��size
template <class T>
void FixedMemoryPool<T>::GetSize(DWORD& size)
{
    InterlockedExchange(&size, m_dwSize);
}
//�õ����гص�size
template <class T>
void FixedMemoryPool<T>::GetIdleCounter(DWORD& idle_counter)
{
	InterlockedExchange(&idle_counter, m_dwSize - m_dwUsedCount);
}
//�õ�busy�ص�size
template <class T>
void FixedMemoryPool<T>::GetUsedCounter(DWORD& busy_counter)
{
	InterlockedExchange(&busy_counter, m_dwUsedCount);
}

//�Ƿ���Ҫ�ͷ��ڴ�
template <class T>
bool FixedMemoryPool<T>::NeedToDeleteMemory()
{
	if(m_bDelete_flag)
	{
		DWORD dwTotalSize = m_dwSize;
		DWORD dwUsedSize = m_dwUsedCount;

		DWORD dwTotalMemorySize = dwTotalSize * sizeof(T);
		if(dwTotalMemorySize > MAX_MEMORY_SIZE)
		{//�ڴ��������С����
			return true;
		}

		if(dwTotalSize > FFIX_MEMINITCOUNT)
		{
			DWORD dwIdleCount = dwTotalSize - dwUsedSize;
			if(dwIdleCount > dwTotalSize * MAX_IDLE_PERCENT)
			{//���������ȹ���
				return true;
			}
		}
	}

	return false;
	
// 	����������
// 	DWORD dwIdleCount = 0;
// 	GetIdleCounter(dwIdleCount);
// 	
// 	DWORD dwIdleMemorySize = dwIdleCount * sizeof(T);
// 	if(dwIdleMemorySize < MIN_FREEMEMORY_SIZE)
// 	{//���С����С�ڴ棬����Ҫ�ͷ�
// 		return false;
// 	}
// 	
// 	return true;
}