#ifndef CDATAMANAGERTHREAD_H
#define CDATAMANAGERTHREAD_H

#include <QThread>
#include <QVector>
#include "Define.h"

class CDataManagerThread : public QThread
{
	Q_OBJECT

public:
	CDataManagerThread(QObject *parent = 0);
	~CDataManagerThread();

	static CDataManagerThread* Instance();

	bool Start();
	void Stop();

	//添加数据行为，此数据为异步插入数据库
	void AddDataMotion(DataMotion& dataMotion);

	//加载数据库文件
	//bSaveImmediately - true Save会立即存储 - false Save操作通过事务模式，最后需要调用Commit保存
	//bCreateIfInExist - true 如果数据库不存在会自动创建 - false 数据库不存在，会创建失败，返回false
	bool LoadDatabase(const QString& strFilePath, bool bCreateIfInExist = true);

	//通过门店编号判断客户端是否存在
	bool IsClientExist(const QString& clientId);

	//判断软件是否存在
	bool IsSoftwareExist(const QString& strSoftwatename);

	//判断软件安装通知信息是否存在
	bool IsSoftInstallInfoExist(const QString& clientId, const QString& name, const QString& version);

	//判断正在插入的软件信息是否存在
	bool IsSoftInfoExist(const QString& clientId);

protected:
	virtual void run();

	//检查是否有数据需要写入数据库
	bool CheckDataList();

	//将数据写入数据库
	void WriteToDatabase();

	//判断DataMotionVector里是否存在Client
	bool isClientExistInDataVector(const QString& clientId);

	//判断ToBeExcuteDataVector里是否存在Client
	bool isClientExistInToExcuteDataVector(const QString& clientId);

	//判断DataMotionVector里是否存在SoftInstallInfo
	bool isSoftInstallInfoExistInDataVector(const QString& clientId, const QString& name, const QString& version);

	//判断ToBeExcuteDataVector里是否存在SoftInstallInfo
	bool isSoftInstallInfoExistInToExcuteDataVector(const QString& clientId, const QString& name, const QString& version);

	//写入门店信息到EXCEL
	void WriteShopInfoToExcel();

private: 
	bool m_bThreadExit;
	static CDataManagerThread * m_Instance;
	QMutex                    m_DataLock;
	QMutex                    m_ToBeExcuteDataLock;
	DataMotionVector          m_DataMotionVector;
	DataMotionVector          m_ToBeExcuteDataMotionVector;
};

#define DataManagerThread CDataManagerThread::Instance()

#endif // CDATAMANAGERTHREAD_H
