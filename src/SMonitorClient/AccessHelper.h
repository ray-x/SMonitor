#ifndef ACCESSHELPER_H
#define ACCESSHELPER_H

#include <QObject>
#include <QMutex>

//Access������
class AccessHelper : public QObject
{
	Q_OBJECT

public:
	AccessHelper(QObject *parent = 0);
	~AccessHelper();

	//��ȡ�ŵ���Ϣ
	bool GetShopInfo(const QString& strDatabasePath, const QString& strPassword, QString& strShopId, QString& strUnitNum, QString& strUnitName);

private:

	//ע��ϵͳADO
	bool RegSysMSADO();

	//ע�᱾��ADO
	bool RegLocalMSADO();
	
	QMutex m_ShopDateLock;
};

#endif // ACCESSHELPER_H
