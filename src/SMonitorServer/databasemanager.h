#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include "Define.h"

class DatabaseManager : public QObject
{
	Q_OBJECT

public:
	DatabaseManager();
	~DatabaseManager();

	static DatabaseManager* Instance();

	//加载数据库文件
	//bSaveImmediately - true Save会立即存储 - false Save操作通过事务模式，最后需要调用Commit保存
	//bCreateIfInExist - true 如果数据库不存在会自动创建 - false 数据库不存在，会创建失败，返回false
	bool LoadDatabase(const QString& strFilePath, bool bCreateIfInExist = true);

	/*---------------------------------User---------------------------------*/

	//检测用户名和密码是否存在
	bool IsUserExist(const QString& strUserName, const QString& strPassword);

	//添加用户
	bool AddUser(const QString& strUserName, const QString& strPassword);

	//修改密码
	bool ModifyPassword(const QString& strUserName, const QString& strPassword);

	/*---------------------------------Client---------------------------------*/

	//通过门店编号判断客户端是否存在
	bool IsClientExist(const QString& clientId);

	//添加客户端信息
	bool AddClientInfo(const ClientInfo& clientInfo);

	//更新客户信息
	bool UpdateClientInfo(const ClientInfo& clientInfo);

	/*---------------------------------Software---------------------------------*/

	//判断软件是否存在
	bool IsSoftwareExist(const QString& strSoftwatename);

	//判断软件和版本是否一致
	bool IsSoftwareExist(const QString& strSoftwareName, const QString& strVersion);

	//读取所有软件信息
	bool ReadAllSoftwareInfo(SoftwareinfoVector& softwareInfoVector);

	//保存软件信息(清空->加入) -》不推荐使用，过于粗暴
	bool SaveSoftwareInfos(const SoftwareinfoVector& softwareInfoVector);

	//修改软件信息，不存在则为添加 通过packType来定义类型 0 - insert 1 - updatewithoutnum 2 - updateall 3 - remove
	bool UpdateSoftwareInfos(const SoftwareinfoVector& softwareInfoVector);

	//判断软件安装通知信息是否存在
	bool IsSoftInstallInfoExist(const QString& clientId, const QString& name, const QString& version);

	//添加软件信息
	bool AddSoftwareInfo(const SoftwareInfo& softwareInfo);

	//增加软件安装数
	bool AddSoftwareInstallNum(const QString& name, int nAddNums = 1);

	//增加软件未安装数
	bool AddSoftwareUninstalledNum(const QString& name, int nAddNums = 1);

	//更新软件信息
	bool UpdateSoftwareInfo(const SoftwareInfo& softwareInfo);

	//更新软件信息
	bool UpdateSoftwareInfoWithOuInstallNum(const SoftwareInfo& softwareInfo);

	//读取指定软件名称信息
	bool ReadSoftwareInfo(const QString& name, SoftwareInfo& softwareInfo);

	//删除软件
	bool RemoveSoftware(const QString& name);

	/*---------------------------------Help---------------------------------*/

	//判断帮助信息是否存在
	bool IsHelpInfoExist(const QString& title);

	//读取帮助信息
	bool ReadHelpInfos(HelpInfoMap& helpInfoMap);

	//保存帮助信息
	bool SaveHelpInfos(const HelpInfoMap& helpInfoMap);

	/*---------------------------------StatisticsInfo---------------------------------*/

	//获取门店软件安装结果
	bool GetCrosslinkRunInstalledResult(const QString& strShopId, QString& strResult);

	//读取门店数据
	bool ReadShopInfoHash(QHash<int, QStringList>& data);
	
	/*---------------------------------Orther---------------------------------*/

	//写入数据
	bool WriteDataMotionList(const DataMotionVector& dataMotionVector);

	//关闭数据库
	void CloseDatabase();

	//获取当前使用的数据库路径
	QString GetDatabasePath();

Q_SIGNALS:
	void sig_updateInstallData(QPair<QString, QString> key, int num);
	void sig_updateUnInstallData(QPair<QString, QString> key, int num);
	void sig_updateAllNumData(const QString& name, const QString& version, int installNum, int unInstallNum);
	void sig_initData();

protected:
	void readSoftwareItem(SoftwareInfo &software, QSqlQuery &query);

	QString getDataMotionSql(const DataMotion& dataMotion);

	bool excuteSql(const QString& strSQL);

private:
	static DatabaseManager* m_Instance;
	QMutex                  m_DateLock;
	QSqlDatabase*           m_pSqlDatabase;           //数据库核心
	QString                 m_strDatabasePath;        //数据库路径 
	int                     m_bFull;                  //数据库满
	QString                 m_strSqlDirPath;          //数据库目录    
	SoftwareInstallInfoMap  m_SoftwareInstallInfoMap; //软件安装数目
};

#endif // DATABASEMANAGER_H
