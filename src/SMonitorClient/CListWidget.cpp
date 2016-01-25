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

#include "CListWidget.h"

#include <QWheelEvent>
#include <QScrollBar>
#include <QContextMenuEvent>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QClipboard>
#include <QApplication>

CListWidget::CListWidget(QWidget *parent)
	: QListWidget(parent)
{
	setFixedHeight(107);
	setObjectName("clistwidget");
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setAlternatingRowColors(true);
	setContextMenuPolicy(Qt::NoContextMenu);
}

CListWidget::~CListWidget()
{

}

void CListWidget::wheelEvent(QWheelEvent * e)
{
	if(Qt::ScrollBarAsNeeded == this->verticalScrollBarPolicy())
		QListWidget::wheelEvent(e);
}

CLabel::CLabel(QWidget *parent)
	: QLabel(parent)
{

}

CLabel::~CLabel()
{

}

void CLabel::contextMenuEvent(QContextMenuEvent *ev)
{
	QMenu* menu = new QMenu(this);
	menu->addAction(QString::fromLocal8Bit("¸´ÖÆ"), this, SLOT(copy()), QKeySequence::Copy);
	menu->addAction(QString::fromLocal8Bit("È«Ñ¡"), this, SLOT(selectAll()), QKeySequence::SelectAll);
	
	if (!menu) {
		ev->ignore();
		return;
	}

	ev->accept();
	menu->setAttribute(Qt::WA_DeleteOnClose);
	menu->popup(ev->globalPos());
}

void CLabel::copy()
{
	QString text = this->selectedText();
	QClipboard* clip = QApplication::clipboard();
	clip->setText(text);
}

void CLabel::selectAll()
{
	//int start = this->text().indexOf("]") + 1;
	//int end = this->text().length() - start;
	this->setSelection(0, this->text().length());
}