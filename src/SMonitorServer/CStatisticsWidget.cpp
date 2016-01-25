
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
#include "CStatisticsWidget.h"
#include "databasemanager.h"
#include "Define.h"
#include "excelhelper.h"
#include "CNoFocusDelegate.h"

#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

CStatisticsWidget::CStatisticsWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	initWidget();

	tableWidget->setColumnWidth(0, 200);
	tableWidget->setColumnWidth(1, 200);
	tableWidget->setColumnWidth(2, 200);
	tableWidget->setColumnWidth(3, 200);
	tableWidget->setColumnWidth(4, 200);
	tableWidget->setColumnWidth(5, 100);
	tableWidget->setItemDelegate(new CNoFocusDelegate(this));

	connect(btn_ExportAll, &QPushButton::clicked, this, &CStatisticsWidget::exportAll);
}

CStatisticsWidget::~CStatisticsWidget()
{

}


void CStatisticsWidget::initWidget()
{
	SoftwareinfoVector vec;
	DatabaseManager::Instance()->ReadAllSoftwareInfo(vec);
	
	int index = 0;
	for (int i = 0; i < vec.count(); i++)
	{
		const SoftwareInfo& info = vec.at(i);
		if(info.category == QString::fromLocal8Bit("网页"))
			continue;
		
		tableWidget->setRowCount(index + 1);
		for (int j = 0; j < 6; j++)
		{
			SetItem(index, j, info);
		}
		index++;
	}

	for (int row = 0; row < tableWidget->rowCount(); row++)
	{
		QString name = tableWidget->item(row, 0)->text();
		QString version = tableWidget->item(row, 1)->text();
		m_dataRowHash.insert(name, row);
	}
}

void CStatisticsWidget::SetItem(int row, int column, const SoftwareInfo& info)
{
	QTableWidgetItem* item = new QTableWidgetItem;
	item->setTextAlignment(Qt::AlignCenter);
	switch(column)
	{
	case 0:
		{
			item->setText(info.name);
			tableWidget->setItem(row, column, item);
		}
		break;
	case 1:
		{
			item->setText(info.version);
			tableWidget->setItem(row, column, item);
		}
		break;
	case 2:
		{
			item->setText(QString::number(info.installNum));
			tableWidget->setItem(row, column, item);
		}
		break;
	case 3:
		{
			item->setText(QString::number(info.uninstalledNum));
			tableWidget->setItem(row, column, item);
		}
		break;
	case 4:
		{
			item->setText(info.category);
			tableWidget->setItem(row, column, item);
		}
		break;
	case 5:
		{
			QPushButton* btn_export = new QPushButton(QString::fromLocal8Bit("导出"),tableWidget);
			btn_export->setMinimumWidth(50);
			connect(btn_export, &QPushButton::clicked, this, &CStatisticsWidget::exportData);
			tableWidget->setCellWidget(row, column, btn_export);
			m_exportBtnHash.insert(btn_export, row);
		}
		break;
	default:
		break;
	}
}

void CStatisticsWidget::exportData()
{
	QPushButton* button = qobject_cast<QPushButton*> (sender());
	if(button && m_exportBtnHash.contains(button))
	{
		int row = m_exportBtnHash.value(button);
		qDebug() << row;
		QString path = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("导出"),
			QApplication::applicationDirPath(), tr("Excel (*.xml)"));
		if(!path.isEmpty())
		{
			QString columnNames[6];
			int columnWidths[6];
			QStringList content, contentList;
			for(int i = 0; i < tableWidget->columnCount() - 1; i++)
			{
				columnNames[i] = tableWidget->horizontalHeaderItem(i)->text();
				columnWidths[i] = tableWidget->columnWidth(i);
				content.append(tableWidget->item(row, i)->text());
			}
			contentList.append(content.join(";"));

			bool bExport = ExcelHelper::Instance()->ToExcel(path, QString::fromLocal8Bit("软件安装信息统计"),
						QString::fromLocal8Bit("软件安装信息统计"), columnNames, columnWidths, 5, contentList);
			if(bExport)
			{
				QMessageBox::information(this, QString::fromLocal8Bit("信息"),
					QString::fromLocal8Bit("导出信息成功"),
					QString::fromLocal8Bit("确定"));
			}
		}
	}
}

void CStatisticsWidget::updateInstallData(QPair<QString, QString> key, int num)
{
	if(m_dataRowHash.contains(key.first))
	{
		int row = m_dataRowHash.value(key.first);
		if(tableWidget->item(row, 1) && key.second == tableWidget->item(row, 1)->text())
			tableWidget->item(row, 2)->setText(QString::number(num));
	}
}

void CStatisticsWidget::updateUnInstallData(QPair<QString, QString> key, int num)
{
	if(m_dataRowHash.contains(key.first))
	{
		int row = m_dataRowHash.value(key.first);
		if(tableWidget->item(row, 1) && key.second == tableWidget->item(row, 1)->text())
			tableWidget->item(row, 3)->setText(QString::number(num));
	}
}

