#include "CDataImport.h"
#include "Markup.h"

#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <qmath.h>

CDataImport::CDataImport(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	connect(pushButton, &QPushButton::clicked, this, &CDataImport::Import);
	connect(toolButton, &QToolButton::clicked, this, &CDataImport::SetDataFile);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &CDataImport::close);
	connect(toolButton_db, &QToolButton::clicked, this, &CDataImport::SetDBFile);
}

CDataImport::~CDataImport()
{

}

void CDataImport::Initial()
{
	groupBox_im->setEnabled(false);
}

void CDataImport::SetDataFile()
{
	QString path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择源数据文件"), ".", tr("XML files (*.xml)"));
	if(!path.isEmpty())
	{
		lineEdit->setText(path);
		lineEdit->setToolTip(path);
		pushButton->setEnabled(false);
		groupBox_im->setEnabled(true);
	}
}

void CDataImport::SetDBFile()
{
	QString path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择导入的数据库"), ".", tr("Database files (*.db)"));
	if(!path.isEmpty())
	{
		pushButton->setEnabled(true);
		lineEdit_db->setText(path);
		lineEdit_db->setToolTip(path);
		groupBox_im->setEnabled(true);
	}
}

void CDataImport::Import()
{
	lineEdit->setEnabled(false);
	toolButton->setEnabled(false);
	pushButton->setEnabled(false);

	QHash<int, QStringList> data;
	CMarkup xml;
	if(xml.Load(lineEdit->text().toLocal8Bit().constData()))
	{
		if(xml.FindChildElem("Worksheet"))
		{
			xml.IntoElem();
			if(xml.FindChildElem("Table"))
			{
				xml.IntoElem();
				int index = 0;
				while(xml.FindChildElem("Row"))
				{
					xml.IntoElem();
					
					QStringList cells;
					while(xml.FindChildElem("Cell"))
					{
						xml.IntoElem();
						if(xml.FindChildElem("Data"))
						{
							xml.IntoElem();
							cells.append(QString::fromStdString(xml.GetData()));
							xml.OutOfElem();
						}
						xml.OutOfElem();
					}
					data.insert(index++, cells);
					xml.OutOfElem();
				}
				xml.OutOfElem();
			}
			xml.OutOfElem();
		}
	}
	QString dataPath = lineEdit_db->text();
	if(CreateTable(dataPath))
		Sync(data);
}

void CDataImport::timerEvent(QTimerEvent *event)
{
	//progressBar->setValue(progressBar->value() + 1);
}

bool CDataImport::CreateTable(const QString& strFilePath)
{
	QSqlDatabase tmp = QSqlDatabase::addDatabase("QSQLITE");
	if(!tmp.isValid())
	{//打开SQLITE驱动失败

		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("打开SQLITE驱动失败, 请确认驱动是否已安装"),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	tmp.setDatabaseName(strFilePath);
	if(!tmp.isValid())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("数据库打开失败，错误信息:[%1]").arg(tmp.lastError().text()),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	m_pSqlDatabase = new QSqlDatabase(tmp);
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("内存不足，数据库打开失败"),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	if(!m_pSqlDatabase->open())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("数据库打开失败，错误信息:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("数据库打开失败，错误信息:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	QSqlQuery sql(*m_pSqlDatabase);
	if(!sql.exec("CREATE TABLE IF NOT EXISTS StatisticsInfo(regional char(512), unitnum char(256), unitname char(512), shopid char(256), shopname char(512), install char(256));"))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("打开UserInfo表失败，错误信息:[%1]").arg(sql.lastError().text()),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("数据库打开失败，错误信息:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("确定"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

bool CDataImport::Sync(const QHash<int, QStringList>& data)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("数据库未打开，无法保存"),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("数据库保存失败，错误信息:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("确定"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("DELETE FROM StatisticsInfo;");
	QSqlQuery deleteQuery(*m_pSqlDatabase);
	if(!deleteQuery.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("数据库保存失败，错误信息:[%1]").arg(deleteQuery.lastError().text()),
			QString::fromLocal8Bit("确定"));
		m_pSqlDatabase->rollback();
		return false;
	}

	int count = data.count();
	int step = qFloor((double)count / 100);
	int index = 0;
	QHashIterator<int, QStringList> iter(data);
	while(iter.hasNext())
	{
		iter.next();
		QStringList cells = iter.value();

		if(cells.count() == 5)
			cells.append(""); //没有填写的自动补一列
		else if(cells.contains(QString::fromLocal8Bit("营销区域")) &&
				cells.contains(QString::fromLocal8Bit("组织编号")) &&
				cells.contains(QString::fromLocal8Bit("组织名称")))
			continue;

		if(cells.count() == 6)
		{
			QSqlQuery insertQuery(*m_pSqlDatabase);
			insertQuery.prepare("INSERT INTO StatisticsInfo (regional, unitnum, unitname, shopid, shopname, install) VALUES (:regional, :unitnum, :unitname, :shopid, :shopname, :install);");
			insertQuery.bindValue(":regional", cells[0]);
			insertQuery.bindValue(":unitnum", cells[1]);
			insertQuery.bindValue(":unitname", cells[2]);
			insertQuery.bindValue(":shopid", cells[3]);
			insertQuery.bindValue(":shopname", cells[4]);
			insertQuery.bindValue(":install", cells[5]);

			if(!insertQuery.exec())
			{
				QMessageBox::warning(NULL, tr("Error"), 
					QString::fromLocal8Bit("数据库保存失败，错误信息:[%1]").arg(insertQuery.lastError().text()),
					QString::fromLocal8Bit("确定"));
				m_pSqlDatabase->rollback();
				return false;
			}
			
			if(index == step){
				index = 0;
				progressBar->setValue(progressBar->value() + 1);
			}

			index++;
		}
	}


	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("数据库保存失败，错误信息:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("确定"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	QMessageBox::about(this, QString::fromLocal8Bit("消息"), QString::fromLocal8Bit("导入数据成功"));

	lineEdit->setEnabled(true);
	toolButton->setEnabled(true);
	pushButton->setEnabled(true);

	return true;
}
