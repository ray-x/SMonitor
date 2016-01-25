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

#include "CSoftUpdateWidget.h"
#include "CTableWidget.h"
#include "CCategoryFactory.h"
#include "CSoftManager.h"

#include <QLabel>
#include <QVBoxLayout>

CSoftUpdateWidget::CSoftUpdateWidget(QWidget *parent)
	: QWidget(parent)
{
	QLabel *titleLabel = new QLabel(QString::fromLocal8Bit("可升级软件列表"),this);

	m_tableWidget = new CTableWidget(QMap<QString, SoftData>(), this);
	m_tableWidget->setObjectName("updatetable");
	m_tableWidget->setHeaderLabels(QStringList() << QString::fromLocal8Bit("软件名称")
		<< QString::fromLocal8Bit("版本号") << QString::fromLocal8Bit("品牌")
		<< QString::fromLocal8Bit("安装位置") << QString::fromLocal8Bit("升级"));

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(m_tableWidget);

	connect(CategoryFactory, &CCategoryFactory::dataChangedNotify, this, &CSoftUpdateWidget::setUpgradeData);
	connect(SoftManager, &CSoftManager::InstallFinished, this, &CSoftUpdateWidget::InstallFinished);
}

CSoftUpdateWidget::~CSoftUpdateWidget()
{
	delete m_tableWidget;
	m_tableWidget = nullptr;
}

void CSoftUpdateWidget::InstallFinished(const QString& key)
{
	m_tableWidget->removeItem(key);
	emit RowCountChanged(m_tableWidget->rowCount());
	changeTrayIconState();
}

void CSoftUpdateWidget::changeTrayIconState()
{
	if(m_tableWidget->rowCount() <= 0)
	{
		emit upgradeNotify(SMonitor::Normal);
	}
	else
	{
		emit upgradeNotify(SMonitor::Upgrade);
	}
}

void CSoftUpdateWidget::showIndependentItem(QString strKey, QString version, bool bShow)
{
	m_tableWidget->showIndependentItem(strKey, version, bShow);
	emit RowCountChanged(m_tableWidget->rowCount());
	changeTrayIconState();
}

void CSoftUpdateWidget::setUpgradeData(const QMap<QString, QMap<QString, SoftData> >& data)
{
	//类别, <软件名称，软件数据>
	QMap<QString, SoftData> upgrade;
	QMapIterator<QString, QMap<QString, SoftData> > iter(data);
	while(iter.hasNext())
	{
		iter.next();
		QMapIterator<QString, SoftData> iter2(iter.value());
		while(iter2.hasNext())
		{
			iter2.next();
			if(iter2.value().state == SMonitor::Upgrade)
			{
				upgrade.insert(iter2.key(), iter2.value());
			}
		}
	}

	m_tableWidget->updateTable(upgrade);

	emit RowCountChanged(m_tableWidget->rowCount());
	changeTrayIconState();
}

