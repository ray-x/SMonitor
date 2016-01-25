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
#include "CSideBarWidget.h"
#include "CStateButton.h"
#include "CPushButton.h"

#include <QVBoxLayout>
#include <QButtonGroup>
#include <QPainter>

CSideBarWidget::CSideBarWidget(QWidget *parent)
	: QFrame(parent)
{
	setObjectName("sideBarWidget");
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_buttonGroup = new QButtonGroup(this);

	setMaximumWidth(120);
	m_mainLayout = new QVBoxLayout(this);
	m_mainLayout->setMargin(0);
	m_mainLayout->setSpacing(0);
	connect(m_buttonGroup, SIGNAL(buttonToggled(int, bool)), this, SIGNAL(currentIndexChanged(int)));
}

CSideBarWidget::~CSideBarWidget()
{

}

void CSideBarWidget::addTab(const QString& text, const QChar& icon)
{
	CPushButton* sideTab = new CPushButton(icon, text, this);
	sideTab->setCheckable(true);
	int count = m_mainLayout->count();

	int step = 0;
	QList<QAbstractButton*> buttons = m_buttonGroup->buttons();
	for each(auto button in buttons)
	{
		int id = m_buttonGroup->id(button);
		if(count == id)
			step = 1;
		m_buttonGroup->setId(button, id+step); //将位于count之后的button id后移一位
	}
	m_buttonGroup->addButton(sideTab, count);
	m_mainLayout->insertWidget(count, sideTab/*, 0, Qt::AlignTop*/);
}

void CSideBarWidget::insertTab(int index, QString& text, const QString& icon)
{
	CStateButton* sideTab = new CStateButton(icon, this, false);
	int count = m_mainLayout->count();

	QList<QAbstractButton*> buttons = m_buttonGroup->buttons();
	int step = 0;
	for each(auto button in buttons)
	{
		int id = m_buttonGroup->id(button);
		if(index == id)
			step = 1;
		m_buttonGroup->setId(button, id+step);
	}
	m_buttonGroup->addButton(sideTab, index);
	m_mainLayout->insertWidget(index, sideTab, 0, Qt::AlignTop);
}

void CSideBarWidget::setCurrentIndex(int index)
{
	m_buttonGroup->button(index)->setChecked(true);
}