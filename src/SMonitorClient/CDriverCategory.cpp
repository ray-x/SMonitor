#include "CDriverCategory.h"
#include "CMisc.h"
#include "CClientLog.h"

CDriverCategory::CDriverCategory(QObject *parent)
	: CCategoryBase(parent)
{

}

CDriverCategory::~CDriverCategory()
{

}

QString CDriverCategory::Category() const
{
	return QString::fromLocal8Bit("驱动");
}

//判断当前驱动是否已经安装至本地
bool CDriverCategory::IsAppInstall(const SoftData& data)
{
// 	QString strDrive = "";
// 	if(data.brand == QString::fromLocal8Bit("爱普生"))
// 	{
// 		strDrive = "EPSON";
// 	}
// 	else
// 	{
// 		ClientLogger->AddLog(QString::fromLocal8Bit("发现未知的驱动品牌[%1]").arg(data.brand));
// 		return false;
// 	}
// 
// 	if(strDrive.isEmpty())
// 		return false;

	QString strDrive = data.name.split("##").first();
	return CMisc::IsAppInstall(strDrive);
}

const QMap<QString, SoftData> CDriverCategory::UpgradeSoftwareData()
{
	QMap<QString, SoftData> upgradeMap;
	QMapIterator<QString, SoftData> iter(GetData());
	while(iter.hasNext())
	{
		iter.next();
		SoftData data = iter.value();
		QString strKey = data.name.split("##").first();
		//1、判断当前驱动是否兼容当前系统
		if(CMisc::IsOSSuit(data.name))
		{
			if(IsAppInstall(data))
			{//本地已安装，则不显示升级
				data.state = SMonitor::Normal; //状态为已安装
				upgradeMap.insert(strKey, data);
			}
			else
			{
				data.state = SMonitor::UnInstall; //状态未安装
				emit MessageNotify(QDateTime::currentDateTime(), 
					QString::fromLocal8Bit("驱动程序 [%1] 可安装至本机，请及时安装。")
					.arg(strKey).arg(data.version));
				ClientLogger->AddLog(QString::fromLocal8Bit("驱动程序 [%1] 可安装至本机，请及时安装。")
					.arg(data.name).arg(data.version));
				upgradeMap.insert(strKey, data);
			}
		}
		else
		{//系统不兼容或者兼容但已安装，则丢弃该驱动
			//todo nothing
			ClientLogger->AddLog(QString::fromLocal8Bit("驱动程序不兼容当前系统，请联系管理员"));
		}
	}
	return upgradeMap;
}