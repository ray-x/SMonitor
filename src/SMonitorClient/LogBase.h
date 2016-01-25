#pragma once
#include <QObject>
class LogBase : public QObject
{
	Q_OBJECT

public:
	LogBase(QObject *parent = 0);
	virtual ~LogBase(void);

	//启动日志系统
	bool Start(const QString& strLogPath);

	//添加日志
	void AddLog(const QString& strLog);

	//获取最后一条日志
	QString GetLastLog();

private:
	class CLogManager* m_pLogManager;
};

