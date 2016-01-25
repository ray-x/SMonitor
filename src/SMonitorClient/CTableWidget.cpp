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

#include "CTableWidget.h"
#include "CNoFocusDelegate.h"
#include "CSoftManager.h"
#include "CClientLog.h"
#include "CMisc.h"

#include <QFile>
#include <QDir>
#include <QTimerEvent>
#include <QHeaderView>
#include <QToolButton>
#include <QTableWidgetItem>
#include <QDebug>
#include <QDataStream>
#include <QMessageBox>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <CHTTPDownloader.h>

CTableWidget::CTableWidget(const QMap<QString, SoftData>& data, QWidget *parent)
	: QTableWidget(parent)
	, m_curTimerType(0)
{
	m_signalMapper = new QSignalMapper(this);
	setObjectName("Table");
	setColumnCount(5);
	setContentsMargins(10, 10, 10, 10);
	setAlternatingRowColors(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setItemDelegate(new CNoFocusDelegate(this));

	horizontalHeader()->setStretchLastSection(true);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);//;
	horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	//connect(horizontalHeader(), &QHeaderView::sectionResized, this, &CTableWidget::headerGemetoryChanged);
	updateTable(data);
	connect(SoftManager, &CSoftManager::downloadFinished, this, &CTableWidget::downloadFinish);
	connect(SoftManager, &CSoftManager::downloadErrorOccured, this, &CTableWidget::downloadErrorOccur);
	connect(SoftManager, &CSoftManager::downloadPercent, this, &CTableWidget::downloadPercent);
	connect(SoftManager, &CSoftManager::redownloadCountdown, this, &CTableWidget::redownloadCountdown, Qt::DirectConnection);
	connect(SoftManager, &CSoftManager::InstallFinished, this, &CTableWidget::InstallFinish);
	//connect(SoftManager, &CSoftManager::decompressErrorOccured, this, &CTableWidget::decompressError);
	connect(SoftManager, &CSoftManager::redownloadErrorOccered, this, &CTableWidget::redownloadFailed);
	connect(SoftManager, &CSoftManager::InstallCancled, this, &CTableWidget::InstallCancled);
	connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(Cancled(QString)));

	setColumnWidth(0, 170);
	setColumnWidth(1, 80);
	setColumnWidth(2, 80);
	setColumnWidth(3, 170);
}

CTableWidget::~CTableWidget()
{

}

#define SETITEM(row, column, text, upgrade)								\
	{																	\
		QTableWidgetItem* item = new QTableWidgetItem(text);			\
		item->setTextAlignment(Qt::AlignCenter);                        \
		if(upgrade) {item->setForeground(Qt::red);}						\
		item->setToolTip(text);											\
		setItem(row, column, item);										\
	}

void CTableWidget::setHeaderLabels(const QStringList& headerLabels)
{
	m_pHeaderLabels = headerLabels;
	setHorizontalHeaderLabels(headerLabels);
}


void CTableWidget::showIndependentItem(QString strKey, QString version, bool bShow)
{
	if(bShow)
	{
		QMutexLocker locker(&m_TableMutex);
		SoftData data;
		data.name = strKey;
		data.version = version;
		data.state = SMonitor::Upgrade;
		data.category = INDEPENDENT;
		m_IndependentDataMap.insert(strKey, data);

		if(!m_pTableData.contains(strKey))
		{
			m_pTableData.insert(strKey, data);

			if(m_pTableData.count() > 0)
			{
				setRowCount(m_pTableData.count());
				setCellData(data, m_pTableData.count() - 1);
			}
		}
	}
	else
	{
		m_IndependentDataMap.remove(strKey);
		removeItem(strKey);
	}
}

int CTableWidget::getIndepentDataCount()
{
	QMutexLocker locker(&m_TableMutex);
	return m_IndependentDataMap.count();
}

int CTableWidget::FindItemRow(const QString& key)
{
	int rowCount = this->rowCount();
	for (int i = 0; i < rowCount; i++)
	{
		if(this->item(i, 0) && key == this->item(i, 0)->text())
		{
			qDebug() << key << "~~~~~~~~~" << i;
			return i;
		}
	}
	return -1;
}

