/*
This file is part of SMonitor.
Copyright  2015~2016  by: rayx  email rayx.cn@gmail.com
SMonitor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SMonitor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "CStatusWidget.h"
#include "CMisc.h"
#include "databasemanager.h"
#include "IOCPNet.h"
#include "NetClass.h"
#include "CMessageDataMediator.h"
#include "Markup.h"

#include <QSettings>
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <QFile>

CStatusWidget::CStatusWidget(QWidget *parent)
	: QWidget(parent)
	, m_nMsgMaxLength(128)
{
	setupUi(this);
	QString MAC, IP;
	CMisc::GetLocalHostAddr(IP, MAC);
	label_serverIP->setText(IP);
	groupBox_changepasswd->setChecked(false);

	QRegExp regx("[a-zA-Z0-9]+$");
	QValidator *validator = new QRegExpValidator(regx, this);
	lne_affirmPasswd->setValidator(validator); 
	lne_newPasswd->setValidator(validator);
	lne_originalPasswd->setValidator(validator);

	LoadLastMessage();

	connect(btn_Ok, &QPushButton::clicked, this, &CStatusWidget::changePasswd);
	connect(textEdit, &QTextEdit::textChanged, this, &CStatusWidget::checkLength);
	connect(btn_Send, &QPushButton::clicked, this, &CStatusWidget::distributeMessage);
}

CStatusWidget::~CStatusWidget()
{

}

void CStatusWidget::changePasswd()
{
	QString originalPasswd = lne_originalPasswd->text().trimmed();
	QString newPasswd = lne_newPasswd->text().trimmed();
	QString affirmPasswd = lne_affirmPasswd->text().trimmed();

	if(originalPasswd.isEmpty() || newPasswd.isEmpty() || affirmPasswd.isEmpty())
	{
		clearInput();
		QMessageBox::critical(this, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("���벻��Ϊ��"),
			QString::fromLocal8Bit("ȷ��"));
		
		return ;
	}
	if(newPasswd != affirmPasswd)
	{
		clearInput();
		QMessageBox::critical(this, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("�������ȷ���������벻һ��,����������!"),
			QString::fromLocal8Bit("ȷ��"));
		
		return;
	}

	if(newPasswd == originalPasswd)
	{
		clearInput();
		QMessageBox::critical(this, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("�����벻����ԭʼ������ͬ"),
			QString::fromLocal8Bit("ȷ��"));
		
		return ;
	}

	QString path = QApplication::applicationDirPath() + "/login.ini";
	QSettings settings(path, QSettings::IniFormat);
	QString userName = settings.value("LastLogin/UserName").toString();
	if(!DatabaseManager::Instance()->IsUserExist(userName, CMisc::Encrypt(lne_originalPasswd->text().trimmed())))
	{
		clearInput();
		QMessageBox::critical(this, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("ԭʼ���벻һ�£���ȷ�Ϻ���������"),
			QString::fromLocal8Bit("ȷ��"));
		
		return;
	}
	else
	{
		if(DatabaseManager::Instance()->ModifyPassword(userName, CMisc::Encrypt(newPasswd)))
		{
			clearInput();
			QMessageBox::information(this, QString::fromLocal8Bit("�ɹ�"),
				QString::fromLocal8Bit("�޸�����ɹ�"),
				QString::fromLocal8Bit("ȷ��"));
			
			groupBox_changepasswd->setChecked(false);
			return ;
		}
		else
		{
			clearInput();
			QMessageBox::critical(this, QString::fromLocal8Bit("����"),
				QString::fromLocal8Bit("�޸�����ʧ�ܣ����Ժ����ԣ�"),
				QString::fromLocal8Bit("ȷ��"));
			return;
		}
	}
}

void CStatusWidget::clearInput()
{
	lne_originalPasswd->clear();
	lne_newPasswd->clear();
	lne_affirmPasswd->clear();
}

void CStatusWidget::checkLength()
{
	int length = textEdit->toPlainText().length();

	if(m_nMsgMaxLength >= length)
		return;

	QString content = textEdit->toPlainText();
	int position = textEdit->textCursor().position();
	QTextCursor textCursor = textEdit->textCursor();
	content.remove(position - (length - m_nMsgMaxLength), length - m_nMsgMaxLength);
	textEdit->setText(content);
	textCursor.setPosition(position - (length - m_nMsgMaxLength));
	textEdit->setTextCursor(textCursor);
}

void CStatusWidget::distributeMessage()
{
	QString text = textEdit->toPlainText();
	if(text.isEmpty()){
		QMessageBox::warning(this, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("������Ϣ����Ϊ�գ�"),
			QString::fromLocal8Bit("ȷ��"));
		return;
	}

	//QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss zzz");
	MessageDataMediator->m_strDistributeMsg = text;
	MessageDataMediator->m_strDistributeTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss zzz");

	if(MessageDataMediator->m_bTextMode)
	{
		if(NetClass->m_pNetClt->SendMsg(EMSG_DISTRIBUTEMSG, true))
		{
			StorageLastMessage(QDateTime::currentDateTime(), text);
			textEdit->clear();
			QMessageBox::warning(this, QString::fromLocal8Bit("�ɹ�"),
				QString::fromLocal8Bit("��Ϣ���ͳɹ���"),
				QString::fromLocal8Bit("ȷ��"));
		}
		else
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("����"),
				QString::fromLocal8Bit("����ʧ�ܣ����Ժ����ԣ�"),
				QString::fromLocal8Bit("ȷ��"));
		}
	}
	else
	{
		StorageLastMessage(QDateTime::currentDateTime(), text);
		textEdit->clear();

		QMessageBox::warning(this, QString::fromLocal8Bit("�ɹ�"),
			QString::fromLocal8Bit("��Ϣ���ͳɹ���"),
			QString::fromLocal8Bit("ȷ��"));
	}
}

void CStatusWidget::StorageLastMessage(const QDateTime& time, const QString& msg)
{
	MessageDataMediator->m_MessageMap.insert(time, msg);
	if(MessageDataMediator->m_MessageMap.count() > MAXMESGCOUNT)
	{
		//������Ϣ�����������ɾ�����һ����Ϣ
		QDateTime firstKey = MessageDataMediator->m_MessageMap.firstKey();
		MessageDataMediator->m_MessageMap.remove(firstKey);
	}

	QString path = QApplication::applicationDirPath() + "/Message.list";
	QFile::remove(path);
	CMarkup xml;	
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Root");
	xml.AddChildElem("MessageList");
	xml.IntoElem();

	QMapIterator<QDateTime, QString> iter(MessageDataMediator->m_MessageMap);
	while(iter.hasNext())
	{
		iter.next();
		xml.AddChildElem("Message");
		xml.IntoElem();
		xml.AddAttrib("time", iter.key().toString("yyyy-MM-dd hh:mm:ss zzz").toStdString());
		xml.AddAttrib("content", iter.value().toStdString());
		xml.OutOfElem();
	}

	xml.OutOfElem();
	xml.Save(path.toLocal8Bit().constData());

	::SetFileAttributes(path.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
	ServerLogger->AddLog("���������������Ϣ");
}

void CStatusWidget::LoadLastMessage()
{
	MessageDataMediator->m_MessageMap.clear();
	QString path = QApplication::applicationDirPath() + "/Message.list";
	CMarkup xml;
	if(xml.Load(path.toLocal8Bit().constData()))
	{
		if(xml.FindChildElem("MessageList"))
		{
			xml.IntoElem();
			while(xml.FindChildElem("Message"))
			{
				xml.IntoElem();
				//MessageDataMediator->m_strDistributeMsg = QString::fromStdString(xml.GetAttrib("content"));
				//MessageDataMediator->m_strDistributeTime = QString::fromStdString(xml.GetAttrib("time"));
				MessageDataMediator->m_MessageMap.insert(QDateTime::fromString(QString::fromStdString(xml.GetAttrib("time")), "yyyy-MM-dd hh:mm:ss zzz"),
					QString::fromStdString(xml.GetAttrib("content")));
				xml.OutOfElem();
			}
			xml.OutOfElem();
			ServerLogger->AddLog("��������ȡ������Ϣ���");
		}
	}
	else
	{
		ServerLogger->AddLog("������������������Ϣ�������Ǵ洢���ļ�������");
	}
}