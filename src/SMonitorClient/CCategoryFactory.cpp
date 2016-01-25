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
		//��������ֻ���ڰ�װ���ʱ�Żᱻ�ı䣬
		//���Կ�����Ϊ�����������Ѱ�װ����ļ���
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
					//���������Ѱ�װ������Ҫ��¼�Ѿ���װ��Ʒ��
					if(iter2.value().category != QString::fromLocal8Bit("����"))
						SetLocalBrand(iter2.value().brand);
				}
			}
		}

		emit dataChangedNotify(m_pLocalData);
		ClientLogger->AddLog(QString::fromLocal8Bit("���ر������ݳɹ�"));
	}
}

//������³ɹ�֮�󣬱����������ļ�
void CCategoryFactory::SetLocalData(const SoftData& data)
{
	bool bReturn = false;
	CMarkup xml;
	QString path = QApplication::applicationDirPath() + localSoftFile;
	QMutexLocker locker(&m_pXmlFileMutex);
	if(CMisc::IsFileExist(path))
	{//���ڱ�������
		if(xml.Load(path.toLocal8Bit().constData()))
		{
			while(xml.FindChildElem("SoftwareList"))
			{
				xml.IntoElem();
				bool bExist = false;
				//���Ȳ������е�����б������ڸ��������£������ڣ�����Ҫ�½��ڵ�
				while(xml.FindChildElem("Software"))
				{
					xml.IntoElem();
					if((QString::fromStdString(xml.GetAttrib("name")) ==  data.name)&&
						(QString::fromStdString(xml.GetAttrib("brand")) == data.brand) && 
						(QString::fromStdString(xml.GetAttrib("category")) == data.category))
					{//���±�������					
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

				//�����ڸ��������ֱ���ؽ�
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
	{//�����ڱ������ݣ��򴴽���������
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

//��XMLɾ������
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

//��һ��xml�ļ��л�ȡ����б�
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

//������ݱ仯��֪ͨUi���и���
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
				if(iter.key() != QString::fromLocal8Bit("��ҳ"))
				{//������ҳ֮�����������Ͷ���Ҫ���и�����ʾ
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

			if(iter.key() != QString::fromLocal8Bit("��ҳ") && !bNeedInstallUpdate)
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

	//���±�������
	UpdateBrandPasswdBySvrFile();

	if(bNeedInstallUpdate)
	{
		emit upgradeNotify(SMonitor::Upgrade);
	}

// 	QStringList keys = UpgradeData.keys();
// 	if(keys.contains(QString::fromLocal8Bit("��ҳ")))
// 	{
// 		keys.removeAll(QString::fromLocal8Bit("��ҳ"));
// 	}
// 	
// 	if(keys.count() > 0)
// 		emit upgradeNotify(SMonitor::Upgrade);

	emit dataChangedNotify(UpgradeData);

	//�ύ���������Ϣ
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
	if(category == QString::fromLocal8Bit("����"))
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
	else if(category == QString::fromLocal8Bit("��ҳ"))
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
		ClientLogger->AddLog(QString::fromLocal8Bit("���[%1]��װ�ɹ���д�뱾������").arg(name));
		
		SetLocalData(data);

		if(data.category != QString::fromLocal8Bit("����"))
			SetLocalBrand(data.brand);//��¼�Ѿ���װ��Ʒ��

		//������Ҫע�⣬����״�ɨ��û���κ�Ʒ�ư�װ���ڰ�װ��һ��Ʒ�Ƶ����֮��
		//���ܻᵼ����Ҫ���µ�����������������٣�������Ҫ����Ʒ�ƾ���
		m_pUpgradeData.remove(name);
		//DATASOFTITERATOR iter(m_pUpgradeData);
		//while(iter.hasNext())
		//{
		//	iter.next();
		//	if((iter.value().category == QString::fromLocal8Bit("ERP")||
		//		iter.value().category == QString::fromLocal8Bit("����"))&&
		//		iter.value().brand != data.brand)
		//	{//���Ʒ�Ʋ�ͬ����������º���ʾ
		//		m_pUpgradeData.remove(iter.key());
		//	}
		//	else if(iter.value().category == QString::fromLocal8Bit("����"))
		//	{//����û��Ʒ�����֣�ֱ��ɾ��
		//		m_pUpgradeData.remove(iter.key());
		//	}
		//	else if(iter.value().category == QString::fromLocal8Bit("��ҳ"))
		//	{//��ҳû�а�װ�͸��£�����ȫ��ɾ��
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
			ClientLogger->AddLog(QString::fromLocal8Bit("���и���(��װ)������ȫ����ɣ���ʼ��������״̬"));
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

//�����Ѱ�װ�ڱ��ص�����Ʒ�Ƶļ��ϣ����ܻ��ж���Ʒ�Ʊ���װ�ڱ���
//todo ����Ʒ���б�֮����Ҫ����һ���Ĵ�����������ѡȡһ��Ʒ��
//ע�⣺������������LocalInstallɨ��֮��Ż���Ч
QString CCategoryFactory::GetLocalBrand()const
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	return settings.value("Local/Brand", QString()).toString();
}

//���±������룬����ʱ�����ĳ��Ʒ�Ƶ�����ı䣬����Ҫ�����������������
//���б����޳�
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
		if(passwd != iter.value()) //��������������
		{
			CMisc::SetBrandsNeedPasswd(iter.key());
			CMisc::SetBrandPasswd(iter.key(), iter.value());
		}
	}
}

//�ж�����Ƿ�װ
bool CCategoryFactory::IsAppInstall(const SoftData& data)
{
	if(data.category == QString::fromLocal8Bit("����"))
	{
		QString strDrive = data.name.split("##").first();
		return CMisc::IsAppInstall(strDrive);
	}
	else if(data.category == QString::fromLocal8Bit("ERP"))
	{
		if(data.mainExe.isEmpty())
		{
			//��EXEû�����ã��ж�Ŀ��Ŀ¼�Ƿ����
			return CMisc::IsFileExist(data.location);
		}

		QString mainPath = data.location + "/" + data.mainExe;
		//ClientLogger->AddLog(QString::fromLocal8Bit("�жϱ������·����%1 �Ƿ����").arg(mainPath));
		return CMisc::IsFileExist(mainPath);
	}
	else if(data.category == QString::fromLocal8Bit("��ҳ"))
	{
		return true;
	}
	else
	{
		if(data.mainExe.isEmpty())
		{
			//��EXEû�����ã��ж�Ŀ��Ŀ¼�Ƿ����
			return CMisc::IsFileExist(data.location);
		}

		QString mainPath = data.location + "/" + data.mainExe;
		//ClientLogger->AddLog(QString::fromLocal8Bit("�жϱ������·����%1 �Ƿ����").arg(mainPath));
		return CMisc::IsFileExist(mainPath);
	}
}