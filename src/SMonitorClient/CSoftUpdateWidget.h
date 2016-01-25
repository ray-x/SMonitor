#ifndef CSOFTUPDATEWIDGET_H
#define CSOFTUPDATEWIDGET_H

#include <QWidget>
#include "common.h"

class CTableWidget;

class CSoftUpdateWidget : public QWidget
{
	Q_OBJECT

public:
	CSoftUpdateWidget(QWidget *parent = 0);
	~CSoftUpdateWidget();

Q_SIGNALS:
	void upgradeNotify(SMonitor::Type type = SMonitor::Upgrade);
	void RowCountChanged(int rowCount);

public Q_SLOTS:
	void showIndependentItem(QString strKey, QString version, bool bShow);

private Q_SLOTS:
	void setUpgradeData(const QMap<QString, QMap<QString, SoftData> >& );
	void InstallFinished(const QString& key);

protected:

	void changeTrayIconState();

private:
	CTableWidget* m_tableWidget;
};

#endif // CSOFTUPDATEWIDGET_H
