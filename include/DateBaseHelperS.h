// DateBaseHelperS.h: interface for the DateBaseHelperS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATEBASEHELPERS_H__DD661E27_AF75_48E9_8F6E_2A4DCF99B4BC__INCLUDED_)
#define AFX_DATEBASEHELPERS_H__DD661E27_AF75_48E9_8F6E_2A4DCF99B4BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern "C" class DateBaseHelperS  
{
public:
	DateBaseHelperS();
	DateBaseHelperS(const char* lpDataBaseName, const char* lpPassword);
	virtual ~DateBaseHelperS();

	//设置数据库
	void SetDataBase(const char* lpDataBaseName, const char* lpPassword);
	
	//初始化数据库
	bool OnInitDBConnect(char * error);
	
	//创建记录集
	bool CreateRecordSet(const char* lpSQL);
	
	//执行SQL语句
	bool ExecuteSQL(const char* lpSQL);
	
	//根据字段得到结果集
	bool GetCollect(const char* lpField, char* strResult, int nBufferLength);
	
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

private:
	class CDataManage* m_pDataBase;
};

#endif // !defined(AFX_DATEBASEHELPERS_H__DD661E27_AF75_48E9_8F6E_2A4DCF99B4BC__INCLUDED_)
