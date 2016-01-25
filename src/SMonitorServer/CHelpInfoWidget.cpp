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
#include "CHelpInfoWidget.h"
#include "Define.h"
#include "databasemanager.h"
#include "CNoFocusDelegate.h"
#include "CMessageDataMediator.h"
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include "Markup.h"

CHelpInfoWidget::CHelpInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	setWindowIcon(QIcon(":/Resources/normal.png"));
	connect(tbtn_add, &QToolButton::clicked, this, &CHelpInfoWidget::addContent);
	connect(tbtn_delete, &QToolButton::clicked, this, &CHelpInfoWidget::removeContent);
	connect(btn_Save, &QPushButton::clicked, this, &CHelpInfoWidget::saveContent);
	connect(btn_SaveXml, &QPushButton::clicked, this, &CHelpInfoWidget::SaveToXml);
	connect(tableWidget, &QTableWidget::itemChanged, this, &CHelpInfoWidget::itemChanged);

	tableWidget->setItemDelegate(new CNoFocusDelegate(this));
	tableWidget->setColumnWidth(0, 150);
	initWidget();
}

CHelpInfoWidget::~CHelpInfoWidget()
{

}

//添加一行数据
void CHelpInfoWidget::addContent()
{
	prepareItemChange();
	int row = tableWidget->rowCount();
	QTableWidgetItem* item0 = new QTableWidgetItem(getNextDistinctChildName(QString::fromLocal8Bit("标题")));
	item0->setTextAlignment(Qt::AlignCenter);
	item0->setForeground(QColor(254, 102, 24));
	QTableWidgetItem* item1 = new QTableWidgetItem(QString::fromLocal8Bit("内容..."));
	item1->setTextAlignment(Qt::AlignCenter);
	item1->setForeground(QColor(254, 102, 24));
	tableWidget->setRowCount(row + 1);
	tableWidget->setItem(row, 0, item0);
	tableWidget->setItem(row, 1, item1);
	tableWidget->setCurrentItem(item0);
	tableWidget->editItem(item0);
	endItemChange();
}

//删除一行数据
void CHelpInfoWidget::removeContent()
{
	int row = tableWidget->currentRow();
	if(row !=  -1)
	{
		tableWidget->removeRow(row);
	}
}

void CHelpInfoWidget::saveContent()
{
	int rowCount = tableWidget->rowCount();
	HelpInfoMap map;

	for(int i = 0; i < rowCount; i++){
		map.insert(tableWidget->item(i, 0)->text(), tableWidget->item(i, 1)->text());
	}

	if(DatabaseManager::Instance()->SaveHelpInfos(map))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("消息"),
			QString::fromLocal8Bit("保存信息成功"), 
			QString::fromLocal8Bit("确定"));

		prepareItemChange();
		for(int i = 0; i < rowCount; i++)
		{
			tableWidget->item(i, 0)->setForeground(Qt::black);
			tableWidget->item(i, 1)->setForeground(Qt::black);
		}
		endItemChange();

		MessageDataMediator->m_HelpInfoMap.clear();
		MessageDataMediator->m_HelpInfoMap = map;
	}
}

void CHelpInfoWidget::itemChanged(QTableWidgetItem* item)
{
	int column = item->column();
	if(column == 0)
	{
		QString text = item->text();
		QList<QTableWidgetItem*> list = tableWidget->findItems(text, Qt::MatchExactly);
		list.removeOne(item);
		if(list.count() > 0)
		{
			QString suggestedName = getNextDistinctChildName(text);
			int value = QMessageBox::warning(this, QString::fromLocal8Bit("重命名"), QString::fromLocal8Bit("<h6 style=\" color:blue;\">将 \"%1\" 重命名为 \"%2\"</h6>\
							<p>已包含同名项.</p>").arg(text).arg(suggestedName), QString::fromLocal8Bit("确定"));
			switch(value)
			{
			case 0:
				{
					prepareItemChange();
					item->setText(suggestedName);
					endItemChange();
				}
				break;
			default:
				break;
			}
		}
	}
}

void CHelpInfoWidget::initWidget()
{
	prepareItemChange();
	if(DatabaseManager::Instance()->ReadHelpInfos(MessageDataMediator->m_HelpInfoMap))
	{
		int row = MessageDataMediator->m_HelpInfoMap.count();
		tableWidget->setRowCount(row);

		int index = 0;
		QMapIterator<QString, QString> iter(MessageDataMediator->m_HelpInfoMap);
		while (iter.hasNext())
		{
			iter.next();
			QTableWidgetItem* item0 = new QTableWidgetItem(iter.key());
			item0->setTextAlignment(Qt::AlignCenter);
			QTableWidgetItem* item1 = new QTableWidgetItem(iter.value());
			item1->setTextAlignment(Qt::AlignCenter);
			tableWidget->setItem(index, 0, item0);
			tableWidget->setItem(index++, 1, item1);
		}
	}
	endItemChange();
}

QString CHelpInfoWidget::getNextDistinctChildName(const QString& suggestedName) const
{
	if(suggestedName.isEmpty())
		return QString();

	QSet<QString> names;
	int rowCount = tableWidget->rowCount();
	for (int i = 0; i < rowCount; i++)
		names.insert(tableWidget->item(i, 0)->text());

	int seq = 2;
	QString base;

	QRegExp regex("(\\d+)$", Qt::CaseInsensitive);
	int idx = regex.indexIn(suggestedName);
	if(idx != -1){
		base = suggestedName.left(idx);
		seq = regex.cap(1).toInt() + 1;
	}
	else
		base = suggestedName;

	QString result = suggestedName;
	while(names.contains(result)){
		result = QString(base+"%1").arg(seq++);
	}

	return result;
}

void CHelpInfoWidget::prepareItemChange()
{
	disconnect(tableWidget, &QTableWidget::itemChanged, this, &CHelpInfoWidget::itemChanged);
}

void CHelpInfoWidget::endItemChange()
{
	connect(tableWidget, &QTableWidget::itemChanged, this, &CHelpInfoWidget::itemChanged);
}

void CHelpInfoWidget::SaveToXml()
{
	QString path = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存"), ".", tr("Xml (*.xml)"));
	if(path.isEmpty()) 
	{
		return ;
	}

	QFile::remove(path);
	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Message");
	xml.AddChildElem("Info");
	xml.IntoElem();
	int rowCount = tableWidget->rowCount();
	for(int i = 0; i < rowCount; i++)
	{
		xml.AddChildElem("HelpInfo");
		xml.IntoElem();
		QString label = tableWidget->item(i, 0)->text();
		QString value = tableWidget->item(i, 1)->text();
		xml.AddAttrib("Label", label.toStdString().c_str());
		xml.AddAttrib("Value", value.toStdString().c_str());
		xml.OutOfElem();
	}
	xml.OutOfElem();
	xml.Save(path.toLocal8Bit().constData());
}
