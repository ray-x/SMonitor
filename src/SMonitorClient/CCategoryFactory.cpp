#include "CCategoryFactory.h"
#include "Markup.h"
#include "CMisc.h"
#include "CClientLog.h"
#include "CDriverCategory.h"
#include "CERPCategory.h"
#include "CWebCategory.h"
#include "COtherCategory.h"
#include "CMessageDataMediator.h"
#include "NetClass.h"
#include "NetClt.h"

#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTextCodec>
#include <QDebug>

CCategoryFactory* CCategoryFactory::m_Instance = nullptr;

CCategoryFactory* CCategoryFactory::Instance()
{
	if(nullptr == m_Instance)
	{
		m_Instance = new CCategoryFactory();
		Q_ASSERT(m_Instance);
	}
	return m_Instance;
}

CCategoryFactory::CCategoryFactory(QObject *parent)
	: QObject(parent)
{

}

CCategoryFactory::~CCategoryFactory()
{

}

DATA CCategoryFactory::LocalData() const
{
	return m_pLocalData;
}


void CCategoryFactory::LoadLocalData()
{
	QString path = QApplication::applicationDirPath() + localSoftFile;
	DATA curMap;
	if(LoadData(path, m_pLocalData, true))
	{
		//本地数据只有在安装完成时才会被改变，
		//所以可以认为本地数据是已安装软件的集合
		DATATTERATOR iter(m_pLocalData);
		while(iter.hasNext())
		{
			iter.next();
			DATASOFTITERATOR iter2(iter.value());
			while(iter2.hasNext())
			{
				iter2.next();
				
				if(iter2.value().state == SMonitor::Normal || iter2.value().state == SMonitor::Upgrade)
				{
					//本地数据已安装，则需要记录已经安装的品牌
					if(iter2.value().category != QString::fromLocal8Bit("驱动"))
						SetLocalBrand(iter2.value().brand);
				}
			}
		}

		emit dataChangedNotify(m_pLocalData);
		ClientLogger->AddLog(QString::fromLocal8Bit("加载本地数据成功"));
	}
}

//软件更新成功之后，保存至本地文件
void CCategoryFactory::SetLocalData(const SoftData& data)
{
	bool bReturn = false;
	CMarkup xml;
	QString path = QApplication::applicationDirPath() + localSoftFile;
	QMutexLocker locker(&m_pXmlFileMutex);
	if(CMisc::IsFileExist(path))
	{//存在本地数据
		if(xml.Load(path.toLocal8Bit().constData()))
		{
			while(xml.FindChildElem("SoftwareList"))
			{
				xml.IntoElem();
				bool bExist = false;
				//首先查找所有的软件列表，若存在该软件则更新，不存在，则需要新建节点
				while(xml.FindChildElem("Software"))
				{
					xml.IntoElem();
					if((QString::fromStdString(xml.GetAttrib("name")) ==  data.name)&&
						(QString::fromStdString(xml.GetAttrib("brand")) == data.brand) && 
						(QString::fromStdString(xml.GetAttrib("category")) == data.category))
					{//更新本地数据					
						xml.SetAttrib("version", data.version.toStdString());
						xml.SetAttrib("url", data.url.toStdString());
						xml.SetAttrib("location", data.location.toStdString());
						xml.SetAttrib("state", data.state);
						xml.SetAttrib("mainexe", data.mainExe.toStdString());
						bExist = true;
						break;
					}
					xml.OutOfElem();
				}

				//不存在该软件，则直接重建
				if(!bExist)
				{
					xml.AddChildElem("Software");
					xml.IntoElem();
					xml.AddAttrib("name", data.name.toStdString());
					xml.AddAttrib("version", data.version.toStdString());
					xml.AddAttrib("url", data.url.toStdString());
					xml.AddAttrib("brand", data.brand.toStdString());
					xml.AddAttrib("category", data.category.toStdString());
					xml.AddAttrib("location", data.location.toStdString());
					xml.AddAttrib("state", data.state);
					xml.AddAttrib("mainexe", data.mainExe.toStdString());
					xml.OutOfElem();
				}
				xml.OutOfElem();
			}
		}
		xml.Save(path.toLocal8Bit().constData());
	}
	else
	{//不存在本地数据，则创建本地数据
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("Root");
		xml.AddChildElem("SoftwareList");
		xml.IntoElem();
		xml.AddChildElem("Software");
		xml.IntoElem();
		xml.AddAttrib("name", data.name.toStdString());
		xml.AddAttrib("version", data.version.toStdString());
		xml.AddAttrib("url", data.url.toStdString());
		xml.AddAttrib("brand", data.brand.toStdString());
		xml.AddAttrib("category", data.category.toStdString());
		xml.AddAttrib("location", data.location.toStdString());
		xml.AddAttrib("mainexe", data.mainExe.toStdString());
		xml.AddAttrib("state", data.state);
		xml.OutOfElem();
		xml.OutOfElem();
		xml.Save(path.toLocal8Bit().constData());
	}
}

