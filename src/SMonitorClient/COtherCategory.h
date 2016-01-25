#ifndef COTHERCATEGORY_H
#define COTHERCATEGORY_H

#include "CERPCategory.h"

class COtherCategory : public CERPCategory
{
	Q_OBJECT

public:
	COtherCategory(QObject *parent = 0);
	~COtherCategory();

	virtual QString Category()const;

private:
	
};

#endif // COTHERCATEGORY_H
