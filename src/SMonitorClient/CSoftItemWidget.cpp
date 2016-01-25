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

#include "CSoftItemWidget.h"
#include "CSoftManager.h"
#include "CMisc.h"
#include "Markup.h"
#include "CCategoryFactory.h"
#include "CMessageDataMediator.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QProcess>
#include <QDesktopServices>
#include <QDebug>
#include <QFileInfo>
#include <Windows.h>

CSoftItemWidget::CSoftItemWidget(const SoftData& data, QWidget *parent)
	: QFrame(parent)
	, m_tbtnAPP(nullptr)
	, m_tbtnOpr(nullptr)
{
	setObjectName("AppSet");
	setFixedHeight(105);

	m_tbtnAPP = new QToolButton(this);
	m_tbtnAPP->setCursor(QCursor(Qt::PointingHandCursor));
	m_tbtnAPP->setObjectName("tbtn_App");
	m_tbtnAPP->setIcon(QIcon(":/Resources/normal.png"));
	m_tbtnAPP->setFixedSize(QSize(100, 70));
	m_tbtnAPP->setIconSize(QSize(40, 40));
	m_tbtnAPP->setAutoRaise(true);
	m_tbtnAPP->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_tbtnAPP->setToolTip(gernateToolTips(data));

	m_tbtnOpr = new QToolButton(this);
	m_tbtnOpr->setMinimumSize(QSize(50, 25));
	m_tbtnOpr->setObjectName("tbtn_Opr");

	updateState(data);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(6);
	mainLayout->setContentsMargins(5, 5, 5, 5);
	mainLayout->addWidget(m_tbtnAPP, 0, Qt::AlignHCenter);
	mainLayout->addWidget(m_tbtnOpr, 1, Qt::AlignRight);

	connect(m_tbtnOpr, &QToolButton::clicked, this, &CSoftItemWidget::setOperation);
	connect(m_tbtnAPP, &QToolButton::clicked, this, &CSoftItemWidget::OpenExternalProgram);
}

CSoftItemWidget::~CSoftItemWidget()
{
	qDebug() << "~CAppSetWidget()";
	DELETE_POINTER(m_tbtnAPP);
	DELETE_POINTER(m_tbtnOpr);
}

const SoftData& CSoftItemWidget::getData()
{
	return m_curData;
}

//���յ��˲ۺ���˵�������״̬�ѳɹ����£�����Ҫ����װ������״̬�ָ�������״̬
void CSoftItemWidget::updateState(const SoftData& data)
{
	m_curData = data;
	QString strName = m_curData.name.split("##").first();
	m_tbtnAPP->setText(strName);

	if(data.category == QString::fromLocal8Bit("��ҳ"))
	{
		m_tbtnAPP->setEnabled(true);
		m_tbtnOpr->setVisible(false);
		m_tbtnOpr->setEnabled(true);
		m_tbtnAPP->setIcon(QIcon(":/Resources/ie.png"));
	}
	else
	{
		if(m_curData.state == SMonitor::Upgrade)
		{
			m_tbtnOpr->setEnabled(true);
			m_tbtnOpr->setVisible(true);
			m_tbtnAPP->setEnabled(true);
			m_tbtnAPP->setIcon(CMisc::GetFileIcon(m_curData.location + "/" + m_curData.mainExe));
			m_tbtnOpr->setText(QString::fromLocal8Bit("����"));
		}
		else if(m_curData.state == SMonitor::UnInstall)
		{
			m_tbtnAPP->setEnabled(false);
			m_tbtnAPP->setVisible(true);
			m_tbtnOpr->setVisible(true);
			m_tbtnOpr->setEnabled(true);
			m_tbtnOpr->setText(QString::fromLocal8Bit("��װ"));
		}
		else
		{
			m_tbtnAPP->setEnabled(true);
			m_tbtnAPP->setVisible(true);
			m_tbtnAPP->setIcon(CMisc::GetFileIcon(m_curData.location + "/" + m_curData.mainExe));
			m_tbtnOpr->setVisible(false);
			m_tbtnOpr->setEnabled(true);
		}
	}
}

