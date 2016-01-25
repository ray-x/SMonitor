#pragma once
#include <QObject>
class LogBase : public QObject
{
	Q_OBJECT

public:
	LogBase(QObject *parent = 0);
	virtual ~LogBase(void);

	//������־ϵͳ
	bool Start(const QString& strLogPath);

	//�����־
	void AddLog(const QString& strLog);

	//��ȡ���һ����־
	QString GetLastLog();

private:
	class CLogManager* m_pLogManager;
};

