#include "CCategoryBase.h"
#include "CMisc.h"
#include "CClientLog.h"

#include <QDir>
#include <QSettings>
#include <QTextCodec>

CCategoryBase::CCategoryBase(QObject *parent)
	: QObject(parent)
{

}

CCategoryBase::~CCategoryBase()
{

}

void CCategoryBase::SetData(const QMap<QString, SoftData>& data)
{
	m_pCategoryMap = data;
}

QMap<QString, SoftData> CCategoryBase::GetData()
{
	return m_pCategoryMap;
}


bool CCategoryBase::LocalInstall(SoftData& data)
{
	//ClientLogger->AddLog(QString::fromLocal8Bit("配置文件软件路径为：%1").arg(data.location));
	QString location;//路劲不存在时，提供一个默认路径，不然解压必然会失败
	QString strLocalDes = data.location;
	strLocalDes = strLocalDes.replace("\\", "/"); //替换原始路劲中的"\"为"/"，QString中转意字符转换
	//ClientLogger->AddLog(QString::fromLocal8Bit("删除转义符'\', 将文件路径替换为：%1").arg(strLocalDes));
	QStringList driveList = CMisc::GetLogicalDriveStringList();

	if(strLocalDes.at(1) == ":")
	{//如果原始路劲中包含盘符
		location = strLocalDes;
		QDir dir(location);
		if(dir.exists())
		{//本地文件夹存在
			location =  dir.path();
			data.location = location;
			if(IsAppInstall(data)){
				data.state = SMonitor::Normal;
				ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1] 存在软件目录[%2] 且主程序[%3]存在，软件标记为已安装").arg(data.name).arg(data.location).arg(data.mainExe));
				if(data.category != QString::fromLocal8Bit("驱动"))
					SetLocalBrand(data.brand);
				return true;
			}
			else
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1] 本地文件夹存在[%2]，但主程序[%3]不存在，软件标记为未安装").arg(data.name).arg(data.location).arg(data.mainExe));
				return false;
			}
		}
		else
		{
			//此处配置路径不存在，需要扫描本地盘符是不是存在该文件夹
			for each (auto driver in driveList)
			{
				QString nLocal = location.replace(0, 3, driver);
				//ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1]配置路径为[%2]，本地不包含所指盘符，替换为[%3]").arg(data.name).arg(location).arg(nLocal));
				location = nLocal;
				if(CMisc::IsFileExist(nLocal))
				{//存在该文件夹，则判断是否该程序是否合法
					data.location = location;
					if(IsAppInstall(data))
					{
						data.state = SMonitor::Normal;
						ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1] 存在软件目录[%2] 且主程序[%3]存在，软件标记为已安装").arg(data.name).arg(data.location).arg(data.mainExe));
						if(data.category != QString::fromLocal8Bit("驱动"))
							SetLocalBrand(data.brand);
						return true;
					}
				}
			}

			//如果所有盘符都不存在文件夹，那么需要判定配置路劲的合法性
			if(driveList.contains(strLocalDes.mid(0, 3)))
			{
				data.location = strLocalDes;//刷新软件结构路径为默认合法路径
				ClientLogger->AddLog(QString::fromLocal8Bit("[%1]本地未安装，软件路径更新为：[%2]").arg(data.name).arg(strLocalDes));
			}
			else
			{
				location = driveList[0] + strLocalDes.mid(3);
				data.location = location;//刷新软件结构路径为默认合法路径
				ClientLogger->AddLog(QString::fromLocal8Bit("[%1]本地未安装，配置路劲非法，软件路径更新为：[%2]").arg(data.name).arg(location));
			}
			return false;
		}
	}
	else
	{//源路径中未包含盘符
		if(strLocalDes.at(0) == ":") //如果源路劲以":"开始，则需要剔除":"
			strLocalDes = strLocalDes.mid(1);

		//如果源路劲以"/"开始，则需要剔除":" "\"已在最开始做出替换
		if(strLocalDes.at(0) == "/" /*|| strLocalDes.at(0) == "\\"*/)
			strLocalDes = strLocalDes.mid(1);

		for each(auto drive in driveList)
		{
			location = QString("%1%2").arg(drive).arg(strLocalDes);
			data.location = location;
			QDir dir(location);
			if(dir.exists() && IsAppInstall(data))
			{
				location =  dir.path();
				data.state = SMonitor::Normal;
				data.location = location;
				ClientLogger->AddLog(QString::fromLocal8Bit("软件[%1] 存在软件目录[%2] 且主程序[%3]存在，软件标记为已安装").arg(data.name).arg(data.location).arg(data.mainExe));
				if(data.category != QString::fromLocal8Bit("驱动"))
					SetLocalBrand(data.brand);
				return true;
			}
		}
		location = QString("%1%2").arg(driveList[0]).arg(strLocalDes);
		data.location = location;//刷新软件结构路径为默认合法路径
		ClientLogger->AddLog(QString::fromLocal8Bit("本地未安装，软件路劲更新为：%1").arg(location));
	}
	return false;
}

void CCategoryBase::SetLocalBrand(const QString& brand)
{
	//目前只需设置第一次扫描到的品牌
	if(GetLocalBrand().isEmpty()){
		ClientLogger->AddLog(QString::fromLocal8Bit("设置本地已安装品牌[%1]").arg(brand));
		QSettings settings(HKCU, QSettings::NativeFormat);
		settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

		//QString brand = settings.value("Local/Brand", QStringList()).toString();
		//brands.removeAll(brand);
		//brands.prepend(brand);
		settings.setValue("Local/Brand", brand);
	}
}

//返回已安装在本地的所有品牌的集合，可能会有多种品牌被安装在本地
//todo 返回品牌列表之后需要做进一步的处理，最终择优选取一种品牌
//注意：本函数必须在LocalInstall扫描之后才会有效
QString CCategoryBase::GetLocalBrand()const
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	return settings.value("Local/Brand", QString()).toString();
}

