#ifndef CSIDEBARWIDGET_H
#define CSIDEBARWIDGET_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QButtonGroup;
QT_END_NAMESPACE

class CSideBarWidget : public QFrame
{
	Q_OBJECT

public:
	CSideBarWidget(QWidget *parent);
	~CSideBarWidget();

	void addTab(const QString& text, const QChar& icon);
	void insertTab(int index, QString& text, const QString& icon);
	void setCurrentIndex(int index);

Q_SIGNALS:
	void currentIndexChanged(int);

private:
	QVBoxLayout*		m_mainLayout;
	QButtonGroup*		m_buttonGroup;
};

#endif // CSIDEBARWIDGET_H