//从XML删除数据
void CCategoryFactory::DelLocalData(const SoftData& data)
{
	QMutexLocker locker(&m_pXmlFileMutex);
	QString path = QApplication::applicationDirPath() + localSoftFile;
	CMarkup xml;
	if(!xml.Load(path.toLocal8Bit().constData()))
	{
		return;
	}

	if(!xml.FindChildElem("SoftwareList"))
		return;

	xml.IntoElem();

	bool bFind = false;
	while(!bFind && xml.FindChildElem("Software"))
	{
		xml.IntoElem();
		QString strName = QString::fromStdString(xml.GetAttrib("name"));
		if(strName == data.name)
		{
			xml.RemoveElem();
			bFind = true;
			break;
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();

	if(bFind)
	{
		xml.Save(path.toLocal8Bit().constData());
	}
}

//从一个xml文件中获取软件列表
bool CCategoryFactory::LoadData(const QString& file, DATA& data, bool bLocal)
{
	bool bReturn = false, bUpdate = false;
	bool bDel = false;
	CMarkup xml;
	QMutexLocker locker(&m_pXmlFileMutex);
	if(xml.Load(file.toLocal8Bit().constData()))
	{
		if(xml.FindChildElem("SoftwareList"))
		{
			bReturn = true;
			xml.IntoElem();
			QMultiMap<QString, SoftData> Multimap;
			int index = 0;
			while(xml.FindChildElem("Software"))
			{
				xml.IntoElem();
				SoftData sData;
				sData.name = QString::fromStdString(xml.GetAttrib("name"));
				sData.version = QString::fromStdString(xml.GetAttrib("version"));
				sData.url = QString::fromStdString(xml.GetAttrib("url"));
				sData.brand = QString::fromStdString(xml.GetAttrib("brand"));
				sData.category = QString::fromStdString(xml.GetAttrib("category"));
				sData.location = QString::fromStdString(xml.GetAttrib("location"));
				sData.mainExe = QString::fromStdString(xml.GetAttrib("mainexe"));
				sData.state = QString::fromStdString(xml.GetAttrib("state")).toInt();
				if(sData.state == SMonitor::Normal || sData.state == SMonitor::Upgrade)
				{
					if(!IsAppInstall(sData))
					{
						sData.state = SMonitor::UnInstall;
					}
				}

				if(bLocal && sData.state == SMonitor::UnInstall)
				{
					xml.RemoveElem();
					xml.OutOfElem();
					bDel = true;
					continue;
				}

				xml.OutOfElem();
				Multimap.insert(sData.category, sData);
			}

			for each(auto key in Multimap.keys())
			{
				QList<SoftData> stdatas = Multimap.values(key);
				QMap<QString, SoftData> dataMap;
				for each(auto value in stdatas)
					dataMap.insert(value.name, value);
				data.insert(key, dataMap);
			}
			xml.OutOfElem();
		}
	}

	if(bDel)
	{
		xml.Save(file.toLocal8Bit().constData());
	}

	return bReturn;
}

//监测数据变化，通知Ui进行更新
void CCategoryFactory::CheckUpgradeData() 
{
	m_pUpgradeData.clear();
	QString svrFile = QApplication::applicationDirPath() + "/Svr.data";
	DATA UpgradeData, SvrData;
	bool bNeedInstallUpdate = false;
	if(LoadData(svrFile,SvrData, false))
	{
		DATATTERATOR iter(SvrData);
		while(iter.hasNext())
		{
			iter.next();
			qDebug() << iter.key();
			DATASOFT data = GetCategoryUpgradeData(iter.key(), iter.value());
			if(data.count() > 0){
				UpgradeData.insert(iter.key(), data);
				if(iter.key() != QString::fromLocal8Bit("网页"))
				{//除了网页之外其他的类型都需要进行更新提示
					QMapIterator<QString, SoftData> iter2(data);
					while(iter2.hasNext()){
						iter2.next();
						if(iter2.value().state == SMonitor::Upgrade 
							|| iter2.value().state == SMonitor::UnInstall)
						{
							m_pUpgradeData.unite(data);
						}
					}
				}
			}

			if(iter.key() != QString::fromLocal8Bit("网页") && !bNeedInstallUpdate)
			{
				QMapIterator<QString, SoftData> iter2(data);
				while(iter2.hasNext())
				{
					iter2.next();
					if(iter2.value().state == SMonitor::Upgrade)
					{
						bNeedInstallUpdate = true;
						break;
					}
				}
			}
		}
	}

	//更新本地密码
	UpdateBrandPasswdBySvrFile();

	if(bNeedInstallUpdate)
	{
		emit upgradeNotify(SMonitor::Upgrade);
	}

// 	QStringList keys = UpgradeData.keys();
// 	if(keys.contains(QString::fromLocal8Bit("网页")))
// 	{
// 		keys.removeAll(QString::fromLocal8Bit("网页"));
// 	}
// 	
// 	if(keys.count() > 0)
// 		emit upgradeNotify(SMonitor::Upgrade);

	emit dataChangedNotify(UpgradeData);

	//提交本地软件信息
	if(MessageDataMediator->m_bIsCanConnectedTime)
	{	
		bool bRecved = false;
		if(MessageDataMediator->getConnectMsg(EMSG_SOFTUPLOAD, bRecved) && !bRecved)
		{
			NetClass->m_pNetClt->SendMsg(EMSG_SOFTUPLOAD, true);
		}

		MessageDataMediator->setConnectMsgMapProperty(EMSG_REQUESTXML, true);
	}

	MessageDataMediator->m_bClientStartOver = true;
	//NetClass->m_pNetClt->SendTextMsg();
}

DATASOFT CCategoryFactory::GetCategoryUpgradeData(const QString& category, const DATASOFT& SvrData) const
{
	DATASOFT dataMap;
	if(category == QString::fromLocal8Bit("驱动"))
	{
		CDriverCategory CDriver;
		connect(&CDriver, &CCategoryBase::MessageNotify, this, &CCategoryFactory::MessageNotify);
		CDriver.SetData(SvrData);
		dataMap =  CDriver.UpgradeSoftwareData();
	}
	else if(category == QString::fromLocal8Bit("ERP"))
	{
		CERPCategory CERP;
		connect(&CERP, &CCategoryBase::MessageNotify, this, &CCategoryFactory::MessageNotify);
		CERP.SetData(SvrData);
		dataMap = CERP.UpgradeSoftwareData();
	}
	else if(category == QString::fromLocal8Bit("网页"))
	{
		CWebCategory CWeb;
		connect(&CWeb, &CCategoryBase::MessageNotify, this, &CCategoryFactory::MessageNotify);
		CWeb.SetData(SvrData);
		dataMap = CWeb.UpgradeSoftwareData();
	}
	else
	{
		COtherCategory COther;
		connect(&COther, &CCategoryBase::MessageNotify, this, &CCategoryFactory::MessageNotify);
		COther.SetData(SvrData);
		dataMap = COther.UpgradeSoftwareData();
	}

	return dataMap;
}

void CCategoryFactory::InstallFinished(const QString& name)
{
	if(m_pUpgradeData.contains(name))
	{
		SoftData data = m_pUpgradeData.value(name);
		data.state = SMonitor::Normal;
		ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1]安装成功，写入本地数据").arg(name));
		
		SetLocalData(data);

		if(data.category != QString::fromLocal8Bit("驱动"))
			SetLocalBrand(data.brand);//记录已经安装的品牌

		//这里需要注意，如果首次扫描没有任何品牌安装，在安装完一个品牌的软件之后
		//可能会导致需要更新的软件数量上有所减少，所以需要按照品牌精简
		m_pUpgradeData.remove(name);
		//DATASOFTITERATOR iter(m_pUpgradeData);
		//while(iter.hasNext())
		//{
		//	iter.next();
		//	if((iter.value().category == QString::fromLocal8Bit("ERP")||
		//		iter.value().category == QString::fromLocal8Bit("其他"))&&
		//		iter.value().brand != data.brand)
		//	{//如果品牌不同，则无需更新和显示
		//		m_pUpgradeData.remove(iter.key());
		//	}
		//	else if(iter.value().category == QString::fromLocal8Bit("驱动"))
		//	{//驱动没有品牌区分，直接删除
		//		m_pUpgradeData.remove(iter.key());
		//	}
		//	else if(iter.value().category == QString::fromLocal8Bit("网页"))
		//	{//网页没有安装和更新，所以全部删除
		//		m_pUpgradeData.remove(iter.key());
		//		/*if(!iter.value().brand.isEmpty() &&iter.value().brand != data.brand)
		//		{
		//			m_pUpgradeData.remove(iter.key());
		//		}*/
		//	}
		//}

		if(m_pUpgradeData.count() <= 0)
		{
			emit upgradeNotify(SMonitor::Normal);
			ClientLogger->AddLog(QString::fromLocal8Bit("所有更新(安装)操作已全部完成，开始更新托盘状态"));
		}
		emit SoftwareInstalled(name);
	}
}

void CCategoryFactory::SetLocalBrand(const QString& brand)
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

	//QString brand = settings.value("Local/Brand", QStringList()).toString();
	//brands.removeAll(brand);
	//brands.prepend(brand);
	settings.setValue("Local/Brand", brand);
	CMisc::SetBrandsNotNeedPasswd(brand);
}

