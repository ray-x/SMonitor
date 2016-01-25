#ifndef CSOFTWARELISTWIDGET_H
#define CSOFTWARELISTWIDGET_H

#include <QWidget>
#include "CXmlParse.h"

class QTabWidget;
class CTableWidget;

class CSoftwareListWidget : public QWidget
{
	Q_OBJECT

public:
	CSoftwareListWidget(QWidget *parent = 0);
	~CSoftwareListWidget();

private Q_SLOTS:
	void updateUi(const QMap<QString, SoftData>& );

private:
	QTabWidget*	 m_pTabWidget;
	QMap<QString, CTableWidget*> m_pTabMap;
};

#endif // CSOFTWARELISTWIDGET_H
