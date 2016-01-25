#ifndef CITEMDELEGATE_H
#define CITEMDELEGATE_H

#include <QStyledItemDelegate>

class QSignalMapper;

class CItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	CItemDelegate(QObject *parent);
	~CItemDelegate();

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index)const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index)const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index)const;

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

public Q_SLOTS:
	void addBrand(const QString&);
	void resetBrand();

private:
	QStringList m_brandList;
};

#endif // CSOFTPACKEDTYPE_H
