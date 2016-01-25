#ifndef CCLIENTLOG_H
#define CCLIENTLOG_H

#include "LogBase.h"
#include <QObject>

class CClientLog : public LogBase
{
	Q_OBJECT

public:
	CClientLog(QObject *parent = 0);
	~CClientLog();

	static CClientLog* Instance();

private:
	static CClientLog* m_Instance;
};

#define ClientLogger CClientLog::Instance()

#endif // CCLIENTLOG_H
