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
#include "CSoftUpgradeWidget.h"
#include "CNoFocusDelegate.h"
#include "CItemDelegate.h"
#include "Define.h"
#include "databasemanager.h"
#include "Markup.h"
#include "CMisc.h"
#include "CPasswdConfig.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDate>
#include <QLineEdit>
#include <QModelIndex>
#include <QKeyEvent>
#include <QDebug>

CSoftUpgradeWidget::CSoftUpgradeWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	m_brandList.append(QStringList() << QString::fromLocal8Bit("Brand1")
		<< QString::fromLocal8Bit("Brand2"));
	comboBox_brand->addItems(m_brandList);

	m_itemDelegate = new CItemDelegate(this);
	connect(this, &CSoftUpgradeWidget::brandAdded, m_itemDelegate, &CItemDelegate::addBrand);
	connect(this, &CSoftUpgradeWidget::brandChanged, m_itemDelegate, &CItemDelegate::resetBrand);

	tableWidget->setItemDelegate(m_itemDelegate);
	initWidget();

	tableWidget->setColumnWidth(0, 180);
	tableWidget->setColumnWidth(1, 100);
	tableWidget->setColumnWidth(2, 300);
	tableWidget->setColumnWidth(3, 100);
	tableWidget->setColumnWidth(4, 100);
	tableWidget->setColumnWidth(5, 300);
	

	connect(tbtn_Passwd, &QToolButton::clicked, this, &CSoftUpgradeWidget::PasswdConfig);
	connect(tbtn_add, &QToolButton::clicked, this, &CSoftUpgradeWidget::addItem);
	connect(tbtn_delete, &QToolButton::clicked, this, &CSoftUpgradeWidget::removeItem);
	connect(tableWidget, &QTableWidget::itemChanged, this, &CSoftUpgradeWidget::itemChanged);
	connect(btn_Save, &QPushButton::clicked, this, &CSoftUpgradeWidget::SaveContentToFile);
	connect(tbtn_addbrand, &QToolButton::clicked, this, &CSoftUpgradeWidget::addSubBrand);
	//connect(lineEdit_brand, &QLineEdit::editingFinished, this, &CSoftUpgradeWidget::addSubBrand);
	connect(comboBox_brand, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentBrandChanged(QString)));
	connect(btn_SaveData, &QPushButton::clicked, this, &CSoftUpgradeWidget::SaveContentToDatabase);
	connect(btn_Init, &QPushButton::clicked, this, &CSoftUpgradeWidget::initWidget);
	
}

CSoftUpgradeWidget::~CSoftUpgradeWidget()
{

}

void CSoftUpgradeWidget::initWidget()
{
	ClearBrands();

	int count = tableWidget->rowCount();
	for(int i = 0; i < count; i++)
	{
		for (int j = 0; j < tableWidget->columnCount(); j++)
		{
			delete tableWidget->takeItem(i, j);
		}
	}
	tableWidget->setRowCount(0);

	m_deleteSoftwareInfoVec.clear();
	m_changedRowVec.clear();

	{
		QMutexLocker locker(&m_softwareInfoMutex);
		m_softwareInfoVector.clear();
		DatabaseManager::Instance()->ReadAllSoftwareInfo(m_softwareInfoVector);
	}
	
	for (int i = 0; i < m_softwareInfoVector.count(); i++)
		insertItem(m_softwareInfoVector.at(i));
}

SoftwareinfoMap CSoftUpgradeWidget::GetSoftwareInfoMap(bool bRemoveNetPage)
{
	QMutexLocker locker(&m_softwareInfoMutex);
	SoftwareinfoMap softwareInfoMap;
	int nCount = m_softwareInfoVector.count();
	for (int i = 0; i < nCount; i++)
	{
		const SoftwareInfo& softwareInfo = m_softwareInfoVector[i];
		if(bRemoveNetPage && softwareInfo.category == QString::fromLocal8Bit("网页"))
			continue;

		softwareInfoMap.insert(softwareInfo.name, softwareInfo);
	}

	return softwareInfoMap;
}