void CTableWidget::setCellData(const SoftData& sData, int row)
{
	if(sData.state == SMonitor::UnInstall)
	{
		SETITEM(row, 0, sData.name, false);
		SETITEM(row, 1, sData.version, false);
		SETITEM(row, 2, sData.brand, false);
		SETITEM(row, 3, "", false);
		QToolButton* tbtnUpgrade = new QToolButton(this);
		tbtnUpgrade->setObjectName("tbtn_Opr");
		tbtnUpgrade->setText(QString::fromLocal8Bit("安装"));
		tbtnUpgrade->setMaximumWidth(70);
		connect(tbtnUpgrade, &QToolButton::clicked, this, &CTableWidget::downloadSoftware);
		setCellWidget(row, 4, tbtnUpgrade);
	}
	else if(sData.state == SMonitor::Normal){
		SETITEM(row, 0, sData.name, false);
		SETITEM(row, 1, sData.version, false);
		SETITEM(row, 2, sData.brand, false);
		SETITEM(row, 3, sData.location, false);
		SETITEM(row, 4, QString::fromLocal8Bit("已是最新版本"), false);
	}
	else if(sData.state == SMonitor::Upgrade)
	{
		SETITEM(row, 0, sData.name, true);
		SETITEM(row, 1, sData.version, true);
		SETITEM(row, 2, sData.brand, true);
		SETITEM(row, 3, sData.location, true);
		QToolButton* tbtnUpgrade = new QToolButton(this);
		tbtnUpgrade->setObjectName("tbtn_Opr");
		tbtnUpgrade->setText(QString::fromLocal8Bit("升级"));
		tbtnUpgrade->setMaximumWidth(70);
		connect(tbtnUpgrade, &QToolButton::clicked, this, &CTableWidget::downloadSoftware);
		setCellWidget(row, 4, tbtnUpgrade);
	}
}

void CTableWidget::clearUninstallData()
{
	QMutexLocker locker(&m_TableMutex);
	clear();
	setHorizontalHeaderLabels(m_pHeaderLabels);

	_clearUninstallData();

	QMapIterator<QString, SoftData> iterIndependent(m_IndependentDataMap);
	while(iterIndependent.hasNext())
	{
		iterIndependent.next();
		m_pTableData.insert(iterIndependent.key(), iterIndependent.value());
	}

	setRowCount(m_pTableData.count());

	QMapIterator<QString, SoftData> iter2(m_pTableData);
	int row = 0;
	while(iter2.hasNext())
	{
		iter2.next();
		SoftData sData = iter2.value();
		sData.name = sData.name.split("##").first();
		setCellData(sData, row);
		//m_pRowKey.insert(row, sData.name);
		row++;
	}
}

void CTableWidget::_clearUninstallData()
{
	QMapIterator<QString, SoftData> iter(m_pTableData);
	while(iter.hasNext())
	{
		iter.next();
		if(iter.value().state == SMonitor::UnInstall)
		{
			m_pTableData.remove(iter.key());
		}
	}
}

