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

//�������ݿ�
void CODBCDataManage::SetDataBase(LPCSTR lpDataBaseName, LPCTSTR lpPassword)
{
	m_strDataBaseName = lpDataBaseName;
	m_strPassword = lpPassword;
}

//��ʼ�����ݿ�
bool CODBCDataManage::OnInitDBConnect(char * error)
{
	//�Ѿ��������ӣ�ֱ�ӷ���
	if(NULL != m_hConnection)
		return true;

	//��ʼ��OLE/COM��
	try
	{
		//��ʼ����������
		RETCODE rc = SQLAllocEnv(&m_hEnvironment);
		if(!SQL_SUCCEEDED(rc) || NULL == m_hEnvironment)
		{
			sprintf(error, "SQLAllocEnv error %d", rc);
			goto _error;
		}

		//��ʼ�����Ӿ��
		rc = SQLAllocConnect(m_hEnvironment, &m_hConnection);
		if(!SQL_SUCCEEDED(rc) || NULL == m_hConnection)
		{
			sprintf(error, "SQLAllocConnect error %d", rc);
			goto _error;
		}

		//���������ִ�
		char chConnectStr[512] = {0}; 
		sprintf(chConnectStr, "Driver={Microsoft Access Driver (*.mdb)};Dbq=%s;Pwd=%s;", m_strDataBaseName.c_str(), m_strPassword.c_str());

		//�������ݿ�
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

//������¼��
bool CODBCDataManage::CreateRecordSet(LPCSTR lpSQL, char * error)
{
	m_ColNameMap.clear();
	ClearValueMap();

	//ִ�в�ѯ���
	if(!ExecuteSQL(lpSQL, error, false))
		return false;

	//��ȡ��ѯ����ı���ͷ
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

	//��ѯ��������
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

//ִ��SQL���
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
		{//CONNECTION����Ϊ�գ���������
			if(!OnInitDBConnect(error))
				return false;
		}
		
		//�����֮ǰ�ļ�¼������
		ReleaseRecordset();
		
		//������¼������
		RETCODE rc = SQLAllocStmt(m_hConnection, &m_hStatment);
		if(!SQL_SUCCEEDED(rc))
		{
			sprintf(error, "SQLAllocStmt error %d", rc);
			goto _error;
		}
		
		//׼��ִ�����
        rc = SQLPrepare(m_hStatment, (unsigned char *)lpSQL, SQL_NTS);
		if(!SQL_SUCCEEDED(rc))
		{
			sprintf(error, "SQLPrepare error %d ", rc);
			goto _error;
		}
		
		//ִ�����
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

//�����ֶεõ������
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

//�жϵ�ǰ��¼���Ƿ���Ϊ��
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

//��¼ָ������
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

//��¼ָ������
void CODBCDataManage::RecordMovePrevious()
{
	
}

//��¼ָ���Ƶ�β��
void CODBCDataManage::RecordMoveToEnd()
{
	
}
//��¼ָ���Ƶ���ͷ
void CODBCDataManage::RecordMoveToFirst()
{

}

//�ͷ�CONNECTION
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

//�ͷ�RECORDSET
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

//���valuemap
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

//�Ͽ������ݿ������
void CODBCDataManage::ExitConnect()
{
	ReleaseRecordset();
	ReleaseConnection();
	ClearValueMap();
}