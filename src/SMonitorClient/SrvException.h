#pragma once

#pragma warning( disable : 4290 )  //vc++����������֧���쳣�淶�����Ժ��Դ˾���
#include <iostream>
#include <sstream>
using namespace std;

class CSrvException
{
public:
	CSrvException(void);
	CSrvException(const QString& expDescription, int expCode = 0);
	~CSrvException(void);

	//��ȡ�쳣��Ϣ
	QString GetExpDescription();

	//��ȡ������
	int GetExpCode();

	//��ȡ��������
	long GetExpLine();

private:
	QString m_strExpDescription;
	int     m_iExpCode;            //�쳣����
	long    m_iExpLineNumber;      //�쳣Դ������������Ե��԰汾��Ч
};

