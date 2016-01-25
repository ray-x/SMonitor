#pragma once
#include <QByteArray>
#include <QString>
//消息类型头
typedef enum _MsgHead
{
	EMSG_UNKNOWN       = 0,   //未知消息
	EMSG_LOGININ       = 1,   //签到(客户端登陆)
	EMSG_REQUESTXML    = 2,   //XML请求(C->S)    
	EMSG_XMLUPDATE     = 3,   //XML更新(S->C)
	EMSG_REGISTERID    = 4,   //门店编号注册
	EMSG_MODIFYID      = 5,   //门店编号修改
	EMSG_SOFTINSTALL   = 6,   //软件安装
	EMSG_HELPINFO	   = 7,   //请求软件帮助信息
	EMSG_DISTRIBUTEMSG = 8,   //分发通知消息(S->C)
	EMSG_REQUESTMSG    = 9,   //请求通知消息(C->S)
	EMSG_SOFTUPLOAD    = 10,  //本地软件信息上报
	EMSG_CROSSRUNINFO  = 11   //CROSSRUN信息上报
}MsgHead; 


//消息
typedef struct _NetMessage
{
	MsgHead           msgHead;     //消息头
	unsigned long     msgLen;      //消息长度
	char*             msgBuffer;   //消息

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

	//根据输入信息得到NetMessage  netMessage中的buffer需要手动释放
	static bool GetNetMessage(const char* inputBuffer, unsigned long bufLen, NetMessage& netMessage);

	//解密netMessage
	static bool GetDecodeBuffer(NetMessage& netMessage);

	//将netMessage中的数据取出到inputBuffer 需要手动释放
	static bool GetNetMessageToBuffer(NetMessage& netMessage, char*& inputBuffer, unsigned long& bufLen);

	//4字节整数转为字符串
	static QString IntToString(int nNum);

	//8自己整数转为字符串
	static QString ULongToString(unsigned long lNum);

	//字符串转为4字节整数
	static int StringToInt(const QString& strNum);

	//字符串转为8自己整数
	static unsigned long StringToULong(const QString& strNum);

	//4字节整数转为QByteArray
	static QByteArray IntToQByteArray(int nNum);

	//8自己整数转为QByteArray
	static QByteArray ULongToQByteArray(unsigned long lNum);

	//QByteArray转为4字节整数
	static int QByteArrayToInt(const QByteArray& strNum);

	//QByteArray转为8自己整数
	static unsigned long QByteArrayToULong(const QByteArray& strNum);
};