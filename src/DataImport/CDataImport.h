#ifndef CDATAIMPORT_H
#define CDATAIMPORT_H

#include <QtWidgets/QWidget>
#include "ui_CDataImport.h"

class QSqlDatabase;

class CDataImport : public QWidget, Ui::CDataImportClass
{
	Q_OBJECT

public:
	CDataImport(QWidget *parent = 0);
	~CDataImport();

protected:
	virtual void timerEvent(QTimerEvent *event);

private Q_SLOTS:
	void Import();
	void SetDataFile();
	void SetDBFile();

private:
	void Initial();
	bool Sync(const QHash<int, QStringList>& data);
	bool CreateTable(const QString& strFilePath);
private:
	QSqlDatabase* m_pSqlDatabase;
	QMutex        m_DateLock;
};

#endif // CDATAIMPORT_H