void CStatisticsWidget::updateAllNumData(const QString& name, const QString& version, int installNum, int unInstallNum)
{
	if(m_dataRowHash.contains(name))
	{
		int row = m_dataRowHash.value(name);
		if(tableWidget->item(row, 1) && version == tableWidget->item(row, 1)->text())
		{
			tableWidget->item(row, 2)->setText(QString::number(installNum));
			tableWidget->item(row, 3)->setText(QString::number(unInstallNum));
		}
	}
}

void CStatisticsWidget::initData()
{
	updateData();
}

void CStatisticsWidget::updateData()
{

	for(int i = 0; i < tableWidget->rowCount(); i++){
		tableWidget->removeCellWidget(i, 5);
		tableWidget->removeRow(i);
	}

	QHashIterator<QPushButton*, int> iter(m_exportBtnHash);
	while(iter.hasNext())
	{
		iter.next();
		m_exportBtnHash.remove(iter.key());
	}
	m_exportBtnHash.clear();
	m_dataRowHash.clear();
	initWidget();
}

void CStatisticsWidget::updateItem(const QString& key)
{
	if(m_dataRowHash.contains(key))
	{
		int row = m_dataRowHash.value(key);
		SoftwareInfo info;
		if(DatabaseManager::Instance()->ReadSoftwareInfo(key, info))
		{
			for (int i = 0; i < tableWidget->columnCount(); i++)
			{
				QTableWidgetItem* item = tableWidget->item(row, i);
				if(!item) continue;
				switch(i)
				{
				case 0:
					item->setText(info.name);
					break;
				case 1:
					item->setText(info.version);
					break;
				case 2:
					item->setText(QString::number(info.installNum));
					break;
				case 3:
					item->setText(QString::number(info.uninstalledNum));
					break;
				case 4:
					item->setText(info.category);
					break;
				default:
					break;
				}
			}
		}
	}
	else
	{
		updateData();
	}
}

void CStatisticsWidget::removeItem(const QString& key, int nRow)
{
	if(m_dataRowHash.contains(key))
	{
		int row = m_dataRowHash.value(key);
		tableWidget->removeRow(row);
		m_dataRowHash.remove(key);

		//删除一行后，将位于改行之后的数据的行号-1
		QHash<QString, int> rowHash = m_dataRowHash;
		QHashIterator<QString, int> iter(rowHash);
		while(iter.hasNext())
		{
			iter.next();
			if(iter.value() > row)
			{
				m_dataRowHash.insert(key, iter.value() - 1);
			}
		}

	}
}

void CStatisticsWidget::addItem(const QString& key)
{
	if(!m_dataRowHash.contains(key))
	{
		int rowCount = tableWidget->rowCount();
		tableWidget->setRowCount(rowCount + 1);

		SoftwareInfo info;
		DatabaseManager::Instance()->ReadSoftwareInfo(key, info);
		for (int i = 0; i < 6; i++)
		{
			SetItem(rowCount, i, info);
		}
		m_dataRowHash.insert(key, rowCount);
	}
}


void CStatisticsWidget::exportAll()
{
	QString path = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("导出"),
		QApplication::applicationDirPath(), tr("Excel (*.xml)"));

	if(path.isEmpty())
		return;

	QString columnNames[6] = {""};
	int columnWidths[6] = {100, 100, 100, 100, 100};
	QStringList contentList;
	int rowCount = tableWidget->rowCount();
	int columnCount = tableWidget->columnCount() - 1;
	for (int i = 0; i < rowCount; i++)
	{
		QStringList content;
		for (int j = 0; j < columnCount; j++)
		{
			if(columnNames[j].isEmpty()){
				columnNames[j] = tableWidget->horizontalHeaderItem(j)->text();
				columnWidths[j] = tableWidget->columnWidth(j);
			}
			content.append(tableWidget->item(i, j)->text());
		}
		contentList.append(content.join(";"));
	}

	if(ExcelHelper::Instance()->ToExcel(path, QString::fromLocal8Bit("软件安装信息统计"),
		QString::fromLocal8Bit("软件信息统计"), columnNames, columnWidths, columnCount, contentList))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("信息"),
			QString::fromLocal8Bit("导出信息成功"),
			QString::fromLocal8Bit("确定"));
	}
	else
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("导出信息失败"),
			QString::fromLocal8Bit("确定"));
	}

	return;

	QHash<int, QStringList> data;

	for (int i = 0; i < 10000; i++)
	{
		QStringList cells;
		for (int j = 0; j < 6; j++)
		{
			cells << QString("%1%2").arg(i).arg(QString::number(j+10, 16).toUpper());
		}
		data.insert(i, cells);
	}

	QString fileName = QApplication::applicationDirPath() + "/Database/export.xml";
	qDebug() << QString::fromLocal8Bit("数据量：%1条").arg(data.count());
	qDebug() << QString::fromLocal8Bit("开始时间：") << QDateTime::currentDateTime().toString("yyyy-HH-MM hh:mm:ss zzz");
	ExcelHelper::Instance()->ToExcel(fileName, data);
	QDateTime::currentDateTime().toString("yyyy-HH-MM hh:mm:ss zzz");
	qDebug() << QString::fromLocal8Bit("结束时间：") << QDateTime::currentDateTime().toString("yyyy-HH-MM hh:mm:ss zzz");
}
