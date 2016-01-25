#ifndef CNOFOCUSFRMEDELEGATE_H
#define CNOFOCUSFRMEDELEGATE_H

#include <QStyledItemDelegate>

class CNoFocusDelegate : public QStyledItemDelegate
{
public:
	CNoFocusDelegate(QObject *parent);
	~CNoFocusDelegate();
	
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	
};

#endif // CNOFOCUSFRMEDELEGATE_H
