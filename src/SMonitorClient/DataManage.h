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

	//�������ݿ�
	void SetDataBase(LPCSTR lpDataBaseName, LPCTSTR lpPassword);

	//��ʼ�����ݿ�
	bool OnInitDBConnect(char * error);	

	//������¼��
	bool CreateRecordSet(LPCTSTR lpSQL, char * error);

	//ִ��SQL���
	bool ExecuteSQL(LPCTSTR lpSQL, char * error, bool bCleanStateAfterExcute = true);

	//�����ֶεõ������
	string GetCollect(LPCTSTR lpField);
	
	//�жϵ�ǰ��¼���Ƿ���Ϊ��
	bool HasRecord();
	
	//��¼ָ������
	void RecordMoveNext();

	//��¼ָ������
	void RecordMovePrevious();

	//��¼ָ���Ƶ�β��
	void RecordMoveToEnd();

	//��¼ָ���Ƶ���ͷ
	void RecordMoveToFirst();

	//�Ͽ������ݿ������
	void ExitConnect();
public:

	HENV                m_hEnvironment;     //�������
    HDBC                m_hConnection;      //���Ӿ��
	HSTMT               m_hStatment;        //���ִ�о��
	string              m_strDataBaseName;  //���ݿ�·��
	string              m_strPassword;      //����
	map<string, int>    m_ColNameMap;       //����ͷmap
	map<int, SQLCHAR*>  m_ValueMap;         //ֵmap
	//�ͷ�CONNECTION
	void ReleaseConnection();

	//�Ƿ�RECORDSET
	void ReleaseRecordset();

	//���valuemap
	void ClearValueMap();
};