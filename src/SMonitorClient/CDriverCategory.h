#ifndef CDRIVERCATEGORY_H
#define CDRIVERCATEGORY_H

#include "CCategoryBase.h"

class CDriverCategory : public CCategoryBase
{
	Q_OBJECT

public:
	CDriverCategory(QObject *parent = 0);
	~CDriverCategory();

	virtual QString Category()const;
	virtual bool IsAppInstall(const SoftData& data);
	virtual const QMap<QString, SoftData> UpgradeSoftwareData();

private:
	
};

#endif // CDRIVERCATEGORY_H
