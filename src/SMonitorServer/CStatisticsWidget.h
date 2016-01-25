#ifndef CSTATISTICSWIDGET_H
#define CSTATISTICSWIDGET_H

#include <QWidget>
#include "ui_CStatisticsWidget.h"

#include "Define.h"

class QTableWidgetItem;

class CStatisticsWidget : public QWidget, public Ui::CStatisticsWidget
{
	Q_OBJECT

public:
	CStatisticsWidget(QWidget *parent = 0);
	~CStatisticsWidget();

public Q_SLOTS:
	void updateData();
	void updateItem(const QString& key);
	void removeItem(const QString& key, int nRow);
	void addItem(const QString& key);

public Q_SLOTS:
	void exportData();
	void updateInstallData(QPair<QString, QString> key, int num);
	void updateUnInstallData(QPair<QString, QString> key, int num);
	void updateAllNumData(const QString& name, const QString& version, int installNum, int unInstallNum);
	void initData();
	void exportAll();

private:
	void initWidget();
	void SetItem(int row, int column, const SoftwareInfo& info);

private:
	QHash<QPushButton*, int> m_exportBtnHash;
	QHash<QString, int> m_dataRowHash;
};

#endif // CSTATISTICSWIDGET_H
