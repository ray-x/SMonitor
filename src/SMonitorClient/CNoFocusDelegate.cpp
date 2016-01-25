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

#include "CNoFocusDelegate.h"

#include <QToolButton>
#include <QProgressBar>
#include <QDebug>

CNoFocusDelegate::CNoFocusDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{

}

CNoFocusDelegate::~CNoFocusDelegate()
{

}

void CNoFocusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem itemOption(option);
	// remove the focus state
	if (itemOption.state & QStyle::State_HasFocus)
	{
		itemOption.state ^= QStyle::State_HasFocus;
	}

	QStyledItemDelegate::paint(painter, itemOption, index);
}

void CNoFocusDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(index.column() == 4)
	{
		QRect rect = option.rect;
		QToolButton* toolButton = qobject_cast<QToolButton*> (editor);
		if(toolButton)
		{
			int width = toolButton->width();
			int rWidth = rect.width();
			rect.setX(rect.x() + (rect.width() - width)/2);
			rect.setY(rect.y() + 2);
			rect.setHeight(25);
			editor->setGeometry( rect );
			//QStyledItemDelegate::updateEditorGeometry(editor, option, index);
		}
		else 
		{
			//QProgressBar* progressBar = qobject_cast<QProgressBar*> (editor);
			//if(progressBar)
			//{
				editor->setGeometry(rect);
			//}
		}
	}
	else
	{
		QStyledItemDelegate::updateEditorGeometry(editor, option, index);
	}
}
