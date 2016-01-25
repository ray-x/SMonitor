#ifndef CCATEGORYFACTORY_H
#define CCATEGORYFACTORY_H

#include <QObject>
#include "common.h"

class CDriverCategory;
class CERPCategory;
class CWebCategory;

#define DATA QMap<QString, QMap<QString, SoftData> > 
#define DATATTERATOR QMapIterator<QString, QMap<QString, SoftData> > 
#define DATASOFT QMap<QString, SoftData>
#define DATASOFTITERATOR QMapIterator<QString, SoftData>

class CCategoryFactory : public QObject
{
	Q_OBJECT

public:
	static CCategoryFactory* Instance();
	~CCategoryFactory();
	//获取本地数据
	DATA LocalData()const;
	//加载本地数据
	void LoadLocalData();
	//返回已经安装至本地的品牌类型
	QString GetLocalBrand()const;

Q_SIGNALS:
	//软件更新广播消息
	void upgradeNotify(SMonitor::Type type = SMonitor::Upgrade); 
	//数据更新通知消息
	void dataChangedNotify(const DATA&); 
	//消息分发
	void MessageNotify(const QDateTime& , const QString& );
	//软件安装成功后提交安装完成信号
	void SoftwareInstalled(const QString& name);

public Q_SLOTS:
	//软件安装成功流程
	void InstallFinished(const QString& name); 
	//获取服务器数据后得到可更新数据
	void CheckUpgradeData() ;

private:
	static CCategoryFactory* m_Instance;

	//从xml文件中读取数据
	bool LoadData(const QString& file, DATA& data, bool bLocal);

	//修改本地数据软件信息
	void SetLocalData(const SoftData& data);

	//从XML删除数据
	void DelLocalData(const SoftData& data);

	//设置某个品牌为已安装类型
	void SetLocalBrand(const QString& brand);
	
	//获取某个类型的软件更新数据
	DATASOFT GetCategoryUpgradeData(const QString& category, const DATASOFT& SvrData) const;

	//获取xml中的密码配置信息
	void UpdateBrandPasswdBySvrFile();

	//判断软件是否安装
	bool IsAppInstall(const SoftData& data);

private:
	class PrivateFree
	{
	public:
		PrivateFree(){}
		~PrivateFree()
		{
			if(	CCategoryFactory::m_Instance )
			{
				delete CCategoryFactory::m_Instance;
				CCategoryFactory::m_Instance = nullptr;
			}
		}
	};
	static PrivateFree pFree;
	CCategoryFactory(QObject *parent = 0);

private:
	DATA				m_pLocalData;		//本地数据
	DATASOFT			m_pUpgradeData;		//保存当前需要更新的数据
	QMutex				m_pXmlFileMutex;	//xml文件锁
};

#define CategoryFactory CCategoryFactory::Instance()

#endif // CCATEGORYFACTORY_H