void CSoftUpgradeWidget::addItem()
{
	int rowCount = tableWidget->rowCount();
	tableWidget->setRowCount(rowCount + 1);
	int row = rowCount;

	for (int j = 0; j < 7; j++)
	{
		QString str = "";
		switch(j)
		{
		case 0:
			str = getNextDistinctChildName(QString::fromLocal8Bit("软件名称"), 0, row);
			break;
		case 1:
			str = QDate::currentDate().toString("yyyyMMdd");
			break;
		case 3:
			str = QString::fromLocal8Bit("CompanyName");
			break;
		case 4:
			str = QString::fromLocal8Bit("ERP");
			break;
		default:
			break;
		}
		prepareItemChanged();
		QTableWidgetItem* item = new QTableWidgetItem;	
		tableWidget->setItem(row, j, item);	
		tableWidget->setRowHeight(row, 30);
		item->setText(str);
		item->setForeground(QColor(254, 102, 24));
		item->setTextAlignment(Qt::AlignCenter);
		endItemChanged();
	}
	tableWidget->setCurrentCell(row, 0);
	if(!m_changedRowVec.contains(row))
		m_changedRowVec.push_back(row);
	tableWidget->editItem(tableWidget->item(row, 0));

	/*SoftwareInfo info;
	getSoftInfo(row, info);
	DatabaseManager::Instance()->AddSoftwareInfo(info);*/
}

void CSoftUpgradeWidget::removeItem()
{
	int row = tableWidget->currentRow();
	if(row >= 0)
	{
		SoftwareInfo info;
		getSoftInfo(row ,info);

		if(hasInstall(info))
		{//只能删除已经保存的软件列表里的软件
			m_deleteSoftwareInfoVec.push_back(info);
		}

		//不是已安装软件，说明是
		for (int i = 0; i < m_changedRowVec.count(); i++)
		{
			if(row == m_changedRowVec.at(i))
			{
				m_changedRowVec.removeAt(i);
				break;
			}
		}
		
		tableWidget->removeRow(row);
		tableWidget->setCurrentCell(row - 1, 0);

		/*if(DatabaseManager::Instance()->RemoveSoftware(tableWidget->item(row, 0)->text())){
		emit itemRemoved(tableWidget->item(row, 0)->text(), row);
		tableWidget->removeRow(row);
		tableWidget->setCurrentCell(row - 1, 0);

		if(m_changedRowVec.contains(row))
		m_changedRowVec.remove(m_changedRowVec.indexOf(row));
		}
		else
		{
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
		QString::fromLocal8Bit("删除数据失败，请稍后重试"),
		QString::fromLocal8Bit("确定"));
		}*/
	}
}

