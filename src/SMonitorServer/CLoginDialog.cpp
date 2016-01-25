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
#include "CLoginDialog.h"
#include "databasemanager.h"
#include "CMisc.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QByteArray>
#include <QSettings>
#include <QDebug>

CLoginDialog::CLoginDialog(QWidget *parent)
	: QDialog(parent)
	, m_bPressed(false)
	, m_movePoint(QPoint(0, 0))
{
	setupUi(this);
	setWindowIcon(QIcon(":/Resources/normal.png"));
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	tbtn_login->setDefault(true);

	connect(tbtn_login, &QPushButton::clicked, this, &CLoginDialog::login);
	connect(tbtn_close, &QToolButton::clicked, this, &CLoginDialog::close);

	QRegExp regx("[a-zA-Z0-9]+$");
	QValidator *validator = new QRegExpValidator(regx, this);
	lne_username->setValidator(validator);
	lne_passwd->setValidator(validator);

	LoadDataBase();

	readSettings();
}

CLoginDialog::~CLoginDialog()
{

}

void CLoginDialog::LoadDataBase()
{
	//�������ݿ�
	bool isExist = false;
	QString dataPath = QApplication::applicationDirPath() + "/Database/database.db";
	
	if(QFile::exists(dataPath))
		isExist = true;

	//dataPath = "d:/BusinessLog/database.db";  //yhb
	if(!CDataManagerThread::Instance()->LoadDatabase(dataPath))
	{
		QMessageBox::information(NULL, "error", QString::fromLocal8Bit("���ݿ��ʧ��, ����������ʧ��"));
		return ;
	}

	if(!isExist)
	{
		//�������ݿ�����һ��Ĭ���û�
		addUser();
	}
}

void CLoginDialog::addUser()
{
	if(DatabaseManager::Instance()->AddUser(CMisc::Encrypt("admin"), CMisc::Encrypt("123456")))
	{
		ServerLogger->AddLog("�������ݿ�ɹ������Ĭ���û�");
	}
}

void CLoginDialog::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), QBrush(QPixmap(":/Resources/background_login.png")));
}

void CLoginDialog::mouseMoveEvent(QMouseEvent *event)
{
	if(m_bPressed)
	{
		QPoint movePos = event->globalPos() - m_movePoint;
		int distance = movePos.manhattanLength();
		bool bMoved =  distance > QApplication::startDragDistance() ? true : false;
		if(bMoved)
		{
			this->move(movePos);
		}
	}
}

void CLoginDialog::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(event->pos().y() < 48)
			m_bPressed = true;
	}
	//�����ƶ�����
	m_movePoint = event->globalPos() - frameGeometry().topLeft();
}

void CLoginDialog::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_bPressed = false;
	}
}

void CLoginDialog::login()
{
	if(lne_passwd->text().isEmpty() || lne_username->text().isEmpty())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("�û��������벻��Ϊ�գ������ԣ�"), 
			QString::fromLocal8Bit("ȷ��"));
	}
	else
	{
		QString userName = lne_username->text().trimmed();
		QString passwd = lne_passwd->text().trimmed();

		if(DatabaseManager::Instance()->IsUserExist(CMisc::Encrypt(userName), CMisc::Encrypt(passwd)))
		{
			saveSettings(CMisc::Encrypt(userName));
			accept();
		}
		else
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("����"),
				QString::fromLocal8Bit("�û��������������ȷ�Ϻ����ԣ�"), 
				QString::fromLocal8Bit("ȷ��"));
		}
	}
}

void CLoginDialog::saveSettings(const QString& userName)
{
	QString path = QApplication::applicationDirPath() + "/login.ini";
	QSettings settings(path, QSettings::IniFormat);
	settings.setValue("LastLogin/UserName", userName);
}

void CLoginDialog::readSettings()
{
	QString path = QApplication::applicationDirPath() + "/login.ini";
	QSettings settings(path, QSettings::IniFormat);
	lne_username->setText(CMisc::Decrypt(settings.value("LastLogin/UserName").toString()));
}