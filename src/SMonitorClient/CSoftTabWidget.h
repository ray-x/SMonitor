#ifndef CSOFTWARELISTWIDGET_H
#define CSOFTWARELISTWIDGET_H

#include <QWidget>
#include "common.h"

class QTabWidget;
class CTableWidget;

class CSoftTabWidget : public QWidget
{
	Q_OBJECT

public:
	CSoftTabWidget(QWidget *parent = 0);
	~CSoftTabWidget();

	void setTab(const QString& label);

public Q_SLOTS:
	void setWidgetData(const QMap<QString, QMap<QString, SoftData> >& );

private:
	QTabWidget*	 m_pTabWidget;
	QMap<QString, CTableWidget*> m_pTabMap;
	QMap<QString, QString> m_valueMap;
	QStringList	m_pHeaderLabels;
};

#endif // CSOFTWARELISTWIDGET_H
