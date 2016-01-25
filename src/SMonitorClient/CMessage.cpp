#include "CMessage.h"
#include "Markup.h"
CMessage::CMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: QObject(parent)
{
	m_pMessageEventMediator = pMessageEventMediator;
}

CMessage::~CMessage()
{

}

char* CMessage::packXml(CMarkup *xml)
{
	if(NULL == xml)
		return NULL;

	const char* src = xml->GetDoc().c_str();
	char* buffer = new char[strlen(src) + 1];
	if(NULL == buffer)
	{
		ClientLogger->AddLog("内存不足");
		return NULL;
	}

	memset(buffer, 0, strlen(src) + 1);
	strcpy(buffer, src);
	return buffer;
}

bool CMessage::checkError(CMarkup *xml, int &errorCode, QString& errorInfo)
{
	errorCode = -1;
	errorInfo = "";
	if(NULL == xml) return false;

	if(xml->FindChildElem("Result"))
	{
		xml->IntoElem();
		QString strId = xml->GetAttrib("ErrorCode").c_str();
		errorCode = strId.toInt();
		errorInfo = QString::fromStdString(xml->GetAttrib("Value"));
		xml->OutOfElem();
	}
	else
		errorInfo = QString::fromLocal8Bit("收到的消息错误， 无法被正确解析");

	return true;
}