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

#include "CBrandList.h"
#include "CTitleWidget.h"
#include "CNoFocusDelegate.h"

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>
#include <QMouseEvent>
#include <QApplication>

CBrandList::CBrandList(QWidget *parent)
	: QDialog(parent)
	, m_pCurSelBrand("")
	, m_bPressed(false)
{
	setObjectName("passwdframe");
	//setFixedSize(QSize(200, 300));
	setMaximumWidth(170);
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::FramelessWindowHint);
	//setAttribute(Qt::WA_DeleteOnClose);

	m_titleWidget = new CTitleWidget(this);
	m_titleWidget->setParent(this);
	m_titleWidget->setMinimumHint(false);
	m_titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_titleWidget->setTitle(QString::fromLocal8Bit("请选择需要安装的品牌"));
	connect(m_titleWidget, &CTitleWidget::closed, this, &CBrandList::reject);

	QFrame* frame = new QFrame(this);
	frame->setObjectName("brandlist");

	m_listWidget = new QListWidget(this);
	m_listWidget->setObjectName("brandlist");
	m_listWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	m_listWidget->setItemDelegate(new CNoFocusDelegate(this));
	m_listWidget->setStyleSheet("border:none");

	m_listWidget->setAlternatingRowColors(true);
	connect(m_listWidget, &QListWidget::itemClicked, this, &CBrandList::itemClicked);

	QVBoxLayout* layout = new QVBoxLayout(frame);
	layout->setMargin(0);
	layout->addWidget(m_listWidget);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(m_titleWidget);
	mainLayout->addWidget(frame);
}

CBrandList::~CBrandList()
{
	qDebug() << "~CBrandList()";
	delete m_titleWidget;
	m_titleWidget = nullptr;
}

void CBrandList::itemClicked(QListWidgetItem* item)
{
	if(item)
	{
		m_pCurSelBrand = item->text();
		accept();
	}
}

void CBrandList::SetBrands(const QStringList &brands)
{
	QStringList brandList = brands;
	brandList.removeDuplicates();
	foreach(auto brand, brandList)
	{
		QListWidgetItem* item = new QListWidgetItem;
		item->setTextAlignment(Qt::AlignCenter);
		item->setText(brand);
		item->setSizeHint(QSize(80, 30));
		m_listWidget->addItem(item);
	}
}

QString CBrandList::GetCurBrand()
{
	return m_pCurSelBrand;
}

void CBrandList::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(event->pos().y() < 27)
			m_bPressed = true;
	}

	//窗口移动距离
	m_movePoint = event->globalPos() - frameGeometry().topLeft();
}

void CBrandList::mouseMoveEvent(QMouseEvent *event)
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

void CBrandList::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_bPressed = false;
	}
}