#pragma once
#include <QThread>
typedef struct _LOGINFO
{
	_LOGINFO()
	{
		strDataTime = "";
		strLogInfo  = "";
	}
	QString strDataTime;
	QString strLogInfo;
}LOGINFO,*PLOGINFO;

class CLogManager : public QThread
{
public:
	CLogManager(QObject *parent = 0);
	virtual ~CLogManager(void);

	//启动日志系统
	bool Start(const QString& strPath);

	//添加日志
	void AddLog(const QString& strLog);

	//释放资源
	void Clear();

	//检查日志列表
	bool CheckLogInfoList();

	//写日志
	void LoggerWrite();

	QList<LOGINFO> m_TempLogInfoList;       //待写入日志列表
	QMutex         m_TempLogInfoListMutex;  //待写入日志列表操作锁
	bool           m_bIsStart;              //是否已经启动
	QString        m_strLogInfo;            //当前需要输出的日志
	QString        m_strPath;               //日志路径
	QString        m_strLastLog;            //最后一条被记录的日志信息
	bool           m_bThreadExit;           //线程退出标志

protected:
	virtual void run();
};

