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

	//�������ݿ��ļ�
	//bSaveImmediately - true Save�������洢 - false Save����ͨ������ģʽ�������Ҫ����Commit����
	//bCreateIfInExist - true ������ݿⲻ���ڻ��Զ����� - false ���ݿⲻ���ڣ��ᴴ��ʧ�ܣ�����false
	bool LoadDatabase(const QString& strFilePath, bool bCreateIfInExist = true);

	/*---------------------------------User---------------------------------*/

	//����û����������Ƿ����
	bool IsUserExist(const QString& strUserName, const QString& strPassword);

	//����û�
	bool AddUser(const QString& strUserName, const QString& strPassword);

	//�޸�����
	bool ModifyPassword(const QString& strUserName, const QString& strPassword);

	/*---------------------------------Client---------------------------------*/

	//ͨ���ŵ����жϿͻ����Ƿ����
	bool IsClientExist(const QString& clientId);

	//��ӿͻ�����Ϣ
	bool AddClientInfo(const ClientInfo& clientInfo);

	//���¿ͻ���Ϣ
	bool UpdateClientInfo(const ClientInfo& clientInfo);

	/*---------------------------------Software---------------------------------*/

	//�ж�����Ƿ����
	bool IsSoftwareExist(const QString& strSoftwatename);

	//�ж�����Ͱ汾�Ƿ�һ��
	bool IsSoftwareExist(const QString& strSoftwareName, const QString& strVersion);

	//��ȡ���������Ϣ
	bool ReadAllSoftwareInfo(SoftwareinfoVector& softwareInfoVector);

	//���������Ϣ(���->����) -�����Ƽ�ʹ�ã����ڴֱ�
	bool SaveSoftwareInfos(const SoftwareinfoVector& softwareInfoVector);

	//�޸������Ϣ����������Ϊ��� ͨ��packType���������� 0 - insert 1 - updatewithoutnum 2 - updateall 3 - remove
	bool UpdateSoftwareInfos(const SoftwareinfoVector& softwareInfoVector);

	//�ж������װ֪ͨ��Ϣ�Ƿ����
	bool IsSoftInstallInfoExist(const QString& clientId, const QString& name, const QString& version);

	//��������Ϣ
	bool AddSoftwareInfo(const SoftwareInfo& softwareInfo);

	//���������װ��
	bool AddSoftwareInstallNum(const QString& name, int nAddNums = 1);

	//�������δ��װ��
	bool AddSoftwareUninstalledNum(const QString& name, int nAddNums = 1);

	//���������Ϣ
	bool UpdateSoftwareInfo(const SoftwareInfo& softwareInfo);

	//���������Ϣ
	bool UpdateSoftwareInfoWithOuInstallNum(const SoftwareInfo& softwareInfo);

	//��ȡָ�����������Ϣ
	bool ReadSoftwareInfo(const QString& name, SoftwareInfo& softwareInfo);

	//ɾ�����
	bool RemoveSoftware(const QString& name);

	/*---------------------------------Help---------------------------------*/

	//�жϰ�����Ϣ�Ƿ����
	bool IsHelpInfoExist(const QString& title);

	//��ȡ������Ϣ
	bool ReadHelpInfos(HelpInfoMap& helpInfoMap);

	//���������Ϣ
	bool SaveHelpInfos(const HelpInfoMap& helpInfoMap);

	/*---------------------------------StatisticsInfo---------------------------------*/

	//��ȡ�ŵ������װ���
	bool GetCrosslinkRunInstalledResult(const QString& strShopId, QString& strResult);

	//��ȡ�ŵ�����
	bool ReadShopInfoHash(QHash<int, QStringList>& data);
	
	/*---------------------------------Orther---------------------------------*/

	//д������
	bool WriteDataMotionList(const DataMotionVector& dataMotionVector);

	//�ر����ݿ�
	void CloseDatabase();

	//��ȡ��ǰʹ�õ����ݿ�·��
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
	QSqlDatabase*           m_pSqlDatabase;           //���ݿ����
	QString                 m_strDatabasePath;        //���ݿ�·�� 
	int                     m_bFull;                  //���ݿ���
	QString                 m_strSqlDirPath;          //���ݿ�Ŀ¼    
	SoftwareInstallInfoMap  m_SoftwareInstallInfoMap; //�����װ��Ŀ
};

#endif // DATABASEMANAGER_H
