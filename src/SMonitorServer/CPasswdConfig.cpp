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
#include "CPasswdConfig.h"
#include "CTitleWidget.h"
#include "CMisc.h"

#include <QTableWidget>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QHeaderView>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QTableWidgetItem>
#include <QApplication>

CPasswdConfig::CPasswdConfig(QWidget *parent)
	: QDialog(parent)
	, m_bPressed(false)
{
	setObjectName("passwdframe");
	setMinimumSize(QSize(350, 300));
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::FramelessWindowHint);
	m_titleWidget = new CTitleWidget(this);
	m_titleWidget->setParent(this);
	m_titleWidget->setMinimumHint(false);
	m_titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_titleWidget->setTitle(QString::fromLocal8Bit("密码配置"));
	connect(m_titleWidget, &CTitleWidget::closed, this, &CPasswdConfig::reject);

	QFrame* frame = new QFrame(this);
	frame->setObjectName("passwdtable");

	QLabel* label = new QLabel(this);
	label->setText(QString::fromLocal8Bit("请在表格中输入品牌密码"));

	m_tableWidget = new QTableWidget(this);
	m_tableWidget->verticalHeader()->hide();
	m_tableWidget->setColumnCount(2);
	m_tableWidget->setHorizontalHeaderLabels(QStringList() 
		<< QString::fromLocal8Bit("品牌名称")
		<< QString::fromLocal8Bit("密码"));
	m_tableWidget->horizontalHeader()->setStretchLastSection(true);
	m_tableWidget->setSelectionBehavior(QTableWidget::SelectItems);
	m_tableWidget->setSelectionMode(QTableWidget::SingleSelection);

	connect(m_tableWidget, &QTableWidget::itemChanged, this, &CPasswdConfig::dataChanged);

	m_buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
	m_buttonBox->addButton(QString::fromLocal8Bit("确定"), QDialogButtonBox::AcceptRole);
	m_buttonBox->addButton(QString::fromLocal8Bit("取消"), QDialogButtonBox::RejectRole);
	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &CPasswdConfig::acceptPasswd);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &CPasswdConfig::reject);

	QVBoxLayout* layout = new QVBoxLayout(frame);
	layout->addWidget(label);
	layout->addWidget(m_tableWidget);
	layout->addWidget(m_buttonBox);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(m_titleWidget);
	mainLayout->addWidget(frame);
}

CPasswdConfig::~CPasswdConfig()
{

}

void CPasswdConfig::SetBrands(const QStringList& brands)
{
	m_pBrandList = brands;
	foreach(auto brand, m_pBrandList)
	{
		setItem(brand);
	}
}

void CPasswdConfig::setItem(const QString& brand)
{
	int rowCount = m_tableWidget->rowCount();
	m_tableWidget->setRowCount(rowCount + 1);
	QTableWidgetItem* item0 = new QTableWidgetItem();
	item0->setText(brand);
	item0->setTextAlignment(Qt::AlignCenter);
	item0->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	QString passwd = CMisc::ReadBrandPasswd(brand);
	if(passwd.isEmpty())
	{
		if(brand == QString::fromLocal8Bit("品牌1"))
			passwd = SEMIRPASSWD;
		else if(brand == QString::fromLocal8Bit("品牌2"))
			passwd = BALABALAPASSWD;
	}

	QTableWidgetItem* item1 = new QTableWidgetItem();
	item1->setText(passwd);
	item1->setTextAlignment(Qt::AlignCenter);	

	m_tableWidget->setItem(rowCount, 0, item0);
	m_tableWidget->setItem(rowCount, 1, item1);
}


void CPasswdConfig::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(event->pos().y() < 27)
			m_bPressed = true;
	}

	//窗口移动距离
	m_movePoint = event->globalPos() - frameGeometry().topLeft();
}

void CPasswdConfig::mouseMoveEvent(QMouseEvent *event)
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

void CPasswdConfig::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_bPressed = false;
	}
}


void CPasswdConfig::acceptPasswd()
{
	if(m_bAnyChanged)
	{
		int rowCount = m_tableWidget->rowCount();
		for (int i = 0; i < rowCount; i++)
		{
			QTableWidgetItem* item0 = m_tableWidget->item(i, 0);
			QTableWidgetItem* item1 = m_tableWidget->item(i, 1);
			if(!CMisc::SetBrandPasswd(item0->text(), item1->text()))
			{
				QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
					QString::fromLocal8Bit("保存品牌密码失败，请稍后重试"),
					QString::fromLocal8Bit("确定"));
			}
		}
	}
	accept();
}

void CPasswdConfig::dataChanged()
{
	m_bAnyChanged = true;
}