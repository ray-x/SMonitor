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
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("密码不能为空"),
			QString::fromLocal8Bit("确定"));
		
		return ;
	}
	if(newPasswd != affirmPasswd)
	{
		clearInput();
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("新密码和确认密码输入不一致,请重新输入!"),
			QString::fromLocal8Bit("确定"));
		
		return;
	}

	if(newPasswd == originalPasswd)
	{
		clearInput();
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("新密码不能与原始密码相同"),
			QString::fromLocal8Bit("确定"));
		
		return ;
	}

	QString path = QApplication::applicationDirPath() + "/login.ini";
	QSettings settings(path, QSettings::IniFormat);
	QString userName = settings.value("LastLogin/UserName").toString();
	if(!DatabaseManager::Instance()->IsUserExist(userName, CMisc::Encrypt(lne_originalPasswd->text().trimmed())))
	{
		clearInput();
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("原始密码不一致，请确认后重新输入"),
			QString::fromLocal8Bit("确定"));
		
		return;
	}
	else
	{
		if(DatabaseManager::Instance()->ModifyPassword(userName, CMisc::Encrypt(newPasswd)))
		{
			clearInput();
			QMessageBox::information(this, QString::fromLocal8Bit("成功"),
				QString::fromLocal8Bit("修改密码成功"),
				QString::fromLocal8Bit("确定"));
			
			groupBox_changepasswd->setChecked(false);
			return ;
		}
		else
		{
			clearInput();
			QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("修改密码失败，请稍后重试！"),
				QString::fromLocal8Bit("确定"));
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
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
			QString::fromLocal8Bit("推送消息不能为空！"),
			QString::fromLocal8Bit("确定"));
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
			QMessageBox::warning(this, QString::fromLocal8Bit("成功"),
				QString::fromLocal8Bit("消息发送成功！"),
				QString::fromLocal8Bit("确定"));
		}
		else
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
				QString::fromLocal8Bit("发送失败，请稍后重试！"),
				QString::fromLocal8Bit("确定"));
		}
	}
	else
	{
		StorageLastMessage(QDateTime::currentDateTime(), text);
		textEdit->clear();

		QMessageBox::warning(this, QString::fromLocal8Bit("成功"),
			QString::fromLocal8Bit("消息发送成功！"),
			QString::fromLocal8Bit("确定"));
	}
}

void CStatusWidget::StorageLastMessage(const QDateTime& time, const QString& msg)
{
	MessageDataMediator->m_MessageMap.insert(time, msg);
	if(MessageDataMediator->m_MessageMap.count() > MAXMESGCOUNT)
	{
		//超过消息最大条数，则删除最后一条消息
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
	ServerLogger->AddLog("保存服务器推送消息");
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
			ServerLogger->AddLog("服务器读取推送消息完毕");
		}
	}
	else
	{
		ServerLogger->AddLog("服务器不存在推送消息，可能是存储的文件不存在");
	}
}