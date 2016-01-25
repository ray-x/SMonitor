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
#include "CItemDelegate.h"

#include <QComboBox>
#include <QDebug>
#include <QCalendarWidget>
#include <QSignalMapper>

CItemDelegate::CItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	resetBrand();
}

CItemDelegate::~CItemDelegate()
{

}


QWidget* CItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index)const
{
	if(index.column() == 1)
	{
		QCalendarWidget *pCalendar = new QCalendarWidget(parent);
		pCalendar->setGridVisible(true);
		connect(pCalendar, &QCalendarWidget::clicked, pCalendar, &QCalendarWidget::hide);
		
		pCalendar->setFixedSize(QSize(250, 180));
		return pCalendar;
	}
	else if(index.column() == 3)
	{
		QComboBox* comboBox = new QComboBox(parent);
		bool isDriver = index.model()->data(index, Qt::UserRole + 1).toBool();
		if(isDriver)
			comboBox->addItems(QStringList() << QString::fromLocal8Bit("爱普生"));
		else
			comboBox->addItems(m_brandList);
		return comboBox;
	}
	else if(index.column() == 4)
	{  //这个版本不用升级包
		QComboBox* comboBox = new QComboBox(parent);
		//comboBox->addItems(QStringList() << QString::fromLocal8Bit("ERP") << QString::fromLocal8Bit("驱动")<< QString::fromLocal8Bit("网页")<< QString::fromLocal8Bit("升级包") << QString::fromLocal8Bit("其他"));
		comboBox->addItems(QStringList() << QString::fromLocal8Bit("ERP") << QString::fromLocal8Bit("驱动")<< QString::fromLocal8Bit("网页") << QString::fromLocal8Bit("其他"));
		return comboBox;
	}

	return QStyledItemDelegate::createEditor(parent, option, index);
}

void CItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index)const
{
	if(index.column() == 3 || index.column() == 4)
	{
		QComboBox* comboBox = qobject_cast<QComboBox*> (editor);
		if(comboBox)
		{
			QString data = index.model()->data(index).toString();
			comboBox->setCurrentText(data);
		}
	}
	else if(index.column() == 1)
	{
		QCalendarWidget *pCalendar = qobject_cast<QCalendarWidget*>(editor);
		if(pCalendar)
		{
			QString str = index.model()->data(index, Qt::EditRole).toString();
			QDate curDate = QDate::fromString(str, "yyyyMMdd");
			if(!curDate.isNull())
			{
				pCalendar->setSelectedDate(curDate);
			}
		}
	}
	else
	{
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void CItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index)const
{
	if(index.column() == 1)
	{
		QCalendarWidget *pCalendar = qobject_cast<QCalendarWidget*>(editor);
		if(pCalendar)
		{
			model->setData(index, pCalendar->selectedDate().toString(QString("yyyyMMdd")));
		}
	}
	else if(index.column() == 3 || index.column() == 4)
	{
		QComboBox* comboBox = qobject_cast<QComboBox*> (editor);
		if(comboBox)
		{
			QString data = comboBox->currentText();
			model->setData(index, data);
		}
	}
	else
	{
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void CItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)const
{
	QStyleOptionViewItem itemOption(option);
	// remove the focus state
	if (itemOption.state & QStyle::State_HasFocus)
	{
		itemOption.state ^= QStyle::State_HasFocus;
	}
	QStyledItemDelegate::paint(painter, itemOption, index);
}

void CItemDelegate::addBrand(const QString& text)
{
	if(!m_brandList.contains(text)){
		m_brandList.append(text);
	}
}

void CItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(index.column() == 1)
	{
		//QCalendarWidget *pCalendar = qobject_cast<QCalendarWidget*>(editor);
		//pCalendar->move(option.rect.x(), 0); //这里控制一下显示的位置，让他始终靠近表格的顶端

		qDebug() << option.rect;

		QRect rect = option.rect;

		if(rect.y() > 177)
			rect.setY(177);

		editor->setGeometry(rect);
	}
	else
	{
		QStyledItemDelegate::updateEditorGeometry(editor, option, index);
	}
}

void CItemDelegate::resetBrand()
{
	m_brandList.clear();
	//初始化时只包含这两个品牌
	m_brandList << QString::fromLocal8Bit("品牌1") << QString::fromLocal8Bit("品牌2");
}