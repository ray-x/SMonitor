#pragma once
#include <QObject>

class CNetClt;
class CMessageEventMediator;
class CHomePageWidget;
class CNetClass : public QObject
{
	Q_OBJECT
public:
	CNetClass(QObject *parent = 0);
	virtual ~CNetClass(void);

	static CNetClass* Instance();

	CNetClt                *m_pNetClt;
	CMessageEventMediator  *m_pMessageEventMediator;
	CHomePageWidget        *m_pHomePageWidget;

private:
	static CNetClass* m_Instance;
};

#define NetClass CNetClass::Instance()