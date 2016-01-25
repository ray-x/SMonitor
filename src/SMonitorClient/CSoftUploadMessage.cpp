#include "CHomePageWidget.h"
#include "CSoftUploadMessage.h"
#include "Markup.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
#include "QFile"
#include "CMisc.h"
CSoftUploadMessage::CSoftUploadMessage(CMessageEventMediator* pMessageEventMediator, QObject *parent)
	: CMessage(pMessageEventMediator, parent)
{

}

CSoftUploadMessage::~CSoftUploadMessage()
{
}

bool CSoftUploadMessage::packedSendMessage(NetMessage& netMessage)
{
	CMarkup xml;
	bool bLocalCrossInstall = false;
	if(NULL == NetClass->m_pHomePageWidget || !NetClass->m_pHomePageWidget->getInstallSoftXml(&xml, bLocalCrossInstall))
	{
		return false;
	}

	netMessage.msgBuffer = packXml(&xml);
	netMessage.msgLen = xml.GetDoc().length();

	bool bRet = true;
	QString strMsg = netMessage.msgBuffer;
	QString path = QApplication::applicationDirPath() + "\\LastSoftMsg.txt"; //上一次上传Msg
	QFile file;
	file.setFileName(path);
	if(file.open(QIODevice::ReadWrite))
	{
		//设置文件共享打开
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::ExeOther | QFileDevice::WriteOther);
		m_strSoft = QString::fromLocal8Bit(file.readAll().constData());
		if(!MessageDataMediator->m_bCrossLinkIsInstalled && !MessageDataMediator->m_strShopId.isEmpty() && bLocalCrossInstall)
		{
			m_strSoft = strMsg;
		}
		else if(strMsg == m_strSoft)
		{
			MessageDataMediator->setConnectMsgMapProperty(EMSG_SOFTUPLOAD, true);
			bRet = false;
		}
		else
		{
			m_strSoft = strMsg;
		}
		file.close();
	}

	return bRet;
}

bool CSoftUploadMessage::treatMessage(const NetMessage& netMessage, CNetClt* pNet)
{
	MessageDataMediator->setConnectMsgMapProperty(EMSG_SOFTUPLOAD, true);
	QString path = QApplication::applicationDirPath() + "\\LastSoftMsg.txt"; //上一次上传Msg
	QFile file;
	file.setFileName(path);
	if(file.open(QIODevice::ReadWrite))
	{
		//设置文件共享打开
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::ExeOther | QFileDevice::WriteOther);
		file.resize(0);
		file.write(m_strSoft.toLocal8Bit().constData());
		file.close();
	}
	CMarkup xml;
	if(xml.SetDoc(netMessage.msgBuffer))
	{
		int errCode = 0;
		QString errorInfo = "";
		if(checkError(&xml, errCode, errorInfo))
		{
			if(0 != errCode)
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("EMSG_SOFTUPLOAD [%1] : \r\n[%2]").arg(errorInfo).arg(netMessage.msgBuffer));
				emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, errorInfo);
				return true;
			}
		}

		xml.ResetMainPos();
		if(xml.FindChildElem("Result"))
		{
			xml.IntoElem();
			QString strName = QString::fromStdString(xml.GetAttrib("CorrsLinkRunState"));
			xml.OutOfElem();

			ClientLogger->AddLog(QString::fromLocal8Bit("服务器现在记录的CrossLinkRun 安装结果为[%1]").arg(strName));
			if(strName == QString::fromLocal8Bit("是"))
			{
				MessageDataMediator->m_bCrossLinkIsInstalled = true;
				CMisc::SetCrossLinkRunInstallState(strName);
			}
		}
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("EMSG_SOFTUPLOAD收到的消息错误， 无法被正确解析 : [%1]").arg(netMessage.msgBuffer));
		emit NetClass->m_pMessageEventMediator->sigError(netMessage.msgHead, QString::fromLocal8Bit("收到的消息错误， 无法被正确解析"));
	}

	return true;
}