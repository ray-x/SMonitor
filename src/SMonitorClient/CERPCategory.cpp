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


//ERP���ж��Ƿ�װ��Ҫ�жϳ���exe�ļ��Ƿ����
bool CERPCategory::IsAppInstall(const SoftData& data)
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

//�жϸ��������Ƿ������Ҫ����(��װ)�����
const QMap<QString, SoftData> CERPCategory::UpgradeSoftwareData()
{
	QMap<QString, SoftData> srcData = GetData();
	QMap<QString, SoftData> upgradeData;
	//����û���κ�Ʒ�ư�װ
	if(GetLocalBrand().isEmpty())
	{
		//����û��Ʒ�ư�װ����ô����Ҫ����һ��ɨ�裬��ȷ������
		//�Ƿ�����Ѱ�װ�������ɨ������б���ܺ��ж���Ʒ�ƣ�
		//��ôɨ���������Ҫ����һ���Ĵ���
		QMapIterator<QString, SoftData> iter(srcData);
		while(iter.hasNext())
		{
			iter.next();
			SoftData data = iter.value();
			//�˴��ı����ļ���ɨ�裬ֻ��Ϊ������ȷ���Ѱ�װ��Ʒ��
			LocalInstall(data);
			if(data.state == SMonitor::Normal || data.state == SMonitor::Upgrade)
			{
				//���������Ѱ�װ������Ҫ��¼�Ѿ���װ��Ʒ��
				SetLocalBrand(data.brand);
			}
			//����ɨ��󣬻��кϷ�·���ó�����������Ҫ���½��и�ֵ
			srcData.insert(data.name, data);
		}
	}

	QString brand = GetLocalBrand();
	if(brand.isEmpty())
	{
		//�����κ�����İ�װ����ı䱾�ذ�װbrand, ɨ��󱾵�δ�����κ��Ѱ�װ
		//��Ʒ�ƣ���ô���ʾΪȫ�µĻ������������������Ҫȫ����ʾ
		QMapIterator<QString, SoftData> iter(srcData);
		while(iter.hasNext())
		{
			iter.next();
			SoftData data = iter.value();
			//�˴��Ѿ�ȷ�����κ�Ʒ�ư�װ�������������ж��Ƿ�װ������
			//LocalInstall(data);
			emit MessageNotify(QDateTime::currentDateTime(), 
				QString::fromLocal8Bit("�ͻ��˿ɰ�װ��� [%1](�汾[%2]),�뼰ʱ��װ")
				.arg(data.name).arg(data.version));
			ClientLogger->AddLog(QString::fromLocal8Bit("�ͻ��˿ɰ�װ��� [%1](�汾[%2]),�뼰ʱ��װ")
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
			if(data.brand == brand) //ֻ��ʾ�Ѱ�װƷ�Ƶ����
			{
				if(LocalData.contains(iter.key()))//�������ݰ���
				{
					const SoftData& lData = LocalData.value(iter.key());
					if(CMisc::IsNeedUpdate(lData.version, data.version))
					{
						//�����Ҫ��������
						data.state = SMonitor::Upgrade;
						emit MessageNotify(QDateTime::currentDateTime(), 
							QString::fromLocal8Bit("�ͻ������ [%1] �ɸ������汾 [%2]")
							.arg(data.name).arg(data.version));
						ClientLogger->AddLog(QString::fromLocal8Bit("�ͻ������ [%1] �ɸ������汾 [%2]")
							.arg(data.name).arg(data.version));
						upgradeData.insert(data.name, data);
					}
					else
					{//������������Ҫ���һ�³����Ƿ񻹴���
						if(!LocalInstall(data))
						{//�Ѿ���������
							upgradeData.insert(data.name, data);
							emit MessageNotify(QDateTime::currentDateTime(), 
								QString::fromLocal8Bit("�ͻ��˿ɰ�װ��� [%1](�汾[%2]),�뼰ʱ��װ")
								.arg(data.name).arg(data.version));
							ClientLogger->AddLog(QString::fromLocal8Bit("�ͻ��˿ɰ�װ��� [%1](�汾[%2]),�뼰ʱ��װ")
								.arg(data.name).arg(data.version));
						}
						else
						{
							if(data.version.isEmpty())
								continue;

							//�����Ѱ�װ������Ҫ�Ƚϰ汾�ţ��ж��Ƿ���Ҫ��������
							QString version = CMisc::GetVersionFileVersion(data.location);
							if(CMisc::IsNeedUpdate(version, data.version))
							{
								//�����Ҫ��������
								data.state = SMonitor::Upgrade;
								upgradeData.insert(data.name, data);
								emit MessageNotify(QDateTime::currentDateTime(), 
									QString::fromLocal8Bit("�ͻ������ [%1] �ɸ������汾 [%2]")
									.arg(data.name).arg(data.version));
								ClientLogger->AddLog(QString::fromLocal8Bit("�ͻ������ [%1] �ɸ������汾 [%2]")
									.arg(data.name).arg(data.version));
							}
						}
					}
				}
				else
				{
					//�жϱ����Ƿ��Ѿ���װ
					if(LocalInstall(data))
					{
						//�����Ѱ�װ������Ҫ�Ƚϰ汾�ţ��ж��Ƿ���Ҫ��������
						QString version = CMisc::GetVersionFileVersion(data.location);
						if(CMisc::IsNeedUpdate(version, data.version))
						{
							//�����Ҫ��������
							data.state = SMonitor::Upgrade;
							upgradeData.insert(data.name, data);
							emit MessageNotify(QDateTime::currentDateTime(), 
								QString::fromLocal8Bit("�ͻ������ [%1] �ɸ������汾 [%2]")
								.arg(data.name).arg(data.version));
							ClientLogger->AddLog(QString::fromLocal8Bit("�ͻ������ [%1] �ɸ������汾 [%2]")
								.arg(data.name).arg(data.version));
						}
						else
						{//�����Ѱ�װ�����Ǳ��������в���������ôҲ��Ҫ��ʾ
							data.state = SMonitor::Normal;
							upgradeData.insert(data.name, data);
						}
					}
					else
					{
						upgradeData.insert(data.name, data);
						emit MessageNotify(QDateTime::currentDateTime(), 
							QString::fromLocal8Bit("�ͻ��˿ɰ�װ��� [%1](�汾[%2]),�뼰ʱ��װ")
							.arg(data.name).arg(data.version));
						ClientLogger->AddLog(QString::fromLocal8Bit("�ͻ��˿ɰ�װ��� [%1](�汾[%2]),�뼰ʱ��װ")
							.arg(data.name).arg(data.version));
					}
				}
			}
		}
		
	}

	return upgradeData;
}

