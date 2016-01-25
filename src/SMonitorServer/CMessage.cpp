#include "StdAfx.h"
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
		ServerLogger->AddLog("ÄÚ´æ²»×ã");
		return NULL;
	}

	memset(buffer, 0, strlen(src) + 1);
	strcpy(buffer, src);
	return buffer;
}

bool CMessage::checkError(CMarkup *xml, int &errorCode, QString& errorInfo)
{
	errorCode = 0;
	errorInfo = "";
	if(NULL == xml) return false;

	if(xml->FindChildElem("Result"))
	{
		QString strId = xml->GetAttrib("ErrorCode").c_str();
		errorCode = strId.toInt();
		errorInfo = xml->GetAttrib("Value").c_str();
		xml->OutOfElem();
	}

	return true;
}