void CSoftUpgradeWidget::itemChanged(QTableWidgetItem* item)
{
	if(item)
	{
		bool bChanged = true;
		prepareItemChanged();
		if(item->column() == 0)
		{
			QString text = item->text();
			if(findItemText(item))
			{
				QString suggestedName = getNextDistinctChildName(text, 0, item->row());
				int value = QMessageBox::warning(this, QString::fromLocal8Bit("重命名"), 
					QString::fromLocal8Bit("<h6 style=\" color:blue;\">将 \"%1\" 重命名为 \"%2\"</h6>\
										   <p>已包含同名项.</p>").arg(text).arg(suggestedName), 
										   QString::fromLocal8Bit("确定"));
				switch(value)
				{
				case 0:
					item->setText(suggestedName);
					break;
				default:
					break;
				}
			}
		}
		else if(item->column() == 4)
		{
			QTableWidgetItem* item3 = tableWidget->item(item->row(), 3);
			if(item->text() == QString::fromLocal8Bit("驱动"))
			{
				//QAbstractItemModel* model = tableWidget->model();
				//QModelIndex index = model->index(item3->row(), item3->column());
				//model->setData(index, QString::fromLocal8Bit("爱普生"), Qt::DisplayRole);
				//m_brandList.append(QString::fromLocal8Bit("爱普生"));
				//comboBox_brand->addItem(QString::fromLocal8Bit("爱普生"));
				//item3->setData(Qt::UserRole + 1, 1);

				QTableWidgetItem* item5 = tableWidget->item(item->row(), 5);
				item5->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item5->setBackground(QColor(230, 230, 230));
				item5->setText("");

				QTableWidgetItem* item6 = tableWidget->item(item->row(), 6);
				item6->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item6->setBackground(QColor(230, 230, 230));
				item6->setText("");

				QTableWidgetItem* item1 = tableWidget->item(item->row(), 1);
				item1->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
				item1->setBackground(Qt::transparent);
				item1->setText(QDateTime::currentDateTime().toString("yyyyMMdd"));
			}
			else if(item->text() == QString::fromLocal8Bit("网页"))
			{
				/*QAbstractItemModel* model = tableWidget->model();
				QModelIndex index = model->index(item3->row(), item3->column());
				item3->setData(Qt::UserRole + 1, 1);*/
				QTableWidgetItem* item5 = tableWidget->item(item->row(), 5);
				item5->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item5->setBackground(QColor(230, 230, 230));
				item5->setText("");

				QTableWidgetItem* item1 = tableWidget->item(item->row(), 1);
				item1->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item1->setBackground(QColor(230, 230, 230));
				item1->setText("");

				QTableWidgetItem* item6 = tableWidget->item(item->row(), 6);
				item6->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item6->setBackground(QColor(230, 230, 230));
				item6->setText("");

				/*m_brandList.removeAll(QString::fromLocal8Bit("爱普生"));
				comboBox_brand->removeItem(comboBox_brand->findText(QString::fromLocal8Bit("爱普生")));*/
			}
			else
			{
				QAbstractItemModel* model = tableWidget->model();
				QModelIndex index = model->index(item3->row(), item3->column());
				model->setData(index, QString::fromLocal8Bit("CompanyName"), Qt::DisplayRole);
				item3->setData(Qt::UserRole + 1, 0);

				QTableWidgetItem* item5 = tableWidget->item(item->row(), 5);
				item5->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
				item5->setBackground(Qt::transparent);

				QTableWidgetItem* item1 = tableWidget->item(item->row(), 1);
				item1->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
				item1->setBackground(Qt::transparent);
				item1->setText(QDateTime::currentDateTime().toString("yyyyMMdd"));

				QTableWidgetItem* item6 = tableWidget->item(item->row(), 6);
				item6->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
				item6->setBackground(Qt::transparent);
				item6->setText("");

				//m_brandList.removeAll(QString::fromLocal8Bit("爱普生"));
				//comboBox_brand->removeItem(comboBox_brand->findText(QString::fromLocal8Bit("爱普生")));
			}
		}
		else if(item->column() == 5)
		{
			/*if(findItemText(item)){
			QString suggestedName = getNextDistinctChildName(item->text(), 5, item->row());
			int value = QMessageBox::warning(this, QString::fromLocal8Bit("重设路劲"), 
			QString::fromLocal8Bit("<h6 style=\" color:blue;\">将路劲 \"%1\" 重设为 \"%2\"</h6>\
			<p>已包含相同路劲.</p>").arg(item->text()).arg(suggestedName), 
			QString::fromLocal8Bit("确定"));
			item->setText(suggestedName);
			}*/
			QRegExp regExp = QRegExp("([A-Za-z]:)((\\\\|/|//)([^?*|<>]*))+");
			if(!regExp.exactMatch(item->text()))
			{
				QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
					QString::fromLocal8Bit("程序路径不合法，请确认后重试"),
					QString::fromLocal8Bit("确定"));
				bChanged = false;
				item->setText("");
			}
		}
		else if(item->column() == 2)
		{
			//((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:www\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\??(?:[\\-\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)
			//QRegExp regExp("((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\/\\/$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:www\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\??(?:[\\-\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)");
			QRegExp regExp("(((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:www\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)(:[0-9]{1,4})?)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\??(?:[\\-\\\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)");
			if(!regExp.exactMatch(item->text()))
			{
				QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
					QString::fromLocal8Bit("Url路径不合法，请确认后重试"),
					QString::fromLocal8Bit("确定"));
				item->setText("");
				bChanged = false;
			}
		}

		if(bChanged)
		{
			for(int i = 0; i < tableWidget->columnCount(); i++){
				if(tableWidget->item(item->row(), i))
					tableWidget->item(item->row(), i)->setForeground(QColor(254,102,24));
			}

			if(!m_changedRowVec.contains(item->row()))
				m_changedRowVec.push_back(item->row());
		}
		endItemChanged();
		qDebug() << item->row();
	}
}

