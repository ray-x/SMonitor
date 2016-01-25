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
#include "databasemanager.h"
#include "NetClass.h"
#include "CSoftUpgradeWidget.h"
#include <QFile>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QtCore/QCoreApplication>
#include <QMessageBox>
#include <QDir>
#include <QUuid>

#define RELEASESQL(x) {if(NULL != (x)){ delete (x); (x) = NULL; }}
DatabaseManager* DatabaseManager::m_Instance = NULL;

DatabaseManager* DatabaseManager::Instance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new DatabaseManager();
		Q_ASSERT(m_Instance);
	}

	return m_Instance;
}

DatabaseManager::DatabaseManager()
	: QObject(qApp)
	, m_strDatabasePath("")
	, m_bFull(false)
{
	m_pSqlDatabase = NULL;
}

DatabaseManager::~DatabaseManager()
{
	CloseDatabase();
}

//�������ݿ��ļ�
//bCreateIfInExist - true ������ݿⲻ���ڻ��Զ����� - false ���ݿⲻ���ڣ��ᴴ��ʧ�ܣ�����false
bool DatabaseManager::LoadDatabase(const QString& strFilePath, bool bCreateIfInExist/* = true*/)
{
	QString dirPath = QFileInfo(strFilePath).absolutePath();
	QDir dir;
	if(!dir.mkpath(dirPath))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�Ŀ¼[%1]����ʧ�ܣ���ȷ��Ŀ������Ƿ�ռ�����").arg(dirPath),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	m_bFull = false;
	QMutexLocker locker(&m_DateLock);
	RELEASESQL(m_pSqlDatabase);

	if(!bCreateIfInExist && !QFile::exists(strFilePath))
		return false;

	QSqlDatabase tmp = QSqlDatabase::addDatabase("QSQLITE");
	if(!tmp.isValid())
	{//��SQLITE����ʧ��

		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("��SQLITE����ʧ��, ��ȷ�������Ƿ��Ѱ�װ"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	tmp.setDatabaseName(strFilePath);
	if(!tmp.isValid())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ��ʧ�ܣ�������Ϣ:[%1]").arg(tmp.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	m_pSqlDatabase = new QSqlDatabase(tmp);
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("�ڴ治�㣬���ݿ��ʧ��"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	if(!m_pSqlDatabase->open())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	//����User��
	QSqlQuery sql(*m_pSqlDatabase);
	if(!sql.exec("CREATE TABLE IF NOT EXISTS UserInfo(name char(522), password char(512));"))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("��UserInfo��ʧ�ܣ�������Ϣ:[%1]").arg(sql.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	//����ClientInfo��
	if(!sql.exec("CREATE TABLE IF NOT EXISTS ClientInfo(id char(256), version char(256));"))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("��ClientInfo��ʧ�ܣ�������Ϣ:[%1]").arg(sql.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	//����Software��
	if(!sql.exec("CREATE TABLE IF NOT EXISTS Software(name char(256), version char(256), url char(512), brand char(512), category char(256) \
				 , location char(256), mainExe char(512), installNum char(256), uninstalledNum char(256), packType char(32));"))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("��Software��ʧ�ܣ�������Ϣ:[%1]").arg(sql.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	//������װ��Ϣ��
	if(!sql.exec("CREATE TABLE IF NOT EXISTS LastInstallHitory(id char(256), name char(256), version char(256), datetime char(256));"))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("��History��ʧ�ܣ�������Ϣ:[%1]").arg(sql.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	//����������Ϣ��
	if(!sql.exec("CREATE TABLE IF NOT EXISTS HelpInfo(title char(512), info char(1024));"))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("��HelpInfo��ʧ�ܣ�������Ϣ:[%1]").arg(sql.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	//�����ŵ����ݱ�
	if(!sql.exec("CREATE TABLE IF NOT EXISTS StatisticsInfo(regional char(512), unitnum char(256), unitname char(512), shopid char(256), shopname char(512), install char(256));"))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("��StatisticsInfo��ʧ�ܣ�������Ϣ:[%1]").arg(sql.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//����û����������Ƿ����
bool DatabaseManager::IsUserExist(const QString& strUserName, const QString& strPassword)
{
	if(NULL == m_pSqlDatabase)
		return false;

	bool bOk = false;
	QString str = "";
	QString strSql = QString::fromLocal8Bit("SELECT * FROM UserInfo WHERE name = '%1' AND password = '%2';").arg(strUserName).arg(strPassword);
	QSqlQuery query(strSql, *m_pSqlDatabase);
	if(query.next()) 
	{
		return true;
	}

	return false;
}

//ͨ���ŵ����жϿͻ����Ƿ����
bool DatabaseManager::IsClientExist(const QString& clientId)
{
	if(NULL == m_pSqlDatabase)
		return false;

	bool bOk = false;
	QString str = "";
	QString strSql = QString::fromLocal8Bit("SELECT * FROM ClientInfo WHERE id = '%1';").arg(clientId);
	QSqlQuery query(strSql, *m_pSqlDatabase);
	if(query.next()) 
	{
		return true;
	}

	return false;
}

//�жϰ�����Ϣ�Ƿ����
bool DatabaseManager::IsHelpInfoExist(const QString& title)
{
	if(NULL == m_pSqlDatabase)
		return false;

	bool bOk = false;
	QString strSql = QString::fromLocal8Bit("SELECT * FROM HelpInfo WHERE title = '%1';").arg(title);

	QSqlQuery query(strSql, *m_pSqlDatabase);
	if(query.next()) 
	{
		return true;
	}

	return false;
}

//��ȡ������Ϣ
bool DatabaseManager::ReadHelpInfos(HelpInfoMap& helpInfoMap)
{
	helpInfoMap.clear();
	if(NULL == m_pSqlDatabase)
		return false;

	//QString strSql = QString::fromLocal8Bit("SELECT * FROM Info WHERE pic_id = '%1' AND proj_id = '%2' ORDER BY rc_t_TextType, rc_t_TextNum;").arg(strID).arg(strProjID);
	QString strSql = QString::fromLocal8Bit("SELECT * FROM HelpInfo;");
	QSqlQuery query(strSql, *m_pSqlDatabase);
	while(query.next()) 
	{
		helpInfoMap.insert(query.value("title").toString(), query.value("info").toString());
	}

	return helpInfoMap.count() != 0;
}

//���������Ϣ
bool DatabaseManager::SaveHelpInfos(const HelpInfoMap& helpInfoMap)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�δ�򿪣��޷�����"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("DELETE FROM HelpInfo;");
	QSqlQuery deleteQuery(*m_pSqlDatabase);
	if(!deleteQuery.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(deleteQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	foreach(auto key, helpInfoMap.keys())
	{
		QSqlQuery insertQuery(*m_pSqlDatabase);
		insertQuery.prepare("INSERT INTO HelpInfo (title, info) VALUES (:title, :info);");
		insertQuery.bindValue(":title", key);
		insertQuery.bindValue(":info", helpInfoMap.value(key));
		
		if(!insertQuery.exec())
		{
			QMessageBox::warning(NULL, tr("Error"), 
				QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(insertQuery.lastError().text()),
				QString::fromLocal8Bit("ȷ��"));
			m_pSqlDatabase->rollback();
			return false;
		}
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//��ȡ�ŵ������װ���
bool DatabaseManager::GetCrosslinkRunInstalledResult(const QString& strShopId, QString& strResult)
{
	strResult = "";
	if(NULL == m_pSqlDatabase)
		return false;

	QString strSql = QString::fromLocal8Bit("SELECT install FROM StatisticsInfo WHERE shopid = '%1';").arg(strShopId);
	QSqlQuery query(strSql, *m_pSqlDatabase);
	if(query.next()) 
	{
		strResult = query.value("install").toString();
		return true;
	}

	return false;
}

//��ȡ�ŵ�����
bool DatabaseManager::ReadShopInfoHash(QHash<int, QStringList>& data)
{
	data.clear();
	if(NULL == m_pSqlDatabase)
		return false;

	QString strSql = QString::fromLocal8Bit("SELECT * FROM StatisticsInfo;");
	QSqlQuery query(strSql, *m_pSqlDatabase);
	int nIndex = 0;
	while(query.next()) 
	{
		QStringList strValueList;
		strValueList << query.value("regional").toString();
		strValueList << query.value("unitnum").toString();
		strValueList << query.value("unitname").toString();
		strValueList << query.value("shopid").toString();
		strValueList << query.value("shopname").toString();
		strValueList << query.value("install").toString();
		data.insert(nIndex++, strValueList);
	}

	return true;
}

//�ж�����Ƿ����
bool DatabaseManager::IsSoftwareExist(const QString& strSoftwatename)
{
	if(NULL == m_pSqlDatabase)
		return false;

	QString strSql = QString::fromLocal8Bit("SELECT * FROM Software WHERE name = '%1';").arg(strSoftwatename);
	QSqlQuery query(strSql, *m_pSqlDatabase);
	if(query.next()) 
	{
		return true;
	}

	return false;
}

//�ж�����Ͱ汾�Ƿ�һ��
bool DatabaseManager::IsSoftwareExist(const QString& strSoftwareName, const QString& strVersion)
{
	if(NULL == m_pSqlDatabase)
		return false;

	QString strSql = QString::fromLocal8Bit("SELECT * FROM Software WHERE name = '%1' AND version = '%2';").arg(strSoftwareName).arg(strVersion);
	QSqlQuery query(strSql, *m_pSqlDatabase);
	if(query.next()) 
	{
		return true;
	}

	return false;
}

//�ж������װ֪ͨ��Ϣ�Ƿ����
bool DatabaseManager::IsSoftInstallInfoExist(const QString& clientId, const QString& name, const QString& version)
{
	if(NULL == m_pSqlDatabase)
		return false;

	QString strSql = QString::fromLocal8Bit("SELECT * FROM LastInstallHitory WHERE id = '%1' AND name = '%2' AND version = '%3';").arg(clientId).arg(name).arg(version);
	QSqlQuery query(strSql, *m_pSqlDatabase);
	if(query.next()) 
	{
		return true;
	}

	return false;
}

//�޸�����
bool DatabaseManager::ModifyPassword(const QString& strUserName, const QString& strPassword)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�δ��"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("UPDATE UserInfo SET password = '%1' WHERE name = '%2';").arg(strPassword).arg(strUserName);
	QSqlQuery excete(*m_pSqlDatabase);
	if(!excete.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(excete.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//����û�
bool DatabaseManager::AddUser(const QString& strUserName, const QString& strPassword)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�δ�򿪣��޷�����"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("DELETE FROM UserInfo WHERE name = '%1';").arg(strUserName);
	QSqlQuery deleteQuery(*m_pSqlDatabase);
	if(!deleteQuery.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(deleteQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	QSqlQuery insertQuery(*m_pSqlDatabase);
	insertQuery.prepare("INSERT INTO UserInfo (name, password) VALUES (:name, :password);");
	insertQuery.bindValue(":name", strUserName);
	insertQuery.bindValue(":password", strPassword);

	if(!insertQuery.exec())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(insertQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//��ӿͻ�����Ϣ
bool DatabaseManager::AddClientInfo(const ClientInfo& clientInfo)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�δ�򿪣��޷�����"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("DELETE FROM ClientInfo WHERE id = '%1';").arg(clientInfo.id);
	QSqlQuery deleteQuery(*m_pSqlDatabase);
	if(!deleteQuery.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(deleteQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

 	QSqlQuery insertQuery(*m_pSqlDatabase);
	insertQuery.prepare("INSERT INTO ClientInfo (id, version) VALUES (:id, :version);");
	insertQuery.bindValue(":id", clientInfo.id);
	insertQuery.bindValue(":version", clientInfo.version);

	if(!insertQuery.exec())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(insertQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	//���ݴ洢�ɹ������������ݿ�
	return true;
}

//��������Ϣ
bool DatabaseManager::AddSoftwareInfo(const SoftwareInfo& softwareInfo)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�δ�򿪣��޷�����"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("DELETE FROM Software WHERE name = '%1';").arg(softwareInfo.name);
	QSqlQuery deleteQuery(*m_pSqlDatabase);
	if(!deleteQuery.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(deleteQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	QSqlQuery insertQuery(*m_pSqlDatabase);
	insertQuery.prepare("INSERT INTO Software (name, version, url, brand, category, location, mainExe, installNum, uninstalledNum, packType) \
		VALUES (:name, :version, :url, :brand, :category, :location, :mainExe, :installNum, :uninstalledNum, :packType);");

	insertQuery.bindValue(":name", softwareInfo.name);
	insertQuery.bindValue(":version", softwareInfo.version);
	insertQuery.bindValue(":url", softwareInfo.url);
	insertQuery.bindValue(":brand", softwareInfo.brand);
	insertQuery.bindValue(":category", softwareInfo.category);
	insertQuery.bindValue(":location", softwareInfo.location);
	insertQuery.bindValue(":mainExe", softwareInfo.mainExe);
	insertQuery.bindValue(":installNum", QString::fromLocal8Bit("%1").arg(softwareInfo.installNum));
	insertQuery.bindValue(":uninstalledNum", QString::fromLocal8Bit("%1").arg(softwareInfo.uninstalledNum));
	insertQuery.bindValue(":packType", QString::fromLocal8Bit("%1").arg(softwareInfo.packType));

	if(!insertQuery.exec())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(insertQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	//���ݴ洢�ɹ������������ݿ�
	return true;
}

//���������Ϣ
bool DatabaseManager::SaveSoftwareInfos(const SoftwareinfoVector& softwareInfoVector)
{
	if(softwareInfoVector.count() == 0)
		return true;

	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�δ�򿪣��޷�����"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("DELETE FROM Software;");
	QSqlQuery deleteQuery(*m_pSqlDatabase);
	if(!deleteQuery.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(deleteQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	int nCount = softwareInfoVector.count();
	for (int i = 0; i < nCount; i++)
	{
		const SoftwareInfo& softwareInfo = softwareInfoVector.at(i);
		QSqlQuery insertQuery(*m_pSqlDatabase);
		insertQuery.prepare("INSERT INTO Software (name, version, url, brand, category, location, mainExe, installNum, uninstalledNum, packType) \
							VALUES (:name, :version, :url, :brand, :category, :location, :mainExe, :installNum, :uninstalledNum, :packType);");

		insertQuery.bindValue(":name", softwareInfo.name);
		insertQuery.bindValue(":version", softwareInfo.version);
		insertQuery.bindValue(":url", softwareInfo.url);
		insertQuery.bindValue(":brand", softwareInfo.brand);
		insertQuery.bindValue(":category", softwareInfo.category);
		insertQuery.bindValue(":location", softwareInfo.location);
		insertQuery.bindValue(":mainExe", softwareInfo.mainExe);
		insertQuery.bindValue(":installNum", QString::fromLocal8Bit("%1").arg(softwareInfo.installNum));
		insertQuery.bindValue(":uninstalledNum", QString::fromLocal8Bit("%1").arg(softwareInfo.uninstalledNum));
		insertQuery.bindValue(":packType", QString::fromLocal8Bit("%1").arg(softwareInfo.packType));

		if(!insertQuery.exec())
		{
			QMessageBox::warning(NULL, tr("Error"), 
				QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(insertQuery.lastError().text()),
				QString::fromLocal8Bit("ȷ��"));
			m_pSqlDatabase->rollback();
			return false;
		}
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//�޸������Ϣ����������Ϊ��� ͨ��packType���������� 0 - insert 1 - updatewithoutnum 2 - updateall 3 - remove
bool DatabaseManager::UpdateSoftwareInfos(const SoftwareinfoVector& softwareInfoVector)
{
	if(softwareInfoVector.count() == 0)
		return true;

	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�δ�򿪣��޷�����"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = "select count(*) from clientinfo";
	QSqlQuery query(strSql, *m_pSqlDatabase);
	int nClientNum = 0;
	if(query.next()) 
	{
		nClientNum = query.value(0).toInt();
	}

	int nCount = softwareInfoVector.count();
	for (int i = 0; i < nCount; i++)
	{
		const SoftwareInfo& softwareInfo = softwareInfoVector.at(i);
		if(0 == softwareInfo.packType)
		{// insert
			QSqlQuery insertQuery(*m_pSqlDatabase);
			insertQuery.prepare("INSERT INTO Software (name, version, url, brand, category, location, mainExe, installNum, uninstalledNum, packType) \
								VALUES (:name, :version, :url, :brand, :category, :location, :mainExe, :installNum, :uninstalledNum, :packType);");

			insertQuery.bindValue(":name", softwareInfo.name);
			insertQuery.bindValue(":version", softwareInfo.version);
			insertQuery.bindValue(":url", softwareInfo.url);
			insertQuery.bindValue(":brand", softwareInfo.brand);
			insertQuery.bindValue(":category", softwareInfo.category);
			insertQuery.bindValue(":location", softwareInfo.location);
			insertQuery.bindValue(":mainExe", softwareInfo.mainExe);
			insertQuery.bindValue(":installNum", QString::fromLocal8Bit("%1").arg(softwareInfo.installNum));
			insertQuery.bindValue(":uninstalledNum", QString::fromLocal8Bit("%1").arg(nClientNum));
			insertQuery.bindValue(":packType", QString::fromLocal8Bit("%1").arg(softwareInfo.packType));

			if(!insertQuery.exec())
			{
				QMessageBox::warning(NULL, tr("Error"), 
					QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(insertQuery.lastError().text()),
					QString::fromLocal8Bit("ȷ��"));
				m_pSqlDatabase->rollback();
				return false;
			}
		}
		else
		{
			QString strSql;
			if(1 == softwareInfo.packType)
			{//1 - updatewithoutnum 
				strSql = QString::fromLocal8Bit("UPDATE Software SET version = '%1', url = '%2', brand = '%3', category = '%4', location = '%5', mainExe = '%6',\
												packType = '%7'  WHERE name = '%8';").arg(softwareInfo.version).arg(softwareInfo.url).arg(softwareInfo.brand)
												.arg(softwareInfo.category).arg(softwareInfo.location).arg(softwareInfo.mainExe).arg(softwareInfo.packType).arg(softwareInfo.name);
			}
			else if(2 == softwareInfo.packType)
			{//2 - updateall
				strSql = QString::fromLocal8Bit("UPDATE Software SET version = '%1', url = '%2', brand = '%3', category = '%4', location = '%5', mainExe = '%6',\
												installNum = '%7', uninstalledNum = '%8', packType = '%9'  WHERE name = '%10';").arg(softwareInfo.version).arg(softwareInfo.url).arg(softwareInfo.brand)
												.arg(softwareInfo.category).arg(softwareInfo.location).arg(softwareInfo.mainExe).arg(0).arg(nClientNum).arg(softwareInfo.packType).arg(softwareInfo.name);
			}
			else
			{//3 - remove
				strSql = QString::fromLocal8Bit("DELETE FROM Software WHERE name = '%1';").arg(softwareInfo.name);
			}

			QSqlQuery excete(*m_pSqlDatabase);
			if(!excete.exec(strSql))
			{
				QMessageBox::warning(NULL, tr("Error"), 
					QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(excete.lastError().text()),
					QString::fromLocal8Bit("ȷ��"));
				m_pSqlDatabase->rollback();
				return false;
			}
		}
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//���¿ͻ���Ϣ
bool DatabaseManager::UpdateClientInfo(const ClientInfo& clientInfo)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ�δ��"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("UPDATE ClientInfo SET version = '%1' WHERE id = '%2';").arg(clientInfo.version).arg(clientInfo.id);
	QSqlQuery excete(*m_pSqlDatabase);
	if(!excete.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(excete.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//���������Ϣ
bool DatabaseManager::UpdateSoftwareInfo(const SoftwareInfo& softwareInfo)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("�����ݿ�δ��"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("UPDATE Software SET version = '%1', url = '%2', brand = '%3', category = '%4', location = '%5', mainExe = '%6'\
											installNum = '%7', uninstalledNum = '%8', packType = '%9'  WHERE name = '%10';").arg(softwareInfo.version).arg(softwareInfo.url).arg(softwareInfo.brand)
											.arg(softwareInfo.category).arg(softwareInfo.location).arg(softwareInfo.mainExe).arg(softwareInfo.installNum).arg(softwareInfo.uninstalledNum).arg(softwareInfo.packType).arg(softwareInfo.name);

	QSqlQuery excete(*m_pSqlDatabase);
	if(!excete.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(excete.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//���������Ϣ
bool DatabaseManager::UpdateSoftwareInfoWithOuInstallNum(const SoftwareInfo& softwareInfo)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("�����ݿ�δ��"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("UPDATE Software SET version = '%1', url = '%2', brand = '%3', category = '%4', location = '%5', mainExe = '%6'\
											packType = '%7'  WHERE name = '%8';").arg(softwareInfo.version).arg(softwareInfo.url).arg(softwareInfo.brand)
											.arg(softwareInfo.category).arg(softwareInfo.location).arg(softwareInfo.mainExe).arg(softwareInfo.packType).arg(softwareInfo.name);

	QSqlQuery excete(*m_pSqlDatabase);
	if(!excete.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(excete.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//���������װ��
bool DatabaseManager::AddSoftwareInstallNum(const QString& name, int nAddNums/* = 1*/)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("�����ݿ�δ��"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("UPDATE Software SET installNum = installNum + %1 WHERE name = '%2';").arg(nAddNums).arg(name);
	QSqlQuery excete(*m_pSqlDatabase);
	if(!excete.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(excete.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//�������δ��װ��
bool DatabaseManager::AddSoftwareUninstalledNum(const QString& name, int nAddNums/* = 1*/)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("�����ݿ�δ��"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("UPDATE Software SET uninstalledNum = uninstalledNum + %1 WHERE name = '%2';").arg(nAddNums).arg(name);
	QSqlQuery excete(*m_pSqlDatabase);
	if(!excete.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(excete.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//��ȡ���������Ϣ
bool DatabaseManager::ReadAllSoftwareInfo(SoftwareinfoVector& softwareInfoVector)
{
	softwareInfoVector.clear();
	if(NULL == m_pSqlDatabase)
		return false;

//	m_SoftwareInstallInfoMap.clear();

	//QString strSql = QString::fromLocal8Bit("SELECT * FROM Info WHERE pic_id = '%1' AND proj_id = '%2' ORDER BY rc_t_TextType, rc_t_TextNum;").arg(strID).arg(strProjID);
	QString strSql = QString::fromLocal8Bit("SELECT * FROM Software;");
	QSqlQuery query(strSql, *m_pSqlDatabase);
	while(query.next()) 
	{
		SoftwareInfo software;
		readSoftwareItem(software, query);
		softwareInfoVector.append(software);

// 		SoftwareInstallInfo installInfo;
// 		installInfo.name = software.name;
// 		installInfo.version = software.version;
// 		installInfo.installNum = software.installNum;
// 		installInfo.uninstalledNum = software.uninstalledNum;
// 		m_SoftwareInstallInfoMap.insert(installInfo.name, installInfo);
	}

	return softwareInfoVector.count() != 0;
}

//��ȡָ�����������Ϣ
bool DatabaseManager::ReadSoftwareInfo(const QString& name, SoftwareInfo& softwareInfo)
{	
	if(NULL == m_pSqlDatabase)
		return false;

	//QString strSql = QString::fromLocal8Bit("SELECT * FROM Info WHERE pic_id = '%1' AND proj_id = '%2' ORDER BY rc_t_TextType, rc_t_TextNum;").arg(strID).arg(strProjID);
	QString strSql = QString::fromLocal8Bit("SELECT * FROM Software WHERE name = '%1';").arg(name);
	QSqlQuery query(strSql, *m_pSqlDatabase);
	while(query.next()) 
	{
		readSoftwareItem(softwareInfo, query);
		return true;
	}

	return false;
}

//ɾ�����
bool DatabaseManager::RemoveSoftware(const QString& name)
{
	if(NULL == m_pSqlDatabase)
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("�����ݿ�δ��"),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		return false;
	}

	QString strSql = QString::fromLocal8Bit("DELETE FROM Software WHERE name = '%1';").arg(name);
	QSqlQuery deleteQuery(*m_pSqlDatabase);
	if(!deleteQuery.exec(strSql))
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿⱣ��ʧ�ܣ�������Ϣ:[%1]").arg(deleteQuery.lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false;
	}

	if(!m_pSqlDatabase->commit())
	{
		QMessageBox::warning(NULL, tr("Error"), 
			QString::fromLocal8Bit("���ݿ����ʧ�ܣ�������Ϣ:[%1]").arg(m_pSqlDatabase->lastError().text()),
			QString::fromLocal8Bit("ȷ��"));
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//д������
bool DatabaseManager::WriteDataMotionList(const DataMotionVector& dataMotionVector)
{
	if(NULL == m_pSqlDatabase)
	{
		ServerLogger->AddLog("NULL == m_pSqlDatabase ���ݿ�δ��");
		return false;
	}

	QMutexLocker locker(&m_DateLock);
	if(!m_pSqlDatabase->transaction())
	{
		ServerLogger->AddLog("m_pSqlDatabase->transaction() ���ݿ����ʧ�ܣ�������Ϣ:[%s]", m_pSqlDatabase->lastError().text().toStdString().c_str());
		return false;
	}

	SoftwareinfoMap localSoftwareInfoMap;
	if(NetClass->m_pSoftUpgradeWidget != NULL)
	{
		localSoftwareInfoMap = NetClass->m_pSoftUpgradeWidget->GetSoftwareInfoMap(true);
	}

	int nCount = dataMotionVector.count();
	int nInstallAppNum = 0;
	QString strPrevAppName = "";
	for (int i = 0; i < nCount; i++)
	{
		const DataMotion& dataMotion = dataMotionVector[i];
		QString strSql = "";
		if(EMSG_REGISTERID == dataMotion.msg)
		{
			QSqlQuery insertQuery(*m_pSqlDatabase);
			insertQuery.prepare("INSERT INTO ClientInfo (id, version) VALUES (:id, :version);");

			insertQuery.bindValue(":id", dataMotion.clientInfo.id);
			insertQuery.bindValue(":version", dataMotion.clientInfo.version);

			if(!insertQuery.exec())
			{
				ServerLogger->AddLog("ִ�����[%s] ���ݿ����ʧ�ܣ�������Ϣ:[%s]", insertQuery.lastQuery().toStdString().c_str(), insertQuery.lastError().text().toStdString().c_str());
				m_pSqlDatabase->rollback();
				return false;
			}

			//ע��һ���µ��ŵ���ʱ����ʾ����һ���µĿͻ��ˣ���Ҫ�����������δ��װ����1
			strSql = QString::fromLocal8Bit("UPDATE Software SET uninstalledNum = uninstalledNum + %1;").arg(1);
			if(!excuteSql(strSql))
			{
				return false;	
			}

			nInstallAppNum = 2;
		}
		else if(EMSG_MODIFYID == dataMotion.msg)
		{
			strSql = QString::fromLocal8Bit("UPDATE ClientInfo SET id = '%1' WHERE id = '%2';").arg(dataMotion.clientInfo.id).arg(dataMotion.clientInfo.version);
			if(!excuteSql(strSql))
			{
				return false;	
			}
		}
		else if(EMSG_SOFTINSTALL == dataMotion.msg)
		{
			strSql = QString::fromLocal8Bit("DELETE FROM LastInstallHitory WHERE id = '%1' AND name = '%2';").arg(dataMotion.clientInfo.id).arg(dataMotion.softwareInfo.name);
			if(!excuteSql(strSql))
			{
				return false;	
			}

			QString strCurTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
			QSqlQuery insertQuery(*m_pSqlDatabase);
			insertQuery.prepare("INSERT INTO LastInstallHitory (id, name, version, datetime) VALUES(:id, :name, :version, :datetime);");

			insertQuery.bindValue(":id", dataMotion.clientInfo.id);
			insertQuery.bindValue(":name", dataMotion.softwareInfo.name);
			insertQuery.bindValue(":version", dataMotion.softwareInfo.version);
			insertQuery.bindValue(":datetime", strCurTime);

			if(!insertQuery.exec())
			{
				ServerLogger->AddLog("ִ�����[%s] ���ݿ����ʧ�ܣ�������Ϣ:[%s]", insertQuery.lastQuery().toStdString().c_str(), insertQuery.lastError().text().toStdString().c_str());
				m_pSqlDatabase->rollback();
				return false;
			}

			strSql = QString::fromLocal8Bit("UPDATE Software SET installNum = installNum + %1 WHERE name = '%2' AND version = '%3';").arg(1).arg(dataMotion.softwareInfo.name).arg(dataMotion.softwareInfo.version);
			if(!excuteSql(strSql))
			{
				return false;	
			}

			strSql = QString::fromLocal8Bit("UPDATE Software SET uninstalledNum = uninstalledNum - %1 WHERE name = '%2' AND version = '%3';").arg(1).arg(dataMotion.softwareInfo.name).arg(dataMotion.softwareInfo.version);
			if(!excuteSql(strSql))
			{
				return false;	
			}

			if(0 == nInstallAppNum || strPrevAppName != dataMotion.softwareInfo.name)
			{
				strPrevAppName = dataMotion.softwareInfo.name;
				nInstallAppNum++;
			}
		}
		else if(EMSG_SOFTUPLOAD == dataMotion.msg)
		{
			strSql = QString::fromLocal8Bit("DELETE FROM LastInstallHitory WHERE id = '%1';").arg(dataMotion.clientInfo.id);
			if(!excuteSql(strSql))
			{
				return false;	
			}

			int nNewSoftCount = dataMotion.softwareInfoVector.count();
			for (int i = 0; i < nNewSoftCount; i++)
			{
				const SoftwareInfo& softwareInfo = dataMotion.softwareInfoVector[i];
				if(softwareInfo.category == QString::fromLocal8Bit("��ҳ"))
					continue;

				if(localSoftwareInfoMap.contains(softwareInfo.name))
				{
					const SoftwareInfo& localSoftwareInfo = localSoftwareInfoMap[softwareInfo.name];
					if(softwareInfo.category != QString::fromLocal8Bit("����"))
					{//�������ͱȽϰ汾��
						if(softwareInfo.version != localSoftwareInfo.version)
						{
							continue;
						}
					}
				}

				QString strCurTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
				QSqlQuery insertQuery(*m_pSqlDatabase);
				insertQuery.prepare("INSERT INTO LastInstallHitory (id, name, version, datetime) VALUES(:id, :name, :version, :datetime);");

				insertQuery.bindValue(":id", dataMotion.clientInfo.id);
				insertQuery.bindValue(":name", softwareInfo.name);
				insertQuery.bindValue(":version", softwareInfo.version);
				insertQuery.bindValue(":datetime", strCurTime);

				if(!insertQuery.exec())
				{
					ServerLogger->AddLog("ִ�����[%s] ���ݿ����ʧ�ܣ�������Ϣ:[%s]", insertQuery.lastQuery().toStdString().c_str(), insertQuery.lastError().text().toStdString().c_str());
					m_pSqlDatabase->rollback();
					return false;
				}
			}

			if(0 == nInstallAppNum || strPrevAppName != dataMotion.softwareInfo.name)
			{
				strPrevAppName = dataMotion.softwareInfo.name;
				nInstallAppNum++;
			}
		}
		else if(EMSG_CROSSRUNINFO == dataMotion.msg)
		{
			int nMotion = dataMotion.userInfoMap.value("Motion", "0").toInt();
			QString strUnitNum = dataMotion.userInfoMap.value("UnitNum", "");
			QString strUnitName = dataMotion.userInfoMap.value("UnitName", "");
			QString strCrosslinkRunInstalledState = dataMotion.userInfoMap.value("CrosslinkRunInstalledState", "");

			if(strUnitNum.isEmpty())
				continue;

			if(2 == nMotion)
			{//���� 

				//����֮ǰ��Ҫselectһ�¿��Ƿ���ڣ����������
				strSql = QString::fromLocal8Bit("SELECT * FROM StatisticsInfo WHERE shopid = '%1';").arg(strUnitNum);

				QSqlQuery query(strSql, *m_pSqlDatabase);
				if(query.next()) 
				{//���ڣ����ִ�и�������
					nMotion = 1;
				}
				else
				{//����
					QSqlQuery insertQuery(*m_pSqlDatabase);
					insertQuery.prepare("INSERT INTO StatisticsInfo (regional, unitnum, unitname, shopid, shopname, install) VALUES (:regional, :unitnum, :unitname, :shopid, :shopname, :install);");
					insertQuery.bindValue(":regional", "");
					insertQuery.bindValue(":unitnum", "");
					insertQuery.bindValue(":unitname", "");
					insertQuery.bindValue(":shopid", strUnitNum);
					insertQuery.bindValue(":shopname", strUnitName);
					insertQuery.bindValue(":install", strCrosslinkRunInstalledState);

					if(!insertQuery.exec())
					{
						ServerLogger->AddLog("ִ�����[%s] ���ݿ����ʧ�ܣ�������Ϣ:[%s]", insertQuery.lastQuery().toStdString().c_str(), insertQuery.lastError().text().toStdString().c_str());
						m_pSqlDatabase->rollback();
						return false;
					}
				}
			}

			if(1 == nMotion)
			{//����
				strSql = QString::fromLocal8Bit("UPDATE StatisticsInfo SET install = '%1' WHERE shopid = '%2';").arg(strCrosslinkRunInstalledState).arg(strUnitNum);
				if(!excuteSql(strSql))
				{
					return false;
				}
			}
		}
		else
		{
			ServerLogger->AddLog("getDataMotionSql ��⵽��Ч������� ������ϢΪ[%d]", dataMotion.msg);
			continue;
		}
	}

	if(nInstallAppNum > 0)
	{
		QString strSql = "select count(*) from clientinfo";
		QSqlQuery query(strSql, *m_pSqlDatabase);
		int nClientNum = 0;
		if(query.next()) 
		{
			nClientNum = query.value(0).toInt();
		}

		QMapIterator<QString, SoftwareInfo> iter(localSoftwareInfoMap);
		while(iter.hasNext())
		{
			iter.next();
			const SoftwareInfo& software = iter.value();
			if(software.category != QString::fromLocal8Bit("��ҳ"))
			{
				int nInstallCount = 0;
				QString strSql = "";
				if(software.category == QString::fromLocal8Bit("����"))
				{
					strSql = QString::fromLocal8Bit("SELECT COUNT(*) FROM LastInstallHitory WHERE name = '%1'").arg(software.name);
				}
				else
				{
					strSql = QString::fromLocal8Bit("SELECT COUNT(*) FROM LastInstallHitory WHERE name = '%1' AND version = '%2'").arg(software.name).arg(software.version);
				}

				QSqlQuery query(strSql, *m_pSqlDatabase);
				if(query.next()) 
				{
					nInstallCount = query.value(0).toInt();
				}

				int nUnInstall = nClientNum - nInstallCount;
				if(nUnInstall < 0)
				{
					ServerLogger->AddLog("�������[%s]δ��װ��Ϊ[%d] �ͻ���[%d]", software.name, nUnInstall, nClientNum);
					nUnInstall = 0;
				}

				strSql = QString::fromLocal8Bit("UPDATE Software SET installNum = %1, uninstalledNum = %2 WHERE name = '%3';").arg(nInstallCount).arg(nUnInstall).arg(software.name);
				if(!excuteSql(strSql))
				{
					return false;	
				}
			}
		}

		emit sig_initData();
	}

// 	if(1 == nInstallAppNum)
// 	{
// 		QString strSql = QString::fromLocal8Bit("SELECT * FROM Software WHERE name = '%1';").arg(strPrevAppName);
// 		QSqlQuery query(strSql, *m_pSqlDatabase);
// 		while(query.next()) 
// 		{
// 			SoftwareInfo software;
// 			readSoftwareItem(software, query);
// 			emit sig_updateAllNumData(software.name, software.version, software.installNum, software.uninstalledNum);
// 			break;
// 		}
// 	}
// 	else if(nInstallAppNum > 1)
// 	{
// 		emit sig_initData();
// 	}

	if(!m_pSqlDatabase->commit())
	{
		ServerLogger->AddLog("m_pSqlDatabase->commit() ���ݿ����ʧ�ܣ�������Ϣ:[%s]", m_pSqlDatabase->lastError().text().toStdString().c_str());
		m_pSqlDatabase->rollback();
		return false; 
	}

	return true;
}

//�ر����ݿ�
void DatabaseManager::CloseDatabase()
{
	if(NULL != m_pSqlDatabase && m_pSqlDatabase->isOpen())
		m_pSqlDatabase->close();
	RELEASESQL(m_pSqlDatabase);
}

//��ȡ��ǰʹ�õ����ݿ�·��
QString DatabaseManager::GetDatabasePath()
{
	return m_strDatabasePath;
}
 
void DatabaseManager::readSoftwareItem(SoftwareInfo &software, QSqlQuery &query)
{
	software.name = query.value("name").toString();
	software.brand = query.value("brand").toString();
	software.category = query.value("category").toString();
	software.location = query.value("location").toString();
	software.mainExe = query.value("mainExe").toString();
	software.url = query.value("url").toString();
	software.version = query.value("version").toString();

	bool bOk = false;
	software.installNum = query.value("installNum").toInt(&bOk);
	if(!bOk)
		software.installNum = 0;

	software.uninstalledNum = query.value("uninstalledNum").toInt(&bOk);
	if(!bOk)
		software.uninstalledNum = 0;

	software.packType = query.value("packType").toInt(&bOk);
	if(!bOk)
		software.packType = 0;
}

QString DatabaseManager::getDataMotionSql(const DataMotion& dataMotion)
{
	//�˺�������ʹ��
	QString strSql = "";
	switch(dataMotion.msg)
	{
	case EMSG_REGISTERID:
		strSql = QString::fromLocal8Bit("INSERT INTO ClientInfo (id, version) VALUES('%1', '%2');").arg(dataMotion.clientInfo.id).arg(dataMotion.clientInfo.version);
		break;
	case EMSG_MODIFYID:
		strSql = QString::fromLocal8Bit("UPDATE ClientInfo SET id = '%1' WHERE id = '%2';").arg(dataMotion.clientInfo.id).arg(dataMotion.clientInfo.version);
		break;
	case EMSG_SOFTINSTALL:
		strSql = QString::fromLocal8Bit("UPDATE Software SET installNum = installNum + %1 WHERE name = '%2';").arg(1).arg(dataMotion.softwareInfo.name);
		break;
	default:
		break;
	}

	return strSql;
}

bool DatabaseManager::excuteSql(const QString& strSQL)
{
	if(NULL == m_pSqlDatabase || strSQL.isEmpty())
		return false;

	QSqlQuery excete(*m_pSqlDatabase);
	if(!excete.exec(strSQL))
	{
		ServerLogger->AddLog("ִ�����[%s] ���ݿ����ʧ�ܣ�������Ϣ:[%s]", strSQL.toStdString().c_str(), excete.lastError().text().toStdString().c_str());
		m_pSqlDatabase->rollback();
		return false;
	}

	return true;
}