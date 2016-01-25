#pragma once
#include "../../include/ServerApi/Log/CommonLog.h"
class CServerLoger : public CCommonLog, public CSingleInstance<CServerLoger>
{
public:
	CServerLoger(void);
	virtual ~CServerLoger(void);
};

#define ServerLogger CServerLoger::GetInstance()
