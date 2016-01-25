// SingleInstance.h: interface for the CSingleInstance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEINSTANCE_H__CBB88464_5236_4D77_AD08_AF93EDD1990B__INCLUDED_)
#define AFX_SINGLEINSTANCE_H__CBB88464_5236_4D77_AD08_AF93EDD1990B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <class classType>
class CSingleInstance  
{
public:
	//µ¥¼þÀà
	static  classType * GetInstance()
	{
		if(NULL == m_Instance)
		{
			m_Instance = new classType();
			atexit(Destroy);
		}
		
		return m_Instance;
	}

	virtual ~CSingleInstance(void){}

protected:
	CSingleInstance(void){}
	
	static void Destroy()
	{
		if(NULL != m_Instance)
		{
			delete m_Instance;
			m_Instance = NULL;
		}
	}

	static classType * m_Instance;
};

template <class classType>
classType * CSingleInstance<classType>::m_Instance = NULL;

#endif // !defined(AFX_SINGLEINSTANCE_H__CBB88464_5236_4D77_AD08_AF93EDD1990B__INCLUDED_)
