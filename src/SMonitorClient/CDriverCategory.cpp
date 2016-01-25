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
	return QString::fromLocal8Bit("����");
}

//�жϵ�ǰ�����Ƿ��Ѿ���װ������
bool CDriverCategory::IsAppInstall(const SoftData& data)
{
// 	QString strDrive = "";
// 	if(data.brand == QString::fromLocal8Bit("������"))
// 	{
// 		strDrive = "EPSON";
// 	}
// 	else
// 	{
// 		ClientLogger->AddLog(QString::fromLocal8Bit("����δ֪������Ʒ��[%1]").arg(data.brand));
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
		//1���жϵ�ǰ�����Ƿ���ݵ�ǰϵͳ
		if(CMisc::IsOSSuit(data.name))
		{
			if(IsAppInstall(data))
			{//�����Ѱ�װ������ʾ����
				data.state = SMonitor::Normal; //״̬Ϊ�Ѱ�װ
				upgradeMap.insert(strKey, data);
			}
			else
			{
				data.state = SMonitor::UnInstall; //״̬δ��װ
				emit MessageNotify(QDateTime::currentDateTime(), 
					QString::fromLocal8Bit("�������� [%1] �ɰ�װ���������뼰ʱ��װ��")
					.arg(strKey).arg(data.version));
				ClientLogger->AddLog(QString::fromLocal8Bit("�������� [%1] �ɰ�װ���������뼰ʱ��װ��")
					.arg(data.name).arg(data.version));
				upgradeMap.insert(strKey, data);
			}
		}
		else
		{//ϵͳ�����ݻ��߼��ݵ��Ѱ�װ������������
			//todo nothing
			ClientLogger->AddLog(QString::fromLocal8Bit("�������򲻼��ݵ�ǰϵͳ������ϵ����Ա"));
		}
	}
	return upgradeMap;
}