#include "COtherCategory.h"

COtherCategory::COtherCategory(QObject *parent)
	: CERPCategory(parent)
{

}

COtherCategory::~COtherCategory()
{

}

QString COtherCategory::Category()const
{
	return QString::fromLocal8Bit("ÆäËû");
}
