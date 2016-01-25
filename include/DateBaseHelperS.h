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

	//�������ݿ�
	void SetDataBase(const char* lpDataBaseName, const char* lpPassword);
	
	//��ʼ�����ݿ�
	bool OnInitDBConnect(char * error);
	
	//������¼��
	bool CreateRecordSet(const char* lpSQL);
	
	//ִ��SQL���
	bool ExecuteSQL(const char* lpSQL);
	
	//�����ֶεõ������
	bool GetCollect(const char* lpField, char* strResult, int nBufferLength);
	
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

private:
	class CDataManage* m_pDataBase;
};

#endif // !defined(AFX_DATEBASEHELPERS_H__DD661E27_AF75_48E9_8F6E_2A4DCF99B4BC__INCLUDED_)
