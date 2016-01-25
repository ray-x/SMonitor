#include "AccessHelper.h"
#include <QFile>
#include "../../include/DateBaseHelperS.h"
#include "CClientLog.h"
#include "CMisc.h"
#include <QtWidgets/QApplication>
#include "DataManage.h"

AccessHelper::AccessHelper(QObject *parent)
	: QObject(parent)
{

}

AccessHelper::~AccessHelper()
{

}

//��ȡ�ŵ���Ϣ
bool AccessHelper::GetShopInfo(const QString& strDatabasePath, const QString& strPassword, QString& strShopId, QString& strUnitNum, QString& strUnitName)
{
	QMutexLocker locker(&m_ShopDateLock);

	if(strDatabasePath.isEmpty() || !QFile::exists(strDatabasePath))
		return false;

	QString strSql = QString::fromLocal8Bit("SELECT unit_id, unit_num, unit_name FROM sys_unit WHERE unit_id = (SELECT shop_id FROM shop)");

	//ADO
	DateBaseHelperS dataManager;
	dataManager.SetDataBase(strDatabasePath.toLocal8Bit().constData(), "deausbwsm");
	char bufError[1024] = {0};

	bool bInitSuccess = false;
	if(!dataManager.OnInitDBConnect(bufError))
	{
		if(0 == strcmp(bufError, "-2147221005"))
		{//δע��ADO
			ClientLogger->AddLog(QString::fromLocal8Bit("δע��ADO �����Զ�ע��"));
			if(RegSysMSADO())
			{
				memset(bufError, 0, sizeof(bufError));
				if(!dataManager.OnInitDBConnect(bufError))
				{
					ClientLogger->AddLog(QString::fromLocal8Bit("ע��ADO����Ȼ�޷��������ݿ�"));
				}
				else
				{
					bInitSuccess = true;
				}
			}
		}
	}
	else
	{
		bInitSuccess = true;
	}

	if(bInitSuccess)
	{
		// ִ��SQL���
		if(dataManager.CreateRecordSet(strSql.toLocal8Bit().constData()))
		{
			if(dataManager.HasRecord())
			{
				char chBuffer[1024] = {0};
				dataManager.GetCollect("unit_id", chBuffer, sizeof(chBuffer));
				strShopId = chBuffer;

				memset(chBuffer, 0, sizeof(chBuffer));
				dataManager.GetCollect("unit_num", chBuffer, sizeof(chBuffer));
				strUnitNum = chBuffer;

				memset(chBuffer, 0, sizeof(chBuffer));
				dataManager.GetCollect("unit_name", chBuffer, sizeof(chBuffer));
				strUnitName = QString::fromLocal8Bit(chBuffer);
			}
		}
	}
	else
	{//����ODBC

		ClientLogger->AddLog(QString::fromLocal8Bit("ADO���Ӳ��ϣ�����ODBC��ʽ"));
		CODBCDataManage odbcDataManager(strDatabasePath.toLocal8Bit().constData(), "deausbwsm");

		memset(bufError, 0, sizeof(bufError));
		if(!odbcDataManager.OnInitDBConnect(bufError))
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("ODBC��ʽ����ʧ�ܣ�������Ϣ[%1]").arg(bufError));
			return false;
		}

		if(odbcDataManager.CreateRecordSet(strSql.toLocal8Bit().constData(), bufError))
		{
			if(odbcDataManager.HasRecord())
			{
				strShopId = odbcDataManager.GetCollect("unit_id").c_str();
				strUnitNum = odbcDataManager.GetCollect("unit_num").c_str();
				strUnitName = QString::fromLocal8Bit(odbcDataManager.GetCollect("unit_name").c_str());
				int n = 2;
			}
		}
	}
	
	return true;
// 	QSqlDatabase access = QSqlDatabase::addDatabase("QSQLITE");
// 	if(!access.isValid())
// 	{//��SQLITE����ʧ��
// 		return false;
// 	}
// 
// 	access.setDatabaseName(strDatabasePath);
// 	if(!access.isValid())
// 	{
// 		return false;
// 	}
// 
// 	access.setPassword(strPassword);
// 
// 	if(!access.open())
// 	{
// 		return false;
// 	}
// 
// 	QString strSql = QString::fromLocal8Bit("SELECT unit_id, unit_num, unit_name FROM sys_unit WHERE unit_id = (SELECT shop_id FROM shop)");
// 	QSqlQuery query(strSql, access);
// 	bool bRet = false;
// 	if(query.next()) 
// 	{
// 		strShopId = query.value("unit_id").toString();
// 		strUnitNum = query.value("unit_num").toString();
// 		strUnitName = query.value("unit_name").toString();
// 		bRet = true;
// 	}
// 
// 	access.close();
// 	return bRet;
}

//ע��ϵͳADO
bool AccessHelper::RegSysMSADO()
{
	bool bRet = false;
	if(CMisc::IsOsVerHigherXP())
	{
		if(QFile::exists("C:/Program Files/Common Files/System/ado/msado15.dll"))
		{
			bRet = CMisc::ExcuteCmd("regsvr32 /s \"C:/Program Files/Common Files/System/ado/msado15.dll\"");
		}

		if(QFile::exists("C:/Program Files (x86)/Common Files/System/ado/msado15.dll"))
		{
			bRet = CMisc::ExcuteCmd("regsvr32 /s \"C:/Program Files (x86)/Common Files/System/ado/msado15.dll\"");
		}
	}
	else
	{
		if(QFile::exists("C:/Program Files/Common Files/System/ado/msado15.dll"))
		{
			bRet = CMisc::ExcuteCmd("regsvr32 /s \"C:/Program Files/Common Files/System/ado/msado15.dll\"");
		}
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("ϵͳado��ע�� ���[%1]").arg(bRet));

	return bRet;
}

//ע�᱾��ADO
bool AccessHelper::RegLocalMSADO()
{
	QString path = QApplication::applicationDirPath() + "/msado15.dll";
	bool bRet = false;
	if(QFile::exists(path))
	{
		bRet = CMisc::ExcuteCmd(QString::fromLocal8Bit("regsvr32 /s %1").arg(path));
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("����ado������[%1]").arg(path));
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("����ado��ע�� ���[%1]").arg(bRet));

	return bRet;
}