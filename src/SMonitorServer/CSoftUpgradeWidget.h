#ifndef CSOFTUPGRADEWIDGET_H
#define CSOFTUPGRADEWIDGET_H

#include <QWidget>
#include "ui_CSoftUpgredeWidget.h"

#include "Define.h"

class CItemDelegate;


class CSoftUpgradeWidget : public QWidget, public Ui::CSoftUpgredeWidget
{
	Q_OBJECT

public:
	CSoftUpgradeWidget(QWidget *parent = 0);
	~CSoftUpgradeWidget();

	SoftwareinfoMap GetSoftwareInfoMap(bool bRemoveNetPage);

Q_SIGNALS:
	void dataChanged(const QString&);
	void itemRemoved(const QString&, int nRow);
	void itemAdded(const QString&);
	void brandAdded(const QString&);
	void brandChanged();
	void datasaved();

protected:
	virtual void keyPressEvent(QKeyEvent *);

private Q_SLOTS:
	//添加顶级节点
	void addItem();
	void removeItem();
	void itemChanged(QTableWidgetItem* item);
	void SaveContentToFile();
	void SaveContentToDatabase(int row = -1);
	void addSubBrand();
	void currentBrandChanged(const QString&);
	void initWidget();
	void PasswdConfig();
private:
	void getSoftInfo(int row,SoftwareInfo& info);

	void prepareItemChanged();
	void endItemChanged();

	void insertItem(const SoftwareInfo& info);

	void updateItem(int row, const SoftwareInfo& info);

	//获取下一个建议的节点名称
	QString getNextDistinctChildName(const QString& suggestedName, int column, int exceptRow) const;

	bool hasInstall(const SoftwareInfo& info);
	bool findItemText(const QTableWidgetItem* item);

	void ClearBrands();

private:
	QStringList			     m_brandList;
	CItemDelegate*		     m_itemDelegate;
	QVector<int>             m_changedRowVec;
	SoftwareinfoVector       m_deleteSoftwareInfoVec;
	SoftwareinfoVector       m_softwareInfoVector;
	QMutex                   m_softwareInfoMutex;
};

#endif // CSOFTUPGREDEWIDGET_H
