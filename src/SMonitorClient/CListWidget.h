#ifndef CLISTWIDGET_H
#define CLISTWIDGET_H

#include <QListWidget>
#include <QDateTime>
#include <QLabel>

class QListWidgetItem;
class QMenu;

class CListWidget : public QListWidget
{
	Q_OBJECT

public:
	CListWidget(QWidget *parent);
	~CListWidget();

protected:
	virtual void wheelEvent(QWheelEvent * e);
};

class CLabel : public QLabel
{
	Q_OBJECT
public:
	CLabel(QWidget *parent) ;
	~CLabel();

private Q_SLOTS:
	void copy();
	void selectAll();

protected:
	virtual void contextMenuEvent(QContextMenuEvent *ev);
};

#endif // CLISTWIDGET_H
