#ifndef CCATEGORYBASE_H
#define CCATEGORYBASE_H

#include <QObject>

#include "common.h"

//������ͻ���
class CCategoryBase : public QObject
{
	Q_OBJECT

public:
	CCategoryBase(QObject *parent);
	~CCategoryBase();

	//��ǰ����
	virtual QString Category()const = 0;

	//��ȡ�����а����Ŀ������������Ϣ�б�
	virtual const QMap<QString, SoftData> UpgradeSoftwareData() = 0;

	//�жϸ�������Ƿ�װ�ڱ���Ŀ¼��
	virtual bool IsAppInstall(const SoftData& data) = 0;

	//���ø����͵����������
	void SetData(const QMap<QString, SoftData>& data);

	//��ȡ��ǰ�����������(���ݿ����Ӷ����б��޸�)
	QMap<QString, SoftData> GetData();

	//�����Ѿ���װ�����ص�Ʒ������
	QString GetLocalBrand()const;

	//����ĳ��Ʒ��Ϊ�Ѱ�װ����
	void SetLocalBrand(const QString& brand);

	//�жϱ����ļ����Ƿ�����Ϸ������·����
	//���Ϸ�������Զ��޸�����ṹ�е�·��
	virtual bool LocalInstall(SoftData& data);

Q_SIGNALS:
	//��Ҫ�ͻ�����Ϣ�б���ʾ����Ϣ
	void MessageNotify(const QDateTime& time, const QString& message);

private:
	QMap<QString, SoftData> m_pCategoryMap;//��ǰ���͵�����
};

#endif // CCATEGORYBASE_H