QString CSoftUpgradeWidget::getNextDistinctChildName(const QString& suggestedName, int column, int exceptRow) const
{
	if(suggestedName.isEmpty())
		return QString();
	
	QSet<QString> names;
	for (int i = 0; i < tableWidget->rowCount(); i++){
		if(exceptRow == i) continue;
		names.insert(tableWidget->item(i, column)->text());
	}

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

bool CSoftUpgradeWidget::hasInstall(const SoftwareInfo& info)
{
	for (int i = 0; i < m_softwareInfoVector.count(); i++)
	{
		const SoftwareInfo& existInfo = m_softwareInfoVector.at(i);
		if(existInfo.name == info.name)
		{
			return true;
		}
	}

	return false;
}

void CSoftUpgradeWidget::prepareItemChanged()
{
	disconnect(tableWidget, &QTableWidget::itemChanged, this, &CSoftUpgradeWidget::itemChanged);
}

void CSoftUpgradeWidget::endItemChanged()
{
	connect(tableWidget, &QTableWidget::itemChanged, this, &CSoftUpgradeWidget::itemChanged);
}

void CSoftUpgradeWidget::getSoftInfo(int row, SoftwareInfo& info)
{
	for (int j = 0; j < tableWidget->columnCount(); j++)
	{
		switch(j)
		{
		case 0:
			info.name = tableWidget->item(row, 0)->text();
			break;
		case 1:
			info.version = tableWidget->item(row, 1)->text();
			break;
		case 2:
			info.url = tableWidget->item(row, 2)->text();
			break;
		case 3:
			info.brand = tableWidget->item(row, 3)->text();
			break;
		case 4:
			info.category = tableWidget->item(row, 4)->text();
			break;
		case 5:
			info.location = tableWidget->item(row, 5)->text();
			break;
		case 6:
			info.mainExe = tableWidget->item(row, 6)->text();
			break;
		default:
			break;
		}
	}
}

void CSoftUpgradeWidget::insertItem(const SoftwareInfo& info)
{
	int rowCount = tableWidget->rowCount();
	tableWidget->setRowCount(rowCount + 1);
	int row = rowCount;

	prepareItemChanged();
	bool isDriver = false;
	for (int j = 0; j < 7; j++)
	{
		QTableWidgetItem* item = new QTableWidgetItem;	
		tableWidget->setItem(row, j, item);	
		tableWidget->setRowHeight(row, 30);
		item->setTextAlignment(Qt::AlignCenter);
		switch(j)
		{
		case 0:
			{
				item->setText(info.name);
				item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item->setBackground(QColor(230, 230, 230));
			}
			break;
		case 1:
			item->setText(info.version);
			break;
		case 2:
			item->setText(info.url);
			break;
		case 3:
			{
				item->setText(info.brand);
				item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item->setBackground(QColor(230, 230, 230));
				if(!m_brandList.contains(info.brand) && !info.brand.isEmpty()){
					m_brandList.append(info.brand);
					comboBox_brand->addItem(info.brand);
					emit brandAdded(info.brand);
				}
			}
			break;
		case 4:
			{
				item->setText(info.category);
				item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item->setBackground(QColor(230, 230, 230));
			}
			break;
		case 5:
			item->setText(info.location);
			break;
		case 6:
			item->setText(info.mainExe);
			break;
		default:
			break;
		}
	}

	if(tableWidget->item(row, 4)->text() == QString::fromLocal8Bit("驱动"))
	{
		tableWidget->item(row, 5)->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		tableWidget->item(row, 5)->setBackground(QColor(230, 230, 230));
		tableWidget->item(row, 6)->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		tableWidget->item(row, 6)->setBackground(QColor(230, 230, 230));
	}
	else if(tableWidget->item(row, 4)->text() == QString::fromLocal8Bit("网页"))
	{
		tableWidget->item(row, 1)->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		tableWidget->item(row, 1)->setBackground(QColor(230, 230, 230));
		tableWidget->item(row, 5)->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		tableWidget->item(row, 5)->setBackground(QColor(230, 230, 230));
		tableWidget->item(row, 6)->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		tableWidget->item(row, 6)->setBackground(QColor(230, 230, 230));
	}
	endItemChanged();
}

void CSoftUpgradeWidget::updateItem(int row, const SoftwareInfo& info)
{
	prepareItemChanged();
	for (int i = 0; i < tableWidget->columnCount(); i++)
	{
		QTableWidgetItem* item = tableWidget->item(row, i);
		if(!item) continue;

		switch(i)
		{
		case 0:
			{
				item->setText(info.name);
				item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				item->setBackground(QColor(230, 230, 230));
			}
			break;
		case 1:
			item->setText(info.version);
			break;
		case 2:
			item->setText(info.url);
			break;
		case 3:
			item->setText(info.brand);
			break;
		case 4:
			item->setText(info.category);
			break;
		case 5:
			item->setText(info.location);
			break;
		case 6:
			item->setText(info.mainExe);
			break;
		default:
			break;
		}
	}
	endItemChanged();
}

void CSoftUpgradeWidget::SaveContentToFile()
{
	if(tableWidget->rowCount() == 0)
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("当前数据表为空，无数据可生成"),
			QString::fromLocal8Bit("确定"));

		return;
	}

	if(m_changedRowVec.count() > 0 || m_deleteSoftwareInfoVec.count() > 0)
	{
		int ret = QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
			QString::fromLocal8Bit("部分信息已编辑，是否保存编辑后的信息?"),
			QString::fromLocal8Bit("确定"),
			QString::fromLocal8Bit("取消"));

		switch(ret)
		{
		case 0:
			SaveContentToDatabase();
			break;
		default:
			break;
		}
	}

	SoftwareinfoVector vec;
	DatabaseManager::Instance()->ReadAllSoftwareInfo(vec);

	CMarkup xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml.AddElem("Root");
	xml.AddChildElem("SoftwareList");
	xml.IntoElem();
	for (int i = 0; i < vec.count(); i++)
	{
		const SoftwareInfo &info = vec.at(i);
		xml.AddChildElem("Software");
		xml.IntoElem();
		xml.AddAttrib("name", info.name.toStdString());
		xml.AddAttrib("version", info.version.toStdString());
		xml.AddAttrib("url", info.url.toStdString());
		xml.AddAttrib("brand", info.brand.toStdString());
		xml.AddAttrib("category", info.category.toStdString());
		xml.AddAttrib("location", info.location.toStdString());
		xml.AddAttrib("mainexe", info.mainExe.toStdString());
		xml.OutOfElem();
	}
	xml.OutOfElem();
	xml.AddChildElem("PasswdList");
	xml.IntoElem();

	int count = comboBox_brand->count();
	for(int i = 0; i < count; i++)
	{
		QString brand = comboBox_brand->itemText(i);
		QString passwd = CMisc::ReadBrandPasswd(brand);

		if(!passwd.isEmpty())
		{
			xml.AddChildElem("Passwd");
			xml.IntoElem();
			xml.AddAttrib("brand", brand.toStdString());
			xml.AddAttrib("passwd", CMisc::Encrypt(passwd).toStdString());
			xml.OutOfElem();
		}
	}

	xml.OutOfElem();

	//此处添加品牌密码


	QString file = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存配置文件"),
		QApplication::applicationDirPath(), QString::fromLocal8Bit("XML (*.xml)"));
	if(!file.isEmpty())
	{		
		xml.Save(file.toLocal8Bit().constData());
	}
}

