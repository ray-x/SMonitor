#pragma once
#include <stdlib.h>

//删除指针
#define SafeDelete(pData)      { try { delete (pData); } catch (...) { } (pData) = NULL; } 
const DWORD MIN_FREEMEMORY_SIZE = 1048576;//最小释放内存临界值 1M
const DWORD MAX_MEMORY_SIZE = 10485760;   //内存池最大容量 10M
const float MAX_IDLE_PERCENT = 0.5;       //最大IDLE比
const int   FFIX_MEMINITCOUNT = 50;       //公共内存池初始大小

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
	int managerIndex;  //管理器的index,在分布式队列下用

	//让内存地址与cpu cache 行对其,避免出现伪共享
	void* operator new(size_t size)  
	{
		unsigned char* p = (unsigned char*)malloc(size + NFS_LINE_SIZE);
		unsigned char* result = (unsigned char*)((uintptr)(p+NFS_LINE_SIZE)&-NFS_LINE_SIZE);
		((uintptr*)result)[-1] = (uintptr)p;  //保存真实的起始地址
		return result;
	}

	void operator delete(void* p)
	{
		unsigned char* base = ((unsigned char**)p)[-1];  //释放真正的内存
		free(base);
	}
};

typedef struct _MEMPOOL_STU
{
	int managerIndex;  //管理器的index,在分布式队列下用
	
	//让内存地址与cpu cache 行对其,避免出现伪共享
	void* operator new(size_t size)  
	{
		unsigned char* p = (unsigned char*)malloc(size + NFS_LINE_SIZE);
        unsigned char* result = (unsigned char*)((uintptr)(p+NFS_LINE_SIZE)&-NFS_LINE_SIZE);
		((uintptr*)result)[-1] = (uintptr)p;  //保存真实的起始地址
		return result;
	}

	void operator delete(void* p)
	{
		unsigned char* base = ((unsigned char**)p)[-1];  //释放真正的内存
		free(base);
	}
    
}MEMPOOL_STU;

//自己实现此类的目标是高性能

const unsigned long MAX_STACK_CAPACITY = 32768 * 4;  //栈的最大容量

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
   long capacity;//栈最大容量
   long size;//当前的栈大小;
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
   stack_unit = new STACK_ARRAY[capacity];//对象指针数组
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

	if(size+1>=capacity)//栈满
		return false;

	stack_unit[++size].pContext = pContext;
	return true;
}
template <class T>
bool CStack<T>::Pop(T*& pContext)
{
	if(size>=0 && stack_unit)//栈非空
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
	if(size>=0 && stack_unit)//栈非空
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

	//得到内存
	virtual T* GetMemory();

	//释放内存
	virtual bool FreeMemory(T* ptrMemoryStu);

	//得到内存池总容量
	void GetSize(DWORD& size);

	//得到空闲内存池容量
	void GetIdleCounter(DWORD& idle_counter);
	
	//得到使用中的内存池容量
	void GetUsedCounter(DWORD& busy_counter);

public:

	//是否需要释放内存
	virtual bool NeedToDeleteMemory();

private:
	CStack<T>* m_pStack;                    //内存块堆栈
    //CRITICAL_SECTION critical_section;    //内存检查临界区
	DWORD m_dwSize;                         //内存块大小
	DWORD m_dwUsedCount;                    //当前使用的数量
	int m_iIndex;                           //分布式队列使用，表示当前是哪个线程的标示
	bool m_bDelete_flag;                    //内存吃紧时，删除多余内存的标志
	//unsigned long m_iTimerId;             //定时器ID
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

//得到内存
template <class T>
T* FixedMemoryPool<T>::GetMemory()
{
	//EnterCriticalSection(&critical_section);
	//if(TryEnterCriticalSection(&critical_section)) //此处有可能造成内存分配剧烈增加
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
		
		pMemoryStu->managerIndex = m_iIndex;  //分布式队列使用
		m_dwUsedCount++;
	}
	//	LeaveCriticalSection(&critical_section);
	//}
	//else
	//{
	//	ptrMemoryStu = new T();  //不纳入自动删除
	//	m_dwSize++;
	//}

	return pMemoryStu;
}

template <class T>
bool FixedMemoryPool<T>::FreeMemory(T* ptrMemoryStu) //没有考虑入栈失败因素
{
	//EnterCriticalSection(&critical_section);
	//if(TryEnterCriticalSection(&critical_section)) //此处有可能造成内存分配剧烈增加
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
			bRet = m_pStack->Push(ptrMemoryStu); //入栈失败
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

//得到pool的size
template <class T>
void FixedMemoryPool<T>::GetSize(DWORD& size)
{
    InterlockedExchange(&size, m_dwSize);
}
//得到空闲池的size
template <class T>
void FixedMemoryPool<T>::GetIdleCounter(DWORD& idle_counter)
{
	InterlockedExchange(&idle_counter, m_dwSize - m_dwUsedCount);
}
//得到busy池的size
template <class T>
void FixedMemoryPool<T>::GetUsedCounter(DWORD& busy_counter)
{
	InterlockedExchange(&busy_counter, m_dwUsedCount);
}

//是否需要释放内存
template <class T>
bool FixedMemoryPool<T>::NeedToDeleteMemory()
{
	if(m_bDelete_flag)
	{
		DWORD dwTotalSize = m_dwSize;
		DWORD dwUsedSize = m_dwUsedCount;

		DWORD dwTotalMemorySize = dwTotalSize * sizeof(T);
		if(dwTotalMemorySize > MAX_MEMORY_SIZE)
		{//内存池容量大小过大
			return true;
		}

		if(dwTotalSize > FFIX_MEMINITCOUNT)
		{
			DWORD dwIdleCount = dwTotalSize - dwUsedSize;
			if(dwIdleCount > dwTotalSize * MAX_IDLE_PERCENT)
			{//空闲容量比过多
				return true;
			}
		}
	}

	return false;
	
// 	检测空闲容量
// 	DWORD dwIdleCount = 0;
// 	GetIdleCounter(dwIdleCount);
// 	
// 	DWORD dwIdleMemorySize = dwIdleCount * sizeof(T);
// 	if(dwIdleMemorySize < MIN_FREEMEMORY_SIZE)
// 	{//如果小于最小内存，则不需要释放
// 		return false;
// 	}
// 	
// 	return true;
}