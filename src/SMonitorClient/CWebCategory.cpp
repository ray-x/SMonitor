#include "CWebCategory.h"
#include "CClientLog.h"

CWebCategory::CWebCategory(QObject *parent)
	: CCategoryBase(parent)
{

}

CWebCategory::~CWebCategory()
{

}

QString CWebCategory::Category() const
{
	return QString::fromLocal8Bit("��ҳ");
}

bool CWebCategory::IsAppInstall(const SoftData& data)
{
	return true;
}

const QMap<QString, SoftData> CWebCategory::UpgradeSoftwareData()
{
	QString brand = GetLocalBrand();
	if(brand.isEmpty())
	{//�����ǰ����Ʒ��Ϊ�գ�����Ҫ��ʾȫ����ҳ
		return GetData();
	}

	QMap<QString, SoftData> upgradeData, srcData = GetData();
	QMapIterator<QString, SoftData> iter(srcData);
	while(iter.hasNext())
	{
		iter.next();
		SoftData data = iter.value();
		if(data.brand == brand)
		{
			data.state = SMonitor::Normal;
			upgradeData.insert(data.name, data);
		}
		else if(data.brand == "")
		{
			data.state = SMonitor::Normal;
			upgradeData.insert(data.name, data);
			ClientLogger->AddLog(QString::fromLocal8Bit("��ҳ����Ϊ�գ���ʾ����ҳ"));
		}
	}
	return upgradeData;
}
