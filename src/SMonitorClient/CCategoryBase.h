#ifndef CCATEGORYBASE_H
#define CCATEGORYBASE_H

#include <QObject>

#include "common.h"

//软件类型基类
class CCategoryBase : public QObject
{
	Q_OBJECT

public:
	CCategoryBase(QObject *parent);
	~CCategoryBase();

	//当前类型
	virtual QString Category()const = 0;

	//获取类型中包含的可升级的软件信息列表
	virtual const QMap<QString, SoftData> UpgradeSoftwareData() = 0;

	//判断给类软件是否安装在本地目录中
	virtual bool IsAppInstall(const SoftData& data) = 0;

	//设置该类型的软件的数据
	void SetData(const QMap<QString, SoftData>& data);

	//获取当前类型软件数据(数据可在子对象中被修改)
	QMap<QString, SoftData> GetData();

	//返回已经安装至本地的品牌类型
	QString GetLocalBrand()const;

	//设置某个品牌为已安装类型
	void SetLocalBrand(const QString& brand);

	//判断本地文件中是否包含合法的软件路劲，
	//若合法，则会自动修改软件结构中的路径
	virtual bool LocalInstall(SoftData& data);

Q_SIGNALS:
	//需要客户端消息列表显示的消息
	void MessageNotify(const QDateTime& time, const QString& message);

private:
	QMap<QString, SoftData> m_pCategoryMap;//当前类型的数据
};

#endif // CCATEGORYBASE_H
