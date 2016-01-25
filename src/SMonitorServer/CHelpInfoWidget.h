#ifndef CHELPINFOWIDGET_H
#define CHELPINFOWIDGET_H

#include <QWidget>
#include "ui_CHelpInfo.h"
class CHelpInfoWidget : public QWidget, public Ui::CHelpInfo
{
	Q_OBJECT

public:
	CHelpInfoWidget(QWidget *parent = 0);
	~CHelpInfoWidget();


private Q_SLOTS:
	void addContent();
	void removeContent();
	void saveContent();
	void itemChanged(QTableWidgetItem* item);
	void SaveToXml();

private:
	void initWidget();
	void prepareItemChange();
	void endItemChange();
	QString getNextDistinctChildName(const QString& suggestedName) const;
};

#endif // CHELPINFOWIDGET_H
