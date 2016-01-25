#pragma once

template <class InnerInfoType>
class CResMemManager
{
public:

	CResMemManager(int nFixMemInitCount)
	{
		m_pInfoMemoryPool = new FixedMemoryPool<InnerInfoType>(nFixMemInitCount, 0);
	}

	CResMemManager(void)
	{
		m_pInfoMemoryPool = new FixedMemoryPool<InnerInfoType>(FIX_MEMINITCOUNT, 0);
	}

	virtual ~CResMemManager(void)
	{
		//SafeDeleteArray(m_pInfoMemoryPool);
		SafeDelete(m_pInfoMemoryPool);
	}

public:

	virtual void* GetMemory()
	{
		return m_pInfoMemoryPool->GetMemory();
	}

	virtual bool FreeMemory(InnerInfoType* &pInfo)
	{
		bool bRet = false;
		if(m_pInfoMemoryPool->FreeMemory(pInfo))
		{
			pInfo = NULL;
			bRet = true;
		}

		return bRet;
	}

protected:
	FixedMemoryPool<InnerInfoType> *m_pInfoMemoryPool; //基础类型内存池
};

