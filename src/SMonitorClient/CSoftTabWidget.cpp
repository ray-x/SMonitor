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

#include "CSoftTabWidget.h"
#include "CTableWidget.h"
#include "CCategoryFactory.h"

#include <QTabWidget>
#include <QHBoxLayout>
#include <QDebug>

CSoftTabWidget::CSoftTabWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pHeaderLabels << QString::fromLocal8Bit("软件名称")
		<< QString::fromLocal8Bit("版本号") << QString::fromLocal8Bit("品牌")
		<< QString::fromLocal8Bit("安装位置") << QString::fromLocal8Bit("安装");

	m_pTabWidget = new QTabWidget(this);
	QHBoxLayout* hboxLayout = new QHBoxLayout(this);
	hboxLayout->addWidget(m_pTabWidget);
	connect(CategoryFactory, &CCategoryFactory::dataChangedNotify, this, &CSoftTabWidget::setWidgetData);
}

CSoftTabWidget::~CSoftTabWidget()
{
	qDebug() << "~CSoftTabWidget()";
	QMapIterator<QString, CTableWidget*> iter(m_pTabMap);
	while(iter.hasNext())
	{
		iter.next();
		CTableWidget* w =  iter.value();
		delete w;
		w = nullptr;
		m_pTabMap.remove(iter.key());
	}
}

void CSoftTabWidget::setWidgetData(const QMap<QString, QMap<QString, SoftData> >& data)
{
	QMapIterator<QString, QMap<QString, SoftData> > iter(data);
	while(iter.hasNext())
	{
		iter.next();
		if(iter.key() != QString::fromLocal8Bit("网页")){
			//如果存在该类说明Tab页已经创建,那么只需要进行数据更新
			if(m_pTabMap.contains(iter.key()))
			{
				m_pTabMap.value(iter.key())->updateTable(iter.value());
			}
			else
			{
				CTableWidget* table = new CTableWidget(iter.value(), this);
				table->setHeaderLabels(m_pHeaderLabels);
				m_pTabWidget->addTab(table,iter.key());
				m_pTabMap.insert(iter.key(), table);
			}

			QMapIterator<QString, SoftData> dataMapIter(iter.value());
			while(dataMapIter.hasNext())
			{
				dataMapIter.next();
				m_valueMap.insert(dataMapIter.key(), iter.key());
			}
		}
	}

	QMapIterator<QString, CTableWidget*> iterTab(m_pTabMap);
	while(iterTab.hasNext())
	{
		iterTab.next();
		if(!data.contains(iterTab.key()))
		{
			CTableWidget* w =  iterTab.value();
			w->clearUninstallData();
			if(w->rowCount() <= 0)
			{
				delete w;
				w = nullptr;
				m_pTabMap.remove(iterTab.key());
			}
		}
	}

	m_pTabWidget->setCurrentIndex(0);
}

void CSoftTabWidget::setTab(const QString& label)
{
	CTableWidget* w = m_pTabMap.value(label);
	if(w)
	{
		m_pTabWidget->setCurrentIndex(m_pTabWidget->indexOf(w));
	}
}
