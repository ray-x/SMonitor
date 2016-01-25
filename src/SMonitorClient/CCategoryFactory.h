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
	//��ȡ��������
	DATA LocalData()const;
	//���ر�������
	void LoadLocalData();
	//�����Ѿ���װ�����ص�Ʒ������
	QString GetLocalBrand()const;

Q_SIGNALS:
	//������¹㲥��Ϣ
	void upgradeNotify(SMonitor::Type type = SMonitor::Upgrade); 
	//���ݸ���֪ͨ��Ϣ
	void dataChangedNotify(const DATA&); 
	//��Ϣ�ַ�
	void MessageNotify(const QDateTime& , const QString& );
	//�����װ�ɹ����ύ��װ����ź�
	void SoftwareInstalled(const QString& name);

public Q_SLOTS:
	//�����װ�ɹ�����
	void InstallFinished(const QString& name); 
	//��ȡ���������ݺ�õ��ɸ�������
	void CheckUpgradeData() ;

private:
	static CCategoryFactory* m_Instance;

	//��xml�ļ��ж�ȡ����
	bool LoadData(const QString& file, DATA& data, bool bLocal);

	//�޸ı������������Ϣ
	void SetLocalData(const SoftData& data);

	//��XMLɾ������
	void DelLocalData(const SoftData& data);

	//����ĳ��Ʒ��Ϊ�Ѱ�װ����
	void SetLocalBrand(const QString& brand);
	
	//��ȡĳ�����͵������������
	DATASOFT GetCategoryUpgradeData(const QString& category, const DATASOFT& SvrData) const;

	//��ȡxml�е�����������Ϣ
	void UpdateBrandPasswdBySvrFile();

	//�ж�����Ƿ�װ
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
	DATA				m_pLocalData;		//��������
	DATASOFT			m_pUpgradeData;		//���浱ǰ��Ҫ���µ�����
	QMutex				m_pXmlFileMutex;	//xml�ļ���
};

#define CategoryFactory CCategoryFactory::Instance()

#endif // CCATEGORYFACTORY_H
