#include "stdafx.h"
#include "CNetMessage.h"
#include <QByteArray>
CNetMessageWrapper::CNetMessageWrapper()
{

}

CNetMessageWrapper::~CNetMessageWrapper()
{

}

//根据输入信息得到NetMessage bDecode - true 解密msgBuffer信息 netMessage中的buffer需要手动释放
bool CNetMessageWrapper::GetNetMessage(const char* inputBuffer, unsigned long bufLen, NetMessage& netMessage)
{
	char* pBuffer = new char[bufLen + 1];
	if(NULL == pBuffer)
		return false;

	memset(pBuffer, 0, bufLen + 1);
	memcpy(pBuffer, inputBuffer, bufLen);

	QByteArray result;
	result.append(pBuffer);
	
	//执行ZIP解压
	result = qUncompress(QByteArray::fromBase64(result));

	delete pBuffer;
	pBuffer = NULL;

	if(result.size() < 12) // 长度必须大于16个字节
		return false;

	//消息头
	QByteArray temp = result.left(4);
	netMessage.msgHead = (MsgHead)QByteArrayToInt(temp);

	//消息长度
	result = result.mid(4);
	temp = result.left(8);
	netMessage.msgLen = QByteArrayToULong(temp);

	//消息
	netMessage.msgBuffer = new char[netMessage.msgLen + 1];
	memset(netMessage.msgBuffer, 0, netMessage.msgLen + 1);
	result = result.mid(8);

	memcpy(netMessage.msgBuffer, result.data(), netMessage.msgLen);
	return true;
}

//解密netMessage
bool CNetMessageWrapper::GetDecodeBuffer(NetMessage& netMessage)
{

	return true;
}

//将netMessage中的数据取出到inputBuffer 需要手动释放
bool CNetMessageWrapper::GetNetMessageToBuffer(NetMessage& netMessage, char*& inputBuffer, unsigned long& bufLen)
{
	QString strEncryBuffer = "";

	//拼装数据
	QByteArray byteArray;
	byteArray.append(IntToQByteArray(netMessage.msgHead));
	byteArray.append(ULongToQByteArray(netMessage.msgLen));
	byteArray.append(netMessage.msgBuffer);

	//执行ZIP 压缩后，转成BASE64编码
	QByteArray result = qCompress(byteArray);
	strEncryBuffer = result.toBase64();

	//QString strBuffer = QString::fromLocal8Bit("Begin##%1##End").arg(strEncryBuffer);

	bufLen = strlen(strEncryBuffer.toStdString().c_str());
	inputBuffer = new char[bufLen + 1];
	memset(inputBuffer, 0, bufLen + 1);
	memcpy(inputBuffer, strEncryBuffer.toStdString().c_str(), bufLen);

	return true;
}

//4字节整数转为字符串
QString CNetMessageWrapper::IntToString(int nNum)
{
	return IntToQByteArray(nNum).toBase64(); 
}

//8自己整数转为字符串
QString CNetMessageWrapper::ULongToString(unsigned long lNum)
{
	return ULongToQByteArray(lNum).toBase64(); 
}

//字符串转为4字节整数
int CNetMessageWrapper::StringToInt(const QString& strNum)
{
	QByteArray temp;
	temp.append(strNum);
	return QByteArrayToInt(temp);    
}

//字符串转为8自己整数
unsigned long CNetMessageWrapper::StringToULong(const QString& strNum)
{
	QByteArray temp;
	temp.append(strNum);
	return QByteArrayToULong(temp);    
}

//4字节整数转为QByteArray
QByteArray CNetMessageWrapper::IntToQByteArray(int nNum)
{
	QByteArray abyte0;    
	abyte0.resize(4);    
	abyte0[0] = (uchar) (0x000000ff & nNum);    
	abyte0[1] = (uchar) ((0x0000ff00 & nNum) >> 8);    
	abyte0[2] = (uchar) ((0x00ff0000 & nNum) >> 16);    
	abyte0[3] = (uchar) ((0xff000000 & nNum) >> 24);   
	return abyte0; 
}

//8自己整数转为QByteArray
QByteArray CNetMessageWrapper::ULongToQByteArray(unsigned long lNum)
{
	QByteArray abyte0;    
	abyte0.resize(8);    
	abyte0[0] = (uchar) (0x00000000000000ff & lNum);    
	abyte0[1] = (uchar) ((0x000000000000ff00 & lNum) >> 8);    
	abyte0[2] = (uchar) ((0x0000000000ff0000 & lNum) >> 16);    
	abyte0[3] = (uchar) ((0x00000000ff000000 & lNum) >> 24);
	abyte0[4] = (uchar) ((0x000000ff00000000 & lNum) >> 32);
	abyte0[5] = (uchar) ((0x0000ff0000000000 & lNum) >> 40);
	abyte0[6] = (uchar) ((0x00ff000000000000 & lNum) >> 48);
	abyte0[7] = (uchar) ((0xff00000000000000 & lNum) >> 56);
	return abyte0;
}

//QByteArray转为4字节整数
int CNetMessageWrapper::QByteArrayToInt(const QByteArray& strNum)
{
	int addr = strNum[0] & 0x000000FF;    
	addr |= ((strNum[1] << 8) & 0x0000FF00);    
	addr |= ((strNum[2] << 16) & 0x00FF0000);    
	addr |= ((strNum[3] << 24) & 0xFF000000);    
	return addr;
}

//QByteArray转为8自己整数
unsigned long CNetMessageWrapper::QByteArrayToULong(const QByteArray& strNum)
{
	unsigned long addr = strNum[0] & 0x00000000000000ff;    
	addr |= ((strNum[1] << 8) & 0x000000000000FF00);    
	addr |= ((strNum[2] << 16) & 0x0000000000FF0000);    
	addr |= ((strNum[3] << 24) & 0x00000000FF000000);    
	addr |= ((strNum[4] << 32) & 0x000000FF00000000);    
	addr |= ((strNum[5] << 40) & 0x0000FF0000000000);    
	addr |= ((strNum[6] << 48) & 0x00FF000000000000);    
	addr |= ((strNum[7] << 56) & 0xFF00000000000000);  
	return addr;
}