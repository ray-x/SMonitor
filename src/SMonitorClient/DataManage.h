// DataManage.h: interface for the CODBCDataManage class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable:4786)
#pragma comment(lib, "odbc32.lib") 
#include <Windows.h>
#include <sqlext.h>
#include <string>
#include <map>
#include <stdio.h>
using namespace std;

class CODBCDataManage  
{
public:
	CODBCDataManage(LPCSTR lpDataBaseName, LPCTSTR lpPassword);
	CODBCDataManage();
	virtual ~CODBCDataManage();

	//设置数据库
	void SetDataBase(LPCSTR lpDataBaseName, LPCTSTR lpPassword);

	//初始化数据库
	bool OnInitDBConnect(char * error);	

	//创建记录集
	bool CreateRecordSet(LPCTSTR lpSQL, char * error);

	//执行SQL语句
	bool ExecuteSQL(LPCTSTR lpSQL, char * error, bool bCleanStateAfterExcute = true);

	//根据字段得到结果集
	string GetCollect(LPCTSTR lpField);
	
	//判断当前记录集是否已为空
	bool HasRecord();
	
	//记录指针下移
	void RecordMoveNext();

	//记录指针上移
	void RecordMovePrevious();

	//记录指针移到尾部
	void RecordMoveToEnd();

	//记录指针移到开头
	void RecordMoveToFirst();

	//断开与数据库的连接
	void ExitConnect();
public:

	HENV                m_hEnvironment;     //环境句柄
    HDBC                m_hConnection;      //连接句柄
	HSTMT               m_hStatment;        //语句执行句柄
	string              m_strDataBaseName;  //数据库路径
	string              m_strPassword;      //密码
	map<string, int>    m_ColNameMap;       //标题头map
	map<int, SQLCHAR*>  m_ValueMap;         //值map
	//释放CONNECTION
	void ReleaseConnection();

	//是否RECORDSET
	void ReleaseRecordset();

	//清除valuemap
	void ClearValueMap();
};