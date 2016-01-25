#ifndef ACCESSHELPER_H
#define ACCESSHELPER_H

#include <QObject>
#include <QMutex>

//Access操作类
class AccessHelper : public QObject
{
	Q_OBJECT

public:
	AccessHelper(QObject *parent = 0);
	~AccessHelper();

	//获取门店信息
	bool GetShopInfo(const QString& strDatabasePath, const QString& strPassword, QString& strShopId, QString& strUnitNum, QString& strUnitName);

private:

	//注册系统ADO
	bool RegSysMSADO();

	//注册本地ADO
	bool RegLocalMSADO();
	
	QMutex m_ShopDateLock;
};

#endif // ACCESSHELPER_H
