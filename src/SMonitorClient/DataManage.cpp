// DataManage.cpp: implementation of the CODBCDataManage class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DataManage.h"

//////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CODBCDataManage::CODBCDataManage(LPCSTR lpDataBaseName, LPCTSTR lpPassword)
{
	m_hEnvironment = NULL;
	m_hConnection = NULL;
	m_hStatment = NULL;
	m_strDataBaseName = lpDataBaseName;
	m_strPassword = lpPassword;
}

CODBCDataManage::CODBCDataManage()
{
	m_hEnvironment = NULL;
	m_hConnection = NULL;
	m_hStatment = NULL;
}

CODBCDataManage::~CODBCDataManage()
{
	ReleaseRecordset();
	ReleaseConnection();
	ClearValueMap();
}

//设置数据库
void CODBCDataManage::SetDataBase(LPCSTR lpDataBaseName, LPCTSTR lpPassword)
{
	m_strDataBaseName = lpDataBaseName;
	m_strPassword = lpPassword;
}

//初始化数据库
bool CODBCDataManage::OnInitDBConnect(char * error)
{
	//已经建立连接，直接返回
	if(NULL != m_hConnection)
		return true;

	//初始化OLE/COM库
	try
	{
		//初始化环境变量
		RETCODE rc = SQLAllocEnv(&m_hEnvironment);
		if(!SQL_SUCCEEDED(rc) || NULL == m_hEnvironment)
		{
			sprintf(error, "SQLAllocEnv error %d", rc);
			goto _error;
		}

		//初始化连接句柄
		rc = SQLAllocConnect(m_hEnvironment, &m_hConnection);
		if(!SQL_SUCCEEDED(rc) || NULL == m_hConnection)
		{
			sprintf(error, "SQLAllocConnect error %d", rc);
			goto _error;
		}

		//构建连接字串
		char chConnectStr[512] = {0}; 
		sprintf(chConnectStr, "Driver={Microsoft Access Driver (*.mdb)};Dbq=%s;Pwd=%s;", m_strDataBaseName.c_str(), m_strPassword.c_str());

		//连接数据库
		int     iConnStrLength2Ptr = 0;
		char    szConnStrOut[256] = {0}; 
		rc = SQLDriverConnect(m_hConnection, NULL, (unsigned char*)chConnectStr, SQL_NTS, (unsigned char*)szConnStrOut, 
					255, (SQLSMALLINT*)&iConnStrLength2Ptr, SQL_DRIVER_NOPROMPT);
	
		if(!SQL_SUCCEEDED(rc))
		{
			sprintf(error, "SQLDriverConnect error %d", rc);
			goto _error;
		}

		return true;
	}
	catch(...)
	{
		ReleaseConnection();
	}

_error:
	ReleaseConnection();

	return false;
}

//创建记录集
bool CODBCDataManage::CreateRecordSet(LPCSTR lpSQL, char * error)
{
	m_ColNameMap.clear();
	ClearValueMap();

	//执行查询语句
	if(!ExecuteSQL(lpSQL, error, false))
		return false;

	//获取查询结果的标题头
	SQLSMALLINT nFieldCount = 0, nCurrentField = 1;
	RETCODE rc = SQLNumResultCols(m_hStatment, &nFieldCount);
	if(!SQL_SUCCEEDED(rc))
	{
		sprintf(error, "SQLNumResultCols error %d", rc);
		ReleaseRecordset();
		return false;
	}

	for (nCurrentField = 1; nCurrentField <= nFieldCount; nCurrentField++)
	{
		SQLCHAR colName[256] = {0};
		if(SQL_SUCCEEDED(SQLDescribeCol(m_hStatment, nCurrentField, colName, sizeof(colName), 0, 0, 0, 0, 0)))
		{
			string strColName((char*)colName);
			m_ColNameMap.insert(pair<string, int>(strColName, nCurrentField));
		}
	}

	if(m_ColNameMap.size() <= 0)
	{
		sprintf(error, "m_ColNameMap.size() = 0");
		ReleaseRecordset();
		return false;
	}

	//查询首行数据
	for (nCurrentField = 1; nCurrentField <= nFieldCount; nCurrentField++)
	{
		SQLCHAR *colValue = new SQLCHAR[512];
		if(NULL == colValue)
		{
			sprintf(error, "colValue = null");
			ReleaseRecordset();
			return false;
		}

		memset(colValue, 0, 512);
		m_ValueMap.insert(pair<int, SQLCHAR*>(nCurrentField, colValue));

		int nRet = 0;
		rc = SQLBindCol(m_hStatment, nCurrentField, SQL_C_CHAR, m_ValueMap[nCurrentField], 512, (SQLINTEGER*)&nRet);
	}

	rc = SQLFetch(m_hStatment);
	if(!SQL_SUCCEEDED(rc))
	{
		sprintf(error, "SQLFetch error %d", rc);
		ReleaseRecordset();
		return false;
	}

	return true;
}

