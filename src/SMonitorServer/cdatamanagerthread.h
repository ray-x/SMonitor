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

	//���������Ϊ��������Ϊ�첽�������ݿ�
	void AddDataMotion(DataMotion& dataMotion);

	//�������ݿ��ļ�
	//bSaveImmediately - true Save�������洢 - false Save����ͨ������ģʽ�������Ҫ����Commit����
	//bCreateIfInExist - true ������ݿⲻ���ڻ��Զ����� - false ���ݿⲻ���ڣ��ᴴ��ʧ�ܣ�����false
	bool LoadDatabase(const QString& strFilePath, bool bCreateIfInExist = true);

	//ͨ���ŵ����жϿͻ����Ƿ����
	bool IsClientExist(const QString& clientId);

	//�ж�����Ƿ����
	bool IsSoftwareExist(const QString& strSoftwatename);

	//�ж������װ֪ͨ��Ϣ�Ƿ����
	bool IsSoftInstallInfoExist(const QString& clientId, const QString& name, const QString& version);

	//�ж����ڲ���������Ϣ�Ƿ����
	bool IsSoftInfoExist(const QString& clientId);

protected:
	virtual void run();

	//����Ƿ���������Ҫд�����ݿ�
	bool CheckDataList();

	//������д�����ݿ�
	void WriteToDatabase();

	//�ж�DataMotionVector���Ƿ����Client
	bool isClientExistInDataVector(const QString& clientId);

	//�ж�ToBeExcuteDataVector���Ƿ����Client
	bool isClientExistInToExcuteDataVector(const QString& clientId);

	//�ж�DataMotionVector���Ƿ����SoftInstallInfo
	bool isSoftInstallInfoExistInDataVector(const QString& clientId, const QString& name, const QString& version);

	//�ж�ToBeExcuteDataVector���Ƿ����SoftInstallInfo
	bool isSoftInstallInfoExistInToExcuteDataVector(const QString& clientId, const QString& name, const QString& version);

	//д���ŵ���Ϣ��EXCEL
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
