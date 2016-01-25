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
	//ClientLogger->AddLog(QString::fromLocal8Bit("�����ļ����·��Ϊ��%1").arg(data.location));
	QString location;//·��������ʱ���ṩһ��Ĭ��·������Ȼ��ѹ��Ȼ��ʧ��
	QString strLocalDes = data.location;
	strLocalDes = strLocalDes.replace("\\", "/"); //�滻ԭʼ·���е�"\"Ϊ"/"��QString��ת���ַ�ת��
	//ClientLogger->AddLog(QString::fromLocal8Bit("ɾ��ת���'\', ���ļ�·���滻Ϊ��%1").arg(strLocalDes));
	QStringList driveList = CMisc::GetLogicalDriveStringList();

	if(strLocalDes.at(1) == ":")
	{//���ԭʼ·���а����̷�
		location = strLocalDes;
		QDir dir(location);
		if(dir.exists())
		{//�����ļ��д���
			location =  dir.path();
			data.location = location;
			if(IsAppInstall(data)){
				data.state = SMonitor::Normal;
				ClientLogger->AddLog(QString::fromLocal8Bit("���[%1] �������Ŀ¼[%2] ��������[%3]���ڣ�������Ϊ�Ѱ�װ").arg(data.name).arg(data.location).arg(data.mainExe));
				if(data.category != QString::fromLocal8Bit("����"))
					SetLocalBrand(data.brand);
				return true;
			}
			else
			{
				ClientLogger->AddLog(QString::fromLocal8Bit("���[%1] �����ļ��д���[%2]����������[%3]�����ڣ�������Ϊδ��װ").arg(data.name).arg(data.location).arg(data.mainExe));
				return false;
			}
		}
		else
		{
			//�˴�����·�������ڣ���Ҫɨ�豾���̷��ǲ��Ǵ��ڸ��ļ���
			for each (auto driver in driveList)
			{
				QString nLocal = location.replace(0, 3, driver);
				//ClientLogger->AddLog(QString::fromLocal8Bit("���[%1]����·��Ϊ[%2]�����ز�������ָ�̷����滻Ϊ[%3]").arg(data.name).arg(location).arg(nLocal));
				location = nLocal;
				if(CMisc::IsFileExist(nLocal))
				{//���ڸ��ļ��У����ж��Ƿ�ó����Ƿ�Ϸ�
					data.location = location;
					if(IsAppInstall(data))
					{
						data.state = SMonitor::Normal;
						ClientLogger->AddLog(QString::fromLocal8Bit("���[%1] �������Ŀ¼[%2] ��������[%3]���ڣ�������Ϊ�Ѱ�װ").arg(data.name).arg(data.location).arg(data.mainExe));
						if(data.category != QString::fromLocal8Bit("����"))
							SetLocalBrand(data.brand);
						return true;
					}
				}
			}

			//��������̷����������ļ��У���ô��Ҫ�ж�����·���ĺϷ���
			if(driveList.contains(strLocalDes.mid(0, 3)))
			{
				data.location = strLocalDes;//ˢ������ṹ·��ΪĬ�ϺϷ�·��
				ClientLogger->AddLog(QString::fromLocal8Bit("[%1]����δ��װ�����·������Ϊ��[%2]").arg(data.name).arg(strLocalDes));
			}
			else
			{
				location = driveList[0] + strLocalDes.mid(3);
				data.location = location;//ˢ������ṹ·��ΪĬ�ϺϷ�·��
				ClientLogger->AddLog(QString::fromLocal8Bit("[%1]����δ��װ������·���Ƿ������·������Ϊ��[%2]").arg(data.name).arg(location));
			}
			return false;
		}
	}
	else
	{//Դ·����δ�����̷�
		if(strLocalDes.at(0) == ":") //���Դ·����":"��ʼ������Ҫ�޳�":"
			strLocalDes = strLocalDes.mid(1);

		//���Դ·����"/"��ʼ������Ҫ�޳�":" "\"�����ʼ�����滻
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
				ClientLogger->AddLog(QString::fromLocal8Bit("���[%1] �������Ŀ¼[%2] ��������[%3]���ڣ�������Ϊ�Ѱ�װ").arg(data.name).arg(data.location).arg(data.mainExe));
				if(data.category != QString::fromLocal8Bit("����"))
					SetLocalBrand(data.brand);
				return true;
			}
		}
		location = QString("%1%2").arg(driveList[0]).arg(strLocalDes);
		data.location = location;//ˢ������ṹ·��ΪĬ�ϺϷ�·��
		ClientLogger->AddLog(QString::fromLocal8Bit("����δ��װ�����·������Ϊ��%1").arg(location));
	}
	return false;
}

void CCategoryBase::SetLocalBrand(const QString& brand)
{
	//Ŀǰֻ�����õ�һ��ɨ�赽��Ʒ��
	if(GetLocalBrand().isEmpty()){
		ClientLogger->AddLog(QString::fromLocal8Bit("���ñ����Ѱ�װƷ��[%1]").arg(brand));
		QSettings settings(HKCU, QSettings::NativeFormat);
		settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

		//QString brand = settings.value("Local/Brand", QStringList()).toString();
		//brands.removeAll(brand);
		//brands.prepend(brand);
		settings.setValue("Local/Brand", brand);
	}
}

//�����Ѱ�װ�ڱ��ص�����Ʒ�Ƶļ��ϣ����ܻ��ж���Ʒ�Ʊ���װ�ڱ���
//todo ����Ʒ���б�֮����Ҫ����һ���Ĵ�����������ѡȡһ��Ʒ��
//ע�⣺������������LocalInstallɨ��֮��Ż���Ч
QString CCategoryBase::GetLocalBrand()const
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	return settings.value("Local/Brand", QString()).toString();
}

