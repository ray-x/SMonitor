#ifndef CSOFTINSTALLRESULTSENDTH_H
#define CSOFTINSTALLRESULTSENDTH_H

#include <QThread>
#include "common.h"

class CSoftInstallResultSendThread : public QThread
{
	Q_OBJECT

public:
	CSoftInstallResultSendThread(QObject *parent = 0);
	~CSoftInstallResultSendThread();

	static CSoftInstallResultSendThread* Instance();

	//启动
	bool Start(const QString& strPath);

	//添加
	void AddSoftData(const SoftData& softData);

public Q_SLOTS:
	void serverHasGetSoftInstall(const QString& name, const QString& version);

protected:

	//释放资源
	void Clear();

	//检查列表
	bool CheckSoftDataList();

	//上传
	void Update();

	//从文件中还原上传队列到list
	bool ReloadFromFile();

	//记录到文件
	bool WriteToFile(const SoftData& softData);

	//改变信息
	bool ChangeInfoInFile(const SoftData& softData);

	//删除一条信息
	bool DeleteInfoInFile(const SoftData& softData);

	virtual void run();

private:

	static CSoftInstallResultSendThread* m_Instance;
	QList<SoftData>    m_TempSoftDataList;      //待上传列表
	QMutex             m_TempSoftDataListMutex; //待上传列表操作锁
	bool               m_bIsStart;              //是否已经启动
	QString            m_strLogInfo;            //当前需要输出的日志
	QString            m_strPath;               //日志路径
	bool               m_bThreadExit;           //线程退出标志
	int                m_nHasWaitSendTime;      //已经等待的下次发送时间 
	int                m_nSendWaitTime;         //发送超时
	SoftData           m_nCurSendSoftData;      //正在发送的软件信息
	bool               m_bIsTreatSendReceive;   //是否正在处理发送接受
};

#define SoftInstallResultSendManager CSoftInstallResultSendThread::Instance()

#endif // CSOFTINSTALLRESULTSENDTH_H