//返回已安装在本地的所有品牌的集合，可能会有多种品牌被安装在本地
//todo 返回品牌列表之后需要做进一步的处理，最终择优选取一种品牌
//注意：本函数必须在LocalInstall扫描之后才会有效
QString CCategoryFactory::GetLocalBrand()const
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	return settings.value("Local/Brand", QString()).toString();
}

//更新本地密码，更新时，如果某个品牌的密码改变，则需要将其从无需输入密码
//的列表中剔除
void CCategoryFactory::UpdateBrandPasswdBySvrFile()
{
	QString svrFile = QApplication::applicationDirPath() + "/Svr.data";
	QMap<QString, QString> brandPasswdMap;
	{
		CMarkup xml;
		QMutexLocker locker(&m_pXmlFileMutex);
		if(xml.Load(svrFile.toLocal8Bit().constData()))
		{
			if(xml.FindChildElem("PasswdList"))
			{
				xml.IntoElem();
				while(xml.FindChildElem("Passwd"))
				{
					xml.IntoElem();
					QString brand = QString::fromStdString(xml.GetAttrib("brand"));
					QString passwd = QString::fromStdString(xml.GetAttrib("passwd"));
					brandPasswdMap.insert(brand, passwd);
					xml.OutOfElem();
				}
				xml.OutOfElem();
			}
		}
	}

	QMapIterator<QString, QString> iter(brandPasswdMap);
	while(iter.hasNext())
	{
		iter.next();
		QString passwd = CMisc::GetBrandPasswd(iter.key());
		if(passwd != iter.value()) //服务器更新密码
		{
			CMisc::SetBrandsNeedPasswd(iter.key());
			CMisc::SetBrandPasswd(iter.key(), iter.value());
		}
	}
}