void CSoftItemWidget::setOperation()
{
	QToolButton* tbtn = qobject_cast<QToolButton*> (sender());
	QString text = "";
	if(tbtn)
		text = tbtn->text();
	else
	{
		if(m_curData.state == SMonitor::Upgrade)
			text = QString::fromLocal8Bit("����");
		else if(m_curData.state == SMonitor::UnInstall)
			text = QString::fromLocal8Bit("��װ");
	}

	if(text == QString::fromLocal8Bit("��װ"))
	{
		if(confirmOpertion())
		{
			Task task;
			task.key = m_curData.name.split("##").first();
			task.srcPath = m_curData.url;
			task.desPath = m_curData.location;
			task.version = m_curData.version;
			task.category = m_curData.category;
			task.mainExe = m_curData.mainExe;
			task.state = m_curData.state;
			SoftManager->AddTask(task);
			m_tbtnOpr->setEnabled(false);
			m_tbtnOpr->setText(QString::fromLocal8Bit("���ڰ�װ"));
		}
		else
		{
			m_tbtnOpr->setEnabled(true);
			m_tbtnOpr->setText(QString::fromLocal8Bit("��װ"));
			//emit InstallCancled(m_curData.name.split("##").first(), m_curData.state);
			emit SoftManager->InstallCancled(m_curData.name.split("##").first(), m_curData.state);
		}
	}
	else if(text == QString::fromLocal8Bit("����"))
	{
		emit NotifyOperation(SMonitor::Upgrade, m_curData.category, m_curData.name);

		Task task;
		task.key = m_curData.name.split("##").first();
		task.srcPath = m_curData.url;
		task.desPath = m_curData.location;
		task.version = m_curData.version;
		task.category = m_curData.category;
		task.mainExe = m_curData.mainExe;
		task.state = m_curData.state;
		SoftManager->AddTask(task);
		m_tbtnOpr->setEnabled(false);
		m_tbtnOpr->setText(QString::fromLocal8Bit("��������"));
	}
}

void CSoftItemWidget::installFinished()
{
	QMutexLocker qMutexLocker(&m_dataMutex);
	m_curData.state = SMonitor::Normal;
	//SaveSettings(m_curData.brand);
	CMisc::SetBrandsNotNeedPasswd(m_curData.brand);
	updateState(m_curData);

	if(!MessageDataMediator->m_bCrossLinkIsInstalled && MessageDataMediator->m_strShopId.isEmpty())
	{//����û�л�ȡ���ŵ���Ϣ�����»�ȡһ��
		MessageDataMediator->QueryShopInfo(true);
	}
}

void CSoftItemWidget::downloadQuit()
{
	QMutexLocker qMutexLocker(&m_dataMutex);
	//m_curData.state = SMonitor::Normal;
	//SaveSettings(m_curData.brand);
	//CMisc::SetBrandsNotNeedPasswd(m_curData.brand);
	updateState(m_curData);
}

void CSoftItemWidget::installErrorOccur()
{
	m_tbtnOpr->setEnabled(true);

	if(m_tbtnOpr->text().contains(QString::fromLocal8Bit("����")))
		m_tbtnOpr->setText(QString::fromLocal8Bit("����"));
	else if(m_tbtnOpr->text().contains(QString::fromLocal8Bit("��װ")))
		m_tbtnOpr->setText(QString::fromLocal8Bit("��װ"));

	m_tbtnOpr->setEnabled(true);
}


bool CSoftItemWidget::confirmOpertion()
{
	if(IsBrandNeedPasswd(m_curData.brand))
	{
		QString passwd = CMisc::GetBrandPasswd(m_curData.brand);
		if(!passwd.isEmpty())
		{
			QString text = QInputDialog::getText(this, QString::fromLocal8Bit("��������:"),
				QString::fromLocal8Bit("�����밲װ���룺"),
				QLineEdit::Password, "", 0, 
				Qt::WindowSystemMenuHint);

			if(text.isEmpty())
				return false;

			if(CMisc::Encrypt(text) == passwd)
			{
				//����һ������֮�����������������
				CMisc::SetBrandsNotNeedPasswd(m_curData.brand);
				emit NotifyOperation(SMonitor::Downloading,m_curData.category, m_curData.name);
				return true;
			}
			else
			{
				QMessageBox::critical(this, QString::fromLocal8Bit("����"),
					QString::fromLocal8Bit("�������������"),
					QString::fromLocal8Bit("ȷ��"));

				return false;
			}
		}
	}

	emit NotifyOperation(SMonitor::Downloading, m_curData.category, m_curData.name);
	return true;
}


