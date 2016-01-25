#ifndef CERPCATEGORY_H
#define CERPCATEGORY_H

#include "CCategoryBase.h"

class CERPCategory : public CCategoryBase
{
	Q_OBJECT

public:
	CERPCategory(QObject *parent = 0);
	~CERPCategory();

	virtual QString Category()const;
	virtual bool IsAppInstall(const SoftData& data);
	virtual const QMap<QString, SoftData> UpgradeSoftwareData();

private:

};

#endif // CERPCATEGORY_H
