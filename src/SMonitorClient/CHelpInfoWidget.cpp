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


#include "CHelpInfoWidget.h"
#include "CListWidget.h"
#include "NetClt.h"
#include "NetClass.h"
#include "CMessageDataMediator.h"
#include "Markup.h"
#include "CMisc.h"
#include "CClientLog.h"
#include <QMessageBox>
#include <QApplication>
#include <QSettings>

#include <QFile>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>
#include <QTextStream>

#include "common.h"

CHelpInfoWidget::CHelpInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);

	label_CurNum->setText(getId());

	//connect(NetClass->m_pNetClt, &CNetClt::sigSendResult, this, &CHelpInfoWidget::SendResult);
	connect(NetClass->m_pMessageEventMediator, &CMessageEventMediator::sigError, 
		this, &CHelpInfoWidget::serverReqError);
	connect(NetClass->m_pMessageEventMediator, &CMessageEventMediator::sigModifySuccess,
		this, &CHelpInfoWidget::modifySuccess);

	connect(btn_SetNum, &QPushButton::clicked, this, &CHelpInfoWidget::modifyId);

	//getHelpInfo();

	QRegExp regx("[0-9]+$");
	QValidator *validator = new QRegExpValidator(regx, this);
	lineEdit_Num->setValidator(validator);

	label_Version->setText(QString::fromLocal8Bit("客户端版本：%1").arg(MessageDataMediator->m_strClientVersion));

	LoadHelpInfo();
}

CHelpInfoWidget::~CHelpInfoWidget()
{

}

void CHelpInfoWidget::modifyId()
{
	QString text = lineEdit_Num->text().trimmed();
	if(text.isEmpty())
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("门店编号不能为空"),
			QString::fromLocal8Bit("确定"));
		return ;
	}
	else if(text.length() < 11)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("错误"), 
			QString::fromLocal8Bit("电话号码长度为11位，请确认后重试"), 
			QString::fromLocal8Bit("确定"));

		return;
	}

	MessageDataMediator->m_strNewId = text;
	MessageDataMediator->m_strOldId = getId();
	if(NetClass->m_pNetClt->SendMsg(EMSG_MODIFYID, true))
	{
		qDebug() << QString::fromLocal8Bit("发送消息成功");
	}
	else
	{
		QMessageBox::critical(this, 
			QString::fromLocal8Bit("错误"), 
			QString::fromLocal8Bit("服务器链接异常,发送失败"),
			QString::fromLocal8Bit("确定"));
	}
}

void CHelpInfoWidget::SendResult(int nMsgHead, bool bResult, const QString& strInfo)
{
	switch(nMsgHead)
	{
	case EMSG_MODIFYID:
		{
			if(bResult)
			{
				QMessageBox::information(this, 
					QString::fromLocal8Bit("信息"), 
					QString::fromLocal8Bit("修改门店编号成功"),
					QString::fromLocal8Bit("确定"));

				QString path = QApplication::applicationDirPath() + "/configure.ini";
				QSettings settings(path, QSettings::IniFormat);
				settings.setIniCodec("UTF-8");
				settings.setValue("Client/ID", MessageDataMediator->m_strNewId);

				lineEdit_Num->clear();
				label_CurNum->setText(MessageDataMediator->m_strNewId);
			}
		}
		break;
	case EMSG_HELPINFO:
		{
			qDebug() << QString::fromLocal8Bit("获取帮助信息成功:%1").arg(strInfo);
		}
		break;
	default:
		break;
	}
}

void CHelpInfoWidget::modifySuccess()
{
	QMessageBox::information(this, 
		QString::fromLocal8Bit("信息"), 
		QString::fromLocal8Bit("修改门店编号成功"),
		QString::fromLocal8Bit("确定"));

	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec("UTF-8");
	settings.setValue("RegisterID/ID", MessageDataMediator->m_strNewId);

	lineEdit_Num->clear();
	label_CurNum->setText(MessageDataMediator->m_strNewId);
}


void CHelpInfoWidget::serverReqError(int nMsgHead, const QString& strError)
{
	switch(nMsgHead)
	{
	case EMSG_MODIFYID:
		{
			QMessageBox::critical(this, 
				QString::fromLocal8Bit("错误"), 
				QString::fromLocal8Bit("修改门店编号错误: %1").arg(strError),
				QString::fromLocal8Bit("确定"));
		}
		break;
	case EMSG_HELPINFO:
		{
			QMessageBox::critical(this, 
				QString::fromLocal8Bit("错误"), 
				QString::fromLocal8Bit("获取帮助信息错误: %1").arg(strError),
				QString::fromLocal8Bit("确定"));
		}
		break;
	default:
		break;
	}

}

QString CHelpInfoWidget::getId()
{
	/*QString path = QApplication::applicationDirPath() + "/configure.ini";*/
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec("UTF-8");
	return settings.value("RegisterID/ID", QString("")).toString();
}

void CHelpInfoWidget::resetInformation(const StringMap& data)
{
	StorageHelpInfo(data);
	updateInfomation(data);
}