void CTableWidget::ProgressWidget(int row, const QString& key)
{
	//设置进度条
	//QProgressBar* progressBar = new QProgressBar(this);
	//progressBar->setRange(0, 100);
	//progressBar->setValue(0.00);

	QWidget* w = cellWidget(row, 4);
	if(w){
		w->hide();
		w->deleteLater();
	}

	QWidget* widget = new QWidget(this);
	//设置进度条
	QProgressBar* progressBar = new QProgressBar(widget);
	progressBar->setRange(0, 100);
	progressBar->setValue(0.00);
	progressBar->setMinimumHeight(26);

	QToolButton* tbtn = new QToolButton(widget);
	tbtn->setText(QString::fromLocal8Bit("取消"));
	tbtn->setObjectName("tbtn_Opr");
	tbtn->setMinimumHeight(26);
	connect(tbtn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
	m_signalMapper->setMapping(tbtn, key);

	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->setMargin(0);
	layout->setSpacing(3);
	layout->addWidget(progressBar);
	layout->addWidget(tbtn);

	setCellWidget(row, 4, widget);
}

QProgressBar* CTableWidget::FindProgressBar(const QWidget* parent)
{
	QProgressBar* progressBar = nullptr;
	if(parent){
		foreach(auto var , parent->children())
		{
			progressBar = qobject_cast<QProgressBar*> (var);
			if(progressBar)
				break;
		}
	}

	return progressBar;
}

QToolButton* CTableWidget::FindToolButton(const QWidget* parent)
{
	QToolButton* tbtn = nullptr;
	if(parent){
		foreach(auto var , parent->children())
		{
			tbtn = qobject_cast<QToolButton*> (var);
			if(tbtn)
				break;
		}
	}
	return tbtn;
}

void CTableWidget::removeItem(const QString& key)
{
	QMutexLocker locker(&m_TableMutex);
	int row = FindItemRow(key);
	if(row >= 0)
	{
		this->removeRow(row);
		m_pTableData.remove(key);
	}
}

void CTableWidget::updateTable(const QMap<QString, SoftData>& data)
{
	QMutexLocker locker(&m_TableMutex);
	clear();
	setHorizontalHeaderLabels(m_pHeaderLabels);

	_clearUninstallData();

	QMapIterator<QString, SoftData> iter(data);
	while(iter.hasNext())
	{
		iter.next();

		//如果包含则更新数据
		QString strKey = iter.key().split("##").first();
		m_pTableData.insert(strKey, iter.value());
	}

	QMapIterator<QString, SoftData> iterIndependent(m_IndependentDataMap);
	while(iterIndependent.hasNext())
	{
		iterIndependent.next();
		m_pTableData.insert(iterIndependent.key(), iterIndependent.value());
	}
	
	setRowCount(m_pTableData.count());

	QMapIterator<QString, SoftData> iter2(m_pTableData);
	int row = 0;
	while(iter2.hasNext())
	{
		iter2.next();
		SoftData sData = iter2.value();
		sData.name = sData.name.split("##").first();
		setCellData(sData, row);
		//m_pRowKey.insert(row, sData.name);
		row++;
	}
}

void CTableWidget::downloadSoftware()
{
	QToolButton* tbtn = qobject_cast<QToolButton*> (sender());
	int x = tbtn->frameGeometry().x();
	int y = tbtn->frameGeometry().y();
	QModelIndex index = this->indexAt(QPoint(x,y));
	int row = index.row();
	QString key = this->item(row, 0)->text();

	ProgressWidget(row, key);

	const SoftData& data = m_pTableData.value(key);
	QString name = "";

	if(data.category == QString::fromLocal8Bit("驱动"))
		name = data.name.split("##").first();
	else
		name = data.name;

	emit SoftManager->SoftWareInstall(name);

	//SoftData data = m_pTableData.value(m_pRowKey.value(row));
	//if(!data.name.isEmpty())
	//{
	//	Task task;
	//	task.key = data.name.split("##").first();
	//	task.srcPath = data.url;
	//	task.desPath = data.location; //安装目录
	//	task.version = data.version;
	//	task.category = data.category;
	//	task.mainExe = data.mainExe;
	//	task.state = data.state;
	//	SoftManager->AddTask(task);
	//}
}

void CTableWidget::downloadErrorOccur(const QString& key, const QString& msg)
{
	int row = FindItemRow(key);
	if(row < 0) return;

	ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1]下载失败 错误码[%2] 稍后自动重启下载").arg(key).arg(msg));

	QWidget* widget = cellWidget(row, 4);
	if(widget)
	{
		QString format = QString::fromLocal8Bit("下载失败,稍后将自动重启下载");
		QProgressBar* progressBar = FindProgressBar(widget);
		if(progressBar)
		{
			progressBar->setFormat(format);
			progressBar->setValue(0);
		}
		else
		{
			//被动设置进度条
			ProgressWidget(row, key);

			QProgressBar* progressBar = FindProgressBar(cellWidget(row, 4));
			progressBar->setFormat(format);
			progressBar->setValue(0);
		}

		//bool bProgressed = false;
		//QProgressBar* progressBar = nullptr;
		//foreach(auto var , widget->children())
		//{
		//	progressBar = qobject_cast<QProgressBar*> (var);
		//	if(!progressBar)
		//	{
		//		continue;
		//		//被动设置进度条
		//		progressBar = new QProgressBar(this);
		//		progressBar->setRange(0, 100);
		//		progressBar->setValue(0.00);
		//		setCellWidget(row, 4, progressBar);
		//	}
		//	else
		//	{
		//		bProgressed = true;
		//	}
		//}
		//if(bProgressed){
		//	QString format = QString::fromLocal8Bit("下载失败,稍后将自动重启下载");
		//	progressBar->setFormat(format);
		//	progressBar->setValue(0);	
		//}
		//else
		//{
		//	QString format = QString::fromLocal8Bit("下载失败,稍后将自动重启下载");
		//	progressBar->setFormat(format);
		//	progressBar->setValue(0);	
		//}
	}
}