//判断软件是否安装
bool CCategoryFactory::IsAppInstall(const SoftData& data)
{
	if(data.category == QString::fromLocal8Bit("驱动"))
	{
		QString strDrive = data.name.split("##").first();
		return CMisc::IsAppInstall(strDrive);
	}
	else if(data.category == QString::fromLocal8Bit("ERP"))
	{
		if(data.mainExe.isEmpty())
		{
			//主EXE没有配置，判断目标目录是否存在
			return CMisc::IsFileExist(data.location);
		}

		QString mainPath = data.location + "/" + data.mainExe;
		//ClientLogger->AddLog(QString::fromLocal8Bit("判断本地软件路劲：%1 是否存在").arg(mainPath));
		return CMisc::IsFileExist(mainPath);
	}
	else if(data.category == QString::fromLocal8Bit("网页"))
	{
		return true;
	}
	else
	{
		if(data.mainExe.isEmpty())
		{
			//主EXE没有配置，判断目标目录是否存在
			return CMisc::IsFileExist(data.location);
		}

		QString mainPath = data.location + "/" + data.mainExe;
		//ClientLogger->AddLog(QString::fromLocal8Bit("判断本地软件路劲：%1 是否存在").arg(mainPath));
		return CMisc::IsFileExist(mainPath);
	}
}