bool CSoftItemWidget::IsBrandNeedPasswd(const QString& brand)
{
	if(CategoryFactory->GetLocalBrand() == brand)
		return false;

	//��ȡ����Ҫ���������Ʒ�Ƶ��б�,�����������Ҫ�����Ʒ��
	//������ǰƷ�ƣ����߲��������Ǹ�Ʒ�Ƶ�����Ϊ�գ�����Ϊ����Ҫ��������
	if(CMisc::BrandsNotNeedPasswd().contains(brand))
		return false;
	else
	{
		if(CMisc::GetBrandPasswd(brand).isEmpty())
			return false;
	}

	//if(brand != QString::fromLocal8Bit("ɭ��")
	//	&& brand != QString::fromLocal8Bit("����"))
	//	return false;
	
	return true;
}

QString CSoftItemWidget::gernateToolTips(const SoftData& data)
{
	QString tooltip;
	if(data.category == QString::fromLocal8Bit("����"))
		tooltip += QString("<h2 style='color:#1B89CA'>%1</h2>").arg(data.name.split("##").first());
	else
		tooltip += QString("<h2 style='color:#1B89CA'>%1</h2>").arg(data.name);

	QString table = QString::fromLocal8Bit("<table>");
	if(data.category == QString::fromLocal8Bit("��ҳ"))
	{
		//table +=  QString::fromLocal8Bit(
		//	"<tr><td><pre>�汾:</pre></td>"
		//	"<td><pre>%1</pre></td></tr>"
		//	).arg(data.version);

		//table +=  QString::fromLocal8Bit(
		//	"<tr><td><pre>Ʒ��:</pre></td>"
		//	"<td><pre>%1</pre></td></tr>"
		//	).arg(data.brand);

		table += QString::fromLocal8Bit(
			"<tr><td><pre>����:</pre></td>"
			"<td><pre>%1</pre></td></tr>"
			).arg(data.category);

		table += QString::fromLocal8Bit(
			"<tr><td><pre>��ַ:</pre></td>"
			"<td><pre>%1</pre></td></tr>"
			).arg(data.url);
	}
	else if(data.category == QString::fromLocal8Bit("����"))
	{
		table +=  QString::fromLocal8Bit(
			"<tr><td><pre>Ʒ��:</pre></td>"
			"<td><pre>%1</pre></td></tr>"
			).arg(data.brand);

		table += QString::fromLocal8Bit(
			"<tr><td><pre>����:</pre></td>"
			"<td><pre>%1</pre></td></tr>"
			).arg(data.category);
	}
	else
	{
		table +=  QString::fromLocal8Bit(
			"<tr><td><pre>�汾:</pre></td>"
			"<td><pre>%1</pre></td></tr>"
			).arg(data.version);

		table +=  QString::fromLocal8Bit(
			"<tr><td><pre>Ʒ��:</pre></td>"
			"<td><pre>%1</pre></td></tr>"
			).arg(data.brand);

		table += QString::fromLocal8Bit(
			"<tr><td><pre>����:</pre></td>"
			"<td><pre>%1</pre></td></tr>"
			).arg(data.category);

		//table += QString::fromLocal8Bit(
		//	"<tr><td><pre>λ��:</pre></td>"
		//	"<td><pre>%1</pre></td></tr>"
		//	).arg(data.location);

		//table += QString::fromLocal8Bit(
		//	"<tr><td><pre>��ַ:</pre></td>"
		//	"<td><pre>%1</pre></td></tr>"
		//	).arg(data.url);

		
	}
	table += "</table>";

	return tooltip + table;
}

void CSoftItemWidget::OpenExternalProgram()
{
	if(m_curData.category == QString::fromLocal8Bit("��ҳ"))
	{
		QDesktopServices::openUrl(QUrl(m_curData.url));
	}
	else if(m_curData.category != QString::fromLocal8Bit("����"))
	{
		if(!m_curData.mainExe.isEmpty())
		{
			QString strExe = m_curData.location + "/" + m_curData.mainExe;
			if(QFileInfo(strExe).suffix() == "lnk")
			{
				ShellExecute(NULL, "open", strExe.toLocal8Bit().constData(), NULL, NULL, SW_SHOWNORMAL);
			}
			else
			{
				CMisc::ExcuteExe(strExe, false, 0, false);
			}
		}
	}
}