void CTableWidget::decompressError(const QString& key)
{
	int row = FindItemRow(key);
	if(row < 0) return;
	//QProgressBar* progressBar = qobject_cast<QProgressBar*> (cellWidget(row, 4));
	//if(!progressBar)
	//{
	//	//被动设置进度条
	//	progressBar = new QProgressBar(this);
	//	progressBar->setRange(0, 100);
	//	progressBar->setValue(0.00);
	//	setCellWidget(row, 4, progressBar);
	//}

	//QString format = QString::fromLocal8Bit("安装失败,稍后将自动重新安装...");
	//progressBar->setFormat(format);
	//progressBar->setValue(0);	
	QWidget* widget = cellWidget(row, 4);
	if(widget)
	{
		QString format = QString::fromLocal8Bit("安装失败,稍后将自动重新安装...");
		
		QToolButton* tbtn = FindToolButton(cellWidget(row, 4));
		if(tbtn)
			tbtn->hide();

		QProgressBar* progressBar = FindProgressBar(widget);
		if(progressBar)
		{
			progressBar->setFormat(format);
			progressBar->setValue(0);
		}
		else
		{
			//被动设置进度条
			ProgressWidget(row, key);

			QProgressBar* progressBar = FindProgressBar(cellWidget(row, 4));
			progressBar->setFormat(format);
			progressBar->setValue(0);

			QToolButton* tbtn = FindToolButton(cellWidget(row, 4));
			if(tbtn)
				tbtn->hide();
		}
		//foreach(auto var , widget->children())
		//{
		//	QProgressBar* progressBar = qobject_cast<QProgressBar*> (var);
		//	if(!progressBar)
		//	{
		//		continue;
		//		//被动设置进度条
		//		progressBar = new QProgressBar(this);
		//		progressBar->setRange(0, 100);
		//		progressBar->setValue(0.00);
		//		setCellWidget(row, 4, progressBar);
		//	}

		//	QString format = QString::fromLocal8Bit("安装失败,稍后将自动重新安装...");
		//	progressBar->setFormat(format);
		//	progressBar->setValue(0);	
		//}
	}
}

void CTableWidget::downloadPercent(const QString& key, long ldownBytes, int totalBytes)
{
	//RC->addRc(key, ldownBytes, totalBytes);
	int row = FindItemRow(key);
	if(row < 0) return;
	QWidget* widget = cellWidget(row, 4);
	if(widget)
	{
		QString down = QString::number(((float)ldownBytes / totalBytes) * 100.00, 'f', 2);
		QString format = QString("%1%").arg(down);
		QProgressBar* progressBar = FindProgressBar(widget);
		if(progressBar)
		{
			progressBar->setFormat(format);
			progressBar->setValue(down.toFloat());
		}
		else
		{
			//被动设置进度条
			ProgressWidget(row, key);

			QProgressBar* progressBar = FindProgressBar(cellWidget(row, 4));
			progressBar->setFormat(format);
			progressBar->setValue(down.toFloat());
		}

		//foreach(auto var , widget->children())
		//{
		//	QProgressBar* progressBar = qobject_cast<QProgressBar*> (var);
		//	if(!progressBar)
		//	{
		//		continue;
		//		////被动设置进度条
		//		//progressBar = new QProgressBar(this);
		//		//progressBar->setRange(0, 100);
		//		//progressBar->setValue(0.00);
		//		//setCellWidget(row, 4, progressBar);
		//	}

		//	QString down = QString::number(((float)ldownBytes / totalBytes) * 100.00, 'f', 2);
		//	QString format = QString("%1%").arg(down);
		//	progressBar->setFormat(format);
		//	progressBar->setValue(down.toFloat());
		//}
	}
}

