#ifndef CTABLEWIDGET_H
#define CTABLEWIDGET_H

#include <QTableWidget>
#include "common.h"

class QTableWidgetItem;
class QToolButton;
class QProgressBar;
class QSignalMapper;

class CTableWidget : public QTableWidget
{
	Q_OBJECT

public:
	CTableWidget(const QMap<QString, SoftData>& data, QWidget *parent);
	~CTableWidget();

	void updateTable(const QMap<QString, SoftData>& data);
	void removeItem(const QString& key);
	void setHeaderLabels(const QStringList& headlabels);
	void showIndependentItem(QString strKey, QString version, bool bShow);
	int getIndepentDataCount();
	void clearUninstallData();

protected:
	virtual void timerEvent(QTimerEvent *event);
	
private Q_SLOTS:
	void downloadSoftware();
	void redownloadCountdown(int type, const QString& key, int duration);
	void InstallFinish(const QString& key);
	void decompressError(const QString& key);
	void downloadErrorOccur(const QString& key, const QString&);
	void downloadFinish(const QString& key);
	void downloadPercent(const QString& key, long ldownBytes, int tottalBytes);
	void redownloadFailed(const QString& key, int state);
	void InstallCancled(const QString&key, int state);
	void Cancled(const QString& key);


private:

	int FindItemRow(const QString& key);
	void setCellData(const SoftData& sData, int row);
	void _clearUninstallData();
	void ProgressWidget(int row, const QString& key);

	QProgressBar* FindProgressBar(const QWidget* parent);
	QToolButton* FindToolButton(const QWidget* parent);

private:
	QMap<QString, SoftData>			m_pTableData;
	//QMap<int, QString>				m_pRowKey;//行号
	QStringList						m_pHeaderLabels;

	QMap<int, QPair<int, QString> >	m_pTimerMap;
	int								m_curTimerType;//当前的倒计时类型
	QMap<QString, SoftData>         m_IndependentDataMap;
	QMutex                          m_TableMutex;

	QSignalMapper*					m_signalMapper;
};

#endif // CTABLEWIDGET_H