void CHelpInfoWidget::updateHelpInfo()
{
	QString path = QApplication::applicationDirPath() + "/Help.data";

	CMarkup xml;
	if(xml.Load(path.toLocal8Bit().constData()))
	{
		if(xml.FindChildElem("Info"))
		{
			xml.IntoElem();

			QMap<QString, QString> infoMap;
			while(xml.FindChildElem("HelpInfo"))
			{
				xml.IntoElem();
				infoMap.insert(QString::fromStdString(xml.GetAttrib("Label")), 
					QString::fromStdString(xml.GetAttrib("Value")));
				xml.OutOfElem();
			}
			xml.OutOfElem();
			resetInformation(infoMap);
		}
		else
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("解析收到的帮助信息失败"));
		}
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("解析收到的帮助信息失败"));
	}
}

void CHelpInfoWidget::updateInfomation(const StringMap& data)
{
	QList<QLabel*> items = scrollArea->findChildren<QLabel*>(QString());
	foreach(QLabel*item, items)
		delete item;

	int index = 0;
	QFont font = QFont("Microsoft yahei UI", 9);
	QMapIterator<QString, QString> iter(data);
	while(iter.hasNext())
	{
		iter.next();
		QLabel* label = new QLabel(iter.key() + QString::fromLocal8Bit("："), this);
		label->setFixedHeight(30);
		label->setFont(font);
		label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QString strContent = iter.value();
		CLabel* content = new CLabel(this);
		content->setOpenExternalLinks(true);
		QRegExp regExp("((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:www\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\??(?:[\\-\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)");
		strContent.replace(regExp, "<a href=\"\\1\">\\1</a>");
		content->setText(strContent);
		content->setFixedHeight(30);
		content->setFont(font);
		content->setOpenExternalLinks(true);
		content->setTextInteractionFlags(Qt::TextBrowserInteraction);
		content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		gridLayout->addWidget(label, index, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
		gridLayout->addWidget(content, index, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);
		gridLayout->setHorizontalSpacing(40);
		index++;
	}
	if(index > 0){
		gridLayout->setRowStretch(index - 1, 1);
	}
}

void CHelpInfoWidget::GetHelpInfo()
{
	//不再需要HELPINFOMSG
	if(NetClass->m_pNetClt->SendMsg(EMSG_HELPINFO, true))
	{
		qDebug() << QString::fromLocal8Bit("发送消息成功");
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("链接服务器失败,发送失败"));
		LoadHelpInfo();
	}
}

void CHelpInfoWidget::StorageHelpInfo(const StringMap& data)
{
	QString path = QApplication::applicationDirPath() + "/help.list";
	QFile::remove(path);

	CMarkup xml;	
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Root");
	xml.AddChildElem("HelpInfoList");
	xml.IntoElem();
	
	QMapIterator<QString, QString> iter(data);
	while(iter.hasNext())
	{
		iter.next();
		xml.AddChildElem("HelpInfo");
		xml.IntoElem();
		xml.AddAttrib("label", iter.key().toStdString());
		xml.AddAttrib("content", iter.value().toStdString());
		xml.OutOfElem();
	}

	xml.OutOfElem();
	xml.Save(path.toLocal8Bit().constData());
	//QString str = QString::fromStdString(xml.GetDoc());
	//SetEncryHelpInfo(QString::fromStdString(xml.GetDoc()));

	::SetFileAttributes(path.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
	
}

void CHelpInfoWidget::LoadHelpInfo()
{
	ClientLogger->AddLog(QString::fromLocal8Bit("加载本地保存帮助信息"));
	QString path = QApplication::applicationDirPath() + "/help.list";
	StringMap infoMap;
	CMarkup xml;	

	if(xml.Load(path.toLocal8Bit().constData()))
	{
		if(xml.FindChildElem("HelpInfoList"))
		{
			xml.IntoElem();
			while(xml.FindChildElem("HelpInfo"))
			{
				xml.IntoElem();
				infoMap.insert(QString::fromStdString(xml.GetAttrib("label")),
					QString::fromStdString(xml.GetAttrib("content")));
				xml.OutOfElem();
			}
			xml.OutOfElem();
		}
	}
	else
		ClientLogger->AddLog(QString::fromLocal8Bit("加载帮助信息失败，可能是帮助信息文件不存在"));

	qDebug() << infoMap;
	updateInfomation(infoMap);
}

QString CHelpInfoWidget::GetDecryptHelpInfo()
{
	QString path = QApplication::applicationDirPath() + "/help.list";
	QFile file(path);
	if(file.open(QIODevice::ReadOnly))
	{
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
		return QString::fromLocal8Bit("%1").arg(CMisc::Decrypt(file.readAll())) ;
	}

	return QString();
}

void CHelpInfoWidget::SetEncryHelpInfo(const QString& data)
{
	QString path = QApplication::applicationDirPath() + "/help.list";
	QFile::remove(path);
	QFile file(path);
	if(file.open(QIODevice::WriteOnly  | QFile::Truncate))
	{
		QTextStream out(&file);
		out << CMisc::Encrypt(data.toLocal8Bit().constData());
	}
}