void CTableWidget::downloadFinish(const QString& key)
{
	/*QMapIterator<int, QString> iter(m_pRowKey);
	while(iter.hasNext())
	{
	iter.next();
	QString str = iter.value();
	int n = 2;
	}*/

	int row = FindItemRow(key);
	if(row < 0) return;

	QWidget* widget = cellWidget(row, 4);
	if(widget)
	{
		QString format = QString::fromLocal8Bit("正在执行自动安装...");
		QToolButton* tbtn = FindToolButton(cellWidget(row, 4));
		if(tbtn)
			tbtn->hide();

		QProgressBar* progressBar = FindProgressBar(widget);
		if(progressBar)
		{
			progressBar->setFormat(format);
			progressBar->setValue(99);
		}
		else
		{
			//被动设置进度条
			ProgressWidget(row, key);
			QProgressBar* progressBar = FindProgressBar(cellWidget(row, 4));
			progressBar->setFormat(format);
			progressBar->setValue(99);

			QToolButton* tbtn = FindToolButton(cellWidget(row, 4));
			if(tbtn)
				tbtn->hide();
		}

		//foreach(auto var , widget->children())
		//{
		//	QProgressBar* progressBar = qobject_cast<QProgressBar*> (var);
		//	if(!progressBar)
		//	{
		//		continue;
		//		//被动设置进度条
		//		progressBar = new QProgressBar(this);
		//		progressBar->setRange(0, 100);
		//		progressBar->setValue(0.00);
		//		setCellWidget(row, 4, progressBar);
		//	}

		//	if(progressBar)
		//	{
		//		progressBar->setFormat(QString::fromLocal8Bit("正在执行自动安装..."));
		//		progressBar->setValue(99.999);
		//	}
		//}
	}
}

void CTableWidget::redownloadFailed(const QString& key, int state)
{
	int row = FindItemRow(key);
	if(row < 0) return;

	ClientLogger->AddLog(QString::fromLocal8Bit("自动下载/安装失败，不再重试"));
	qDebug() << QString::fromLocal8Bit("自动下载失败，设置手动下载按钮");
	if(state == SMonitor::Upgrade)
	{
		QToolButton* tbtnUpgrade = new QToolButton(this);
		tbtnUpgrade->setObjectName("tbtn_Opr");
		tbtnUpgrade->setText(QString::fromLocal8Bit("升级"));
		tbtnUpgrade->setMaximumWidth(70);
		//这里需要注意，当为安装或者升级按钮被设置之后，cell里边是没有被设置item的，所以需要判断item是否为空
		if(this->item(row, 4)){
			this->item(row, 4)->setText("");
		}
		connect(tbtnUpgrade, &QToolButton::clicked, this, &CTableWidget::downloadSoftware);
		setCellWidget(row, 4, tbtnUpgrade);
	}
	else
	{
		/*setCellWidget(row, 4, nullptr);
		SETITEM(row, 4, QString::fromLocal8Bit("未安装"), false);*/

		QToolButton* tbtnUpgrade = new QToolButton(this);
		tbtnUpgrade->setObjectName("tbtn_Opr");
		tbtnUpgrade->setText(QString::fromLocal8Bit("安装"));
		tbtnUpgrade->setMaximumWidth(70);
		connect(tbtnUpgrade, &QToolButton::clicked, this, &CTableWidget::downloadSoftware);
		setCellWidget(row, 4, tbtnUpgrade);
// 		QTableWidgetItem* item = new QTableWidgetItem(QString::fromLocal8Bit("已升级至最新版本"));
// 		item->setTextAlignment(Qt::AlignCenter);
// 		item->setForeground(Qt::black);
// 		setItem(row, 4, item);
	}
	
	//此处解决按钮不能居中的问题
	this->hide();
	this->show();
	setCurrentItem(this->item(row, 0));
}

