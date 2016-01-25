#pragma once
#include <QObject>

class CIOCPNet;
class CMessageEventMediator;
class CSoftUpgradeWidget;
class CNetClass : public QObject
{
	Q_OBJECT
public:
	CNetClass(QObject *parent = 0);
	virtual ~CNetClass(void);

	static CNetClass* Instance();

	CIOCPNet               *m_pNetClt;
	CMessageEventMediator  *m_pMessageEventMediator;
	CSoftUpgradeWidget     *m_pSoftUpgradeWidget;

private:
	static CNetClass* m_Instance;
};

#define NetClass CNetClass::Instance()