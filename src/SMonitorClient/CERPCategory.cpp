#include "CERPCategory.h"
#include "CCategoryFactory.h"
#include "CMisc.h"
#include  "CClientLog.h"

#include <QFile>

CERPCategory::CERPCategory(QObject *parent)
	: CCategoryBase(parent)
{

}

CERPCategory::~CERPCategory()
{

}

QString CERPCategory::Category() const
{
	return QString::fromLocal8Bit("ERP");
}


//ERP类判断是否安装需要判断程序exe文件是否存在
bool CERPCategory::IsAppInstall(const SoftData& data)
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

//判断该类型中是否存在需要升级(安装)的软件
const QMap<QString, SoftData> CERPCategory::UpgradeSoftwareData()
{
	QMap<QString, SoftData> srcData = GetData();
	QMap<QString, SoftData> upgradeData;
	//本地没有任何品牌安装
	if(GetLocalBrand().isEmpty())
	{
		//本地没有品牌安装，那么就需要进行一次扫描，以确定本地
		//是否存在已安装的软件，扫描出的列表可能含有多种品牌，
		//那么扫描结束后需要做进一步的处理
		QMapIterator<QString, SoftData> iter(srcData);
		while(iter.hasNext())
		{
			iter.next();
			SoftData data = iter.value();
			//此处的本地文件夹扫描，只是为了设置确定已安装的品牌
			LocalInstall(data);
			if(data.state == SMonitor::Normal || data.state == SMonitor::Upgrade)
			{
				//本地数据已安装，则需要记录已经安装的品牌
				SetLocalBrand(data.brand);
			}
			//本地扫描后，会有合法路径得出，所以这里要重新进行赋值
			srcData.insert(data.name, data);
		}
	}

	QString brand = GetLocalBrand();
	if(brand.isEmpty())
	{
		//由于任何软件的安装都会改变本地安装brand, 扫描后本地未存在任何已安装
		//的品牌，那么则表示为全新的环境，则服务器数据需要全部显示
		QMapIterator<QString, SoftData> iter(srcData);
		while(iter.hasNext())
		{
			iter.next();
			SoftData data = iter.value();
			//此处已经确定无任何品牌安装，所以无需再判断是否安装至本地
			//LocalInstall(data);
			emit MessageNotify(QDateTime::currentDateTime(), 
				QString::fromLocal8Bit("客户端可安装软件 [%1](版本[%2]),请及时安装")
				.arg(data.name).arg(data.version));
			ClientLogger->AddLog(QString::fromLocal8Bit("客户端可安装软件 [%1](版本[%2]),请及时安装")
				.arg(data.name).arg(data.version));
			upgradeData.insert(data.name, data);
		}
	}
	else
	{
		QMap<QString, SoftData> LocalData = CategoryFactory->LocalData().value(Category(), QMap<QString, SoftData> ());
		QMapIterator<QString, SoftData> iter(srcData);
		while(iter.hasNext())
		{
			iter.next();
			SoftData data = iter.value();
			if(data.brand == brand) //只显示已安装品牌的软件
			{
				if(LocalData.contains(iter.key()))//本地数据包含
				{
					const SoftData& lData = LocalData.value(iter.key());
					if(CMisc::IsNeedUpdate(lData.version, data.version))
					{
						//软件需要进行升级
						data.state = SMonitor::Upgrade;
						emit MessageNotify(QDateTime::currentDateTime(), 
							QString::fromLocal8Bit("客户端软件 [%1] 可更新至版本 [%2]")
							.arg(data.name).arg(data.version));
						ClientLogger->AddLog(QString::fromLocal8Bit("客户端软件 [%1] 可更新至版本 [%2]")
							.arg(data.name).arg(data.version));
						upgradeData.insert(data.name, data);
					}
					else
					{//无需升级，需要检查一下程序是否还存在
						if(!LocalInstall(data))
						{//已经不存在了
							upgradeData.insert(data.name, data);
							emit MessageNotify(QDateTime::currentDateTime(), 
								QString::fromLocal8Bit("客户端可安装软件 [%1](版本[%2]),请及时安装")
								.arg(data.name).arg(data.version));
							ClientLogger->AddLog(QString::fromLocal8Bit("客户端可安装软件 [%1](版本[%2]),请及时安装")
								.arg(data.name).arg(data.version));
						}
						else
						{
							if(data.version.isEmpty())
								continue;

							//本地已安装，则需要比较版本号，判断是否需要进行升级
							QString version = CMisc::GetVersionFileVersion(data.location);
							if(CMisc::IsNeedUpdate(version, data.version))
							{
								//软件需要进行升级
								data.state = SMonitor::Upgrade;
								upgradeData.insert(data.name, data);
								emit MessageNotify(QDateTime::currentDateTime(), 
									QString::fromLocal8Bit("客户端软件 [%1] 可更新至版本 [%2]")
									.arg(data.name).arg(data.version));
								ClientLogger->AddLog(QString::fromLocal8Bit("客户端软件 [%1] 可更新至版本 [%2]")
									.arg(data.name).arg(data.version));
							}
						}
					}
				}
				else
				{
					//判断本地是否已经安装
					if(LocalInstall(data))
					{
						//本地已安装，则需要比较版本号，判断是否需要进行升级
						QString version = CMisc::GetVersionFileVersion(data.location);
						if(CMisc::IsNeedUpdate(version, data.version))
						{
							//软件需要进行升级
							data.state = SMonitor::Upgrade;
							upgradeData.insert(data.name, data);
							emit MessageNotify(QDateTime::currentDateTime(), 
								QString::fromLocal8Bit("客户端软件 [%1] 可更新至版本 [%2]")
								.arg(data.name).arg(data.version));
							ClientLogger->AddLog(QString::fromLocal8Bit("客户端软件 [%1] 可更新至版本 [%2]")
								.arg(data.name).arg(data.version));
						}
						else
						{//本地已安装，但是本地数据中不包含，那么也需要显示
							data.state = SMonitor::Normal;
							upgradeData.insert(data.name, data);
						}
					}
					else
					{
						upgradeData.insert(data.name, data);
						emit MessageNotify(QDateTime::currentDateTime(), 
							QString::fromLocal8Bit("客户端可安装软件 [%1](版本[%2]),请及时安装")
							.arg(data.name).arg(data.version));
						ClientLogger->AddLog(QString::fromLocal8Bit("客户端可安装软件 [%1](版本[%2]),请及时安装")
							.arg(data.name).arg(data.version));
					}
				}
			}
		}
		
	}

	return upgradeData;
}