void CTableWidget::InstallCancled(const QString&key, int state)
{
	int row = FindItemRow(key);
	if(row < 0) return;

	if(state == SMonitor::Upgrade)
	{
		QToolButton* tbtnUpgrade = new QToolButton(this);
		tbtnUpgrade->setObjectName("tbtn_Opr");
		tbtnUpgrade->setText(QString::fromLocal8Bit("升级"));
		tbtnUpgrade->setMaximumWidth(70);
		if(this->item(row, 4)){
			this->item(row, 4)->setText("");
		}
		connect(tbtnUpgrade, &QToolButton::clicked, this, &CTableWidget::downloadSoftware);
		setCellWidget(row, 4, tbtnUpgrade);
	}
	else
	{
		QToolButton* tbtnUpgrade = new QToolButton(this);
		tbtnUpgrade->setObjectName("tbtn_Opr");
		tbtnUpgrade->setText(QString::fromLocal8Bit("安装"));
		tbtnUpgrade->setMaximumWidth(70);
		connect(tbtnUpgrade, &QToolButton::clicked, this, &CTableWidget::downloadSoftware);
		setCellWidget(row, 4, tbtnUpgrade);
	}
	
	//此处解决按钮不能居中的问题
	this->hide();
	this->show();
	setCurrentItem(this->item(row, 0));

	QMapIterator<int, QPair<int, QString> > iter(m_pTimerMap);
	while (iter.hasNext())
	{
		iter.next();
		if(iter.value().second == key)
		{
			killTimer(iter.key());
			qDebug() << QString::fromLocal8Bit("取消下载, kill timer:%1").arg(iter.key());
			m_pTimerMap.remove(iter.key());
		}
	}
}

void CTableWidget::Cancled(const QString& key)
{
	int row = FindItemRow(key);
	if(row < 0) return;

	if(m_pTableData.contains(key))
	{
		//SoftManager->deleteTask(key, false);
		SoftManager->StopDownload(key);
		emit SoftManager->InstallCancled(key, m_pTableData.value(key).state);
	}
}

void CTableWidget::InstallFinish(const QString& key)
{
	int row = FindItemRow(key);
	if(row < 0) return;
	setCellWidget(row, 4, nullptr);
	QTableWidgetItem* item = new QTableWidgetItem(QString::fromLocal8Bit("已升级至最新版本"));
	item->setTextAlignment(Qt::AlignCenter);
	item->setForeground(Qt::black);
	setItem(row, 4, item);

	if(m_pTableData.contains(key)){
		QTableWidgetItem* item = this->item(row, 3);
		item->setText(m_pTableData.value(key).location);
	}

	this->item(row, 0)->setForeground(Qt::black);
	this->item(row, 1)->setForeground(Qt::black);
	this->item(row, 2)->setForeground(Qt::black);
	this->item(row, 3)->setForeground(Qt::black);
}

void CTableWidget::redownloadCountdown(int type, const QString& key, int duration)
{
	if(m_pTableData.contains(key))
	{
		//每秒倒计时
		m_curTimerType = type;
		int id = startTimer(1000);
		qDebug() << QString::fromLocal8Bit("启动定时器") << id;
		m_pTimerMap.insert(id, qMakePair(duration, key));
	}	
}

void CTableWidget::timerEvent(QTimerEvent *event)
{
	int id = event->timerId();
	if(m_pTimerMap.contains(id))
	{
		int duration = m_pTimerMap.value(id).first;
		QString key = m_pTimerMap.value(id).second;

		if(duration != 0)
		{
			int row = FindItemRow(key);
			if(row < 0) return;
			QWidget* widget = cellWidget(row, 4);
			if(widget)
			{
				foreach(auto var , widget->children())
				{
					QProgressBar* progressBar = qobject_cast<QProgressBar*> (var);
					if(progressBar)
					{
						QString format;
						if(m_curTimerType == 0)
							format = QString::fromLocal8Bit("连接中断,%1秒之后尝试重新连接...").arg(--duration);
						else 
							format = QString::fromLocal8Bit("安装失败,%1秒之后尝试重新安装...").arg(--duration);

						progressBar->setFormat(format);
						m_pTimerMap.insert(id, qMakePair(duration, key));
					}
				}
			}
		}
		else
		{
			m_pTimerMap.remove(id);
			killTimer(id);
		}
		event->accept();
	}
	else
	{
		QTableWidget::timerEvent(event);
	}
}
