#pragma once
#include <QByteArray>
#include <QString>
//��Ϣ����ͷ
typedef enum _MsgHead
{
	EMSG_UNKNOWN       = 0,   //δ֪��Ϣ
	EMSG_LOGININ       = 1,   //ǩ��(�ͻ��˵�½)
	EMSG_REQUESTXML    = 2,   //XML����(C->S)    
	EMSG_XMLUPDATE     = 3,   //XML����(S->C)
	EMSG_REGISTERID    = 4,   //�ŵ���ע��
	EMSG_MODIFYID      = 5,   //�ŵ����޸�
	EMSG_SOFTINSTALL   = 6,   //�����װ
	EMSG_HELPINFO	   = 7,   //�������������Ϣ
	EMSG_DISTRIBUTEMSG = 8,   //�ַ�֪ͨ��Ϣ(S->C)
	EMSG_REQUESTMSG    = 9,   //����֪ͨ��Ϣ(C->S)
	EMSG_SOFTUPLOAD    = 10,  //���������Ϣ�ϱ�
	EMSG_CROSSRUNINFO  = 11   //CROSSRUN��Ϣ�ϱ�
}MsgHead; 


//��Ϣ
typedef struct _NetMessage
{
	MsgHead           msgHead;     //��Ϣͷ
	unsigned long     msgLen;      //��Ϣ����
	char*             msgBuffer;   //��Ϣ

	_NetMessage()
	{
		msgHead = EMSG_UNKNOWN;
		msgLen = 0;
		msgBuffer = NULL;
	}
}NetMessage, *PNetMessage;

class CNetMessageWrapper
{
public:
	CNetMessageWrapper();
	~CNetMessageWrapper();

	//����������Ϣ�õ�NetMessage  netMessage�е�buffer��Ҫ�ֶ��ͷ�
	static bool GetNetMessage(const char* inputBuffer, unsigned long bufLen, NetMessage& netMessage);

	//����netMessage
	static bool GetDecodeBuffer(NetMessage& netMessage);

	//��netMessage�е�����ȡ����inputBuffer ��Ҫ�ֶ��ͷ�
	static bool GetNetMessageToBuffer(NetMessage& netMessage, char*& inputBuffer, unsigned long& bufLen);

	//4�ֽ�����תΪ�ַ���
	static QString IntToString(int nNum);

	//8�Լ�����תΪ�ַ���
	static QString ULongToString(unsigned long lNum);

	//�ַ���תΪ4�ֽ�����
	static int StringToInt(const QString& strNum);

	//�ַ���תΪ8�Լ�����
	static unsigned long StringToULong(const QString& strNum);

	//4�ֽ�����תΪQByteArray
	static QByteArray IntToQByteArray(int nNum);

	//8�Լ�����תΪQByteArray
	static QByteArray ULongToQByteArray(unsigned long lNum);

	//QByteArrayתΪ4�ֽ�����
	static int QByteArrayToInt(const QByteArray& strNum);

	//QByteArrayתΪ8�Լ�����
	static unsigned long QByteArrayToULong(const QByteArray& strNum);
};