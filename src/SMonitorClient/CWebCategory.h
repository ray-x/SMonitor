#ifndef CWEBCATEGORY_H
#define CWEBCATEGORY_H

#include "CCategoryBase.h"

class CWebCategory : public CCategoryBase
{
	Q_OBJECT

public:
	CWebCategory(QObject *parent = 0);
	~CWebCategory();

	virtual QString Category() const;

	virtual bool IsAppInstall(const SoftData& data);
	virtual const QMap<QString, SoftData> UpgradeSoftwareData();

private:
	
};

#endif // CWEBCATEGORY_H
