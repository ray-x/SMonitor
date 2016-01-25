#ifndef CBRANDLIST_H
#define CBRANDLIST_H

#include <QDialog>

class CTitleWidget;
class QListWidget;
class QListWidgetItem;

class CBrandList : public QDialog
{
	Q_OBJECT

public:
	CBrandList(QWidget *parent);
	~CBrandList();

	void SetBrands(const QStringList &brands);
	QString GetCurBrand();

protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

private Q_SLOTS:
	void itemClicked(QListWidgetItem* item);

private:
	CTitleWidget*		m_titleWidget;
	QListWidget*		m_listWidget;
	QString				m_pCurSelBrand;

	bool				m_bPressed;
	QPoint				m_movePoint;
};

#endif // CBRANDLIST_H