//保存表格信息到数据库
void CSoftUpgradeWidget::SaveContentToDatabase(int row)
{
	SoftwareinfoVector curVec;
	SoftwareinfoVector existVec = m_softwareInfoVector;
	for(int i = 0; i < tableWidget->rowCount();i++)
	{
		SoftwareInfo info;
		info.packType = 0;
		getSoftInfo(i, info);

		bool bContinue = false;
		for(int j = 0; j < existVec.count();)
		{
			const SoftwareInfo& existInfo = existVec.at(j);
			if(info.name == existInfo.name)
			{
				if(info.compare(existInfo))
				{
					bContinue = true;
					existVec.removeAt(j);
					break;
				}
				else
				{
					if(existInfo.version == info.version)
					{
						info.packType = 1; //修改其余属性
					}
					else
					{
						info.packType = 2; //version改变
					}
				}

				existVec.removeAt(j);
			}
			else
			{
				j++;
			}
		}

		if(!bContinue)
			curVec.push_back(info);
	}

	for (int i = 0; i < existVec.count(); i++)
	{
		SoftwareInfo existInfo = existVec.at(i);
		existInfo.packType = 3;
		curVec.push_back(existInfo);
	}

	if(DatabaseManager::Instance()->UpdateSoftwareInfos(curVec))
	{
		prepareItemChanged();
		emit datasaved();
		endItemChanged();

		//comboBox_brand->clear();
		//comboBox_brand->addItems(m_brandList);
		ClearBrands();
		initWidget();
	}
	else
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("更新数据失败，请稍后重试"),
			QString::fromLocal8Bit("确定"));
	}
	
}