//执行SQL语句
bool CODBCDataManage::ExecuteSQL(LPCSTR lpSQL, char * error, bool bCleanStateAfterExcute/* = true*/)
{
	try
	{
		if(NULL == lpSQL)
		{
			sprintf(error, "lpSQL is NULL!");
			return false;
		}
		
		if(NULL == m_hConnection)  
		{//CONNECTION对象为空，创建连接
			if(!OnInitDBConnect(error))
				return false;
		}
		
		//先清除之前的记录集对象
		ReleaseRecordset();
		
		//创建记录集对象
		RETCODE rc = SQLAllocStmt(m_hConnection, &m_hStatment);
		if(!SQL_SUCCEEDED(rc))
		{
			sprintf(error, "SQLAllocStmt error %d", rc);
			goto _error;
		}
		
		//准备执行语句
        rc = SQLPrepare(m_hStatment, (unsigned char *)lpSQL, SQL_NTS);
		if(!SQL_SUCCEEDED(rc))
		{
			sprintf(error, "SQLPrepare error %d ", rc);
			goto _error;
		}
		
		//执行语句
        rc = SQLExecute(m_hStatment); 
		if (!SQL_SUCCEEDED(rc)) 
		{
			sprintf(error, "SQLExecute error %d ", rc);
			goto _error;
		}

		if(bCleanStateAfterExcute)
			ReleaseRecordset();
		
		return true;
		
	}
	catch(...)
	{
		ReleaseRecordset();
	}

_error:
	ReleaseRecordset();
	
	return false;
}

//根据字段得到结果集
string CODBCDataManage::GetCollect(LPCTSTR lpField)
{
	try  
    {
		if(m_ColNameMap.end() != m_ColNameMap.find(lpField))
		{
			int nCol = m_ColNameMap[lpField];
			if(m_ValueMap.end() != m_ValueMap.find(nCol))
			{
				return (char*)m_ValueMap[nCol];
			}
		}
	}
	catch(...)
	{
	}	

	return "";
}

//判断当前记录集是否已为空
bool CODBCDataManage::HasRecord()
{
	try  
    {
		return !m_ColNameMap.empty() && !m_ValueMap.empty();
	}
	catch(...)
	{
	}

	return false;
}

//记录指针下移
void CODBCDataManage::RecordMoveNext()
{
	try  
    {
		if(!SQL_SUCCEEDED(SQLFetch(m_hStatment)))
		{
			ClearValueMap();
		}
	}
	catch(...)
	{
	}
}

//记录指针上移
void CODBCDataManage::RecordMovePrevious()
{
	
}

//记录指针移到尾部
void CODBCDataManage::RecordMoveToEnd()
{
	
}
//记录指针移到开头
void CODBCDataManage::RecordMoveToFirst()
{

}

//释放CONNECTION
void CODBCDataManage::ReleaseConnection()
{
	try  
    {
		if(NULL != m_hConnection)
		{
			SQLDisconnect(m_hConnection);
			SQLFreeHandle(SQL_HANDLE_DBC, m_hConnection);
			m_hConnection = NULL;
		}

		if(NULL != m_hEnvironment)
		{
			SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvironment);
			m_hEnvironment = NULL;
		}
	}
	catch(...)
	{
		m_hConnection = NULL;
		m_hEnvironment = NULL;
	}
}

//释放RECORDSET
void CODBCDataManage::ReleaseRecordset()
{
	try  
    {
		if(NULL != m_hStatment)
		{
			SQLFreeStmt(m_hStatment, SQL_DROP);
			m_hStatment = NULL;
		}
	}
	catch(...)
	{
		m_hStatment = NULL;
	}
}

//清除valuemap
void CODBCDataManage::ClearValueMap()
{
	for (map<int, SQLCHAR*>::iterator iter = m_ValueMap.begin(); iter != m_ValueMap.end(); iter++)
	{
		if(iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_ValueMap.clear();
}

//断开与数据库的连接
void CODBCDataManage::ExitConnect()
{
	ReleaseRecordset();
	ReleaseConnection();
	ClearValueMap();
}