//添加子品牌
void CSoftUpgradeWidget::addSubBrand()
{
	QString text = lineEdit_brand->text().trimmed();
	if(!text.isEmpty())
	{
		if(m_brandList.contains(text))
		{
			QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("品牌已存在,请确认需要添加的品牌名称"),
				QString::fromLocal8Bit("确定"));
		}
		else
		{
			QMessageBox::information(this, QString::fromLocal8Bit("信息"),
				QString::fromLocal8Bit("品牌 [%1] 添加成功").arg(text),
				QString::fromLocal8Bit("确定"));
			m_brandList.append(text);
			emit brandAdded(text);
			comboBox_brand->addItem(text);
		}
	}
	lineEdit_brand->clear();
}

void CSoftUpgradeWidget::currentBrandChanged(const QString& brand)
{
	if(brand == QString::fromLocal8Bit("选择品牌"))
	{
		for (int i = 0; i < tableWidget->rowCount(); i++)
			tableWidget->showRow(i);
	}
	else
	{
		for (int i = 0; i < tableWidget->rowCount(); i++)
		{
			QTableWidgetItem* item = tableWidget->item(i, 3);
			if(item->text() != brand)
				tableWidget->hideRow(i);
			else
				tableWidget->showRow(i);
		}
	}
}

bool CSoftUpgradeWidget::findItemText(const QTableWidgetItem* item)
{
	int column = item->column();
	QString text = item->text();
	for(int i = 0; i < tableWidget->rowCount(); i++)
	{
		if(i != item->row() && text == tableWidget->item(i, column)->text())
			return true;
	}

	return false;
}

void CSoftUpgradeWidget::PasswdConfig()
{
	CPasswdConfig dlg(this);
	dlg.SetBrands(m_brandList);
	dlg.exec();
}

void CSoftUpgradeWidget::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Delete)
	{
		QTableWidgetItem* item = tableWidget->currentItem();
		if(item && (item->flags() & Qt::ItemIsEditable) && 
			(item->column() == 1 || item->column() == 2 || 
			item->column() == 5 || item->column() == 6))
		{
			prepareItemChanged();
			item->setText("");
			for(int i = 0; i < tableWidget->columnCount(); i++){
				if(tableWidget->item(item->row(), i))
					tableWidget->item(item->row(), i)->setForeground(QColor(254,102,24));
			}

			if(!m_changedRowVec.contains(item->row()))
				m_changedRowVec.push_back(item->row());
			endItemChanged();
		}
	}
	else
	{
		QWidget::keyPressEvent(event);
	}
}

void CSoftUpgradeWidget::ClearBrands()
{
	comboBox_brand->clear();
	m_brandList.clear();
	comboBox_brand->addItem(QString::fromLocal8Bit("选择品牌"));
	m_brandList.append(QStringList() << QString::fromLocal8Bit("CompanyName1")
		<< QString::fromLocal8Bit("CompanyName2"));
	comboBox_brand->addItems(m_brandList);
	emit brandChanged();
}