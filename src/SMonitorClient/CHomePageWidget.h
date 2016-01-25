#ifndef CHOMEPAGEWIDGET_H
#define CHOMEPAGEWIDGET_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QStyledItemDelegate>
#include "ui_CHomePageWidget.h"
#include "common.h"
#include "CListWidget.h"

class CFlowLayout;
class CSoftItemWidget;
class CStorageMsgThread;
class CMarkup;
class CHomePageWidget : public QWidget, public Ui::CHomePageWidget
{
	Q_OBJECT

public:
	CHomePageWidget(QWidget *parent = 0);
	~CHomePageWidget();

	void init();
	bool getInstallSoftXml(CMarkup *xml, bool& bLocalCrossInstall); 
	bool isItemEmpty(); 
	bool isLocalCrossLinkRunInstalled();

protected:
	virtual void timerEvent(QTimerEvent *);

public Q_SLOTS:
	void updateSoftWidget();
	void showIndependentItem(QString strKey, QString version, bool bShow);

private Q_SLOTS:
	void showTotalMessage(bool checked);
	void setSoftwareListItem(const QMap<QString, QMap<QString, SoftData> >& );
	void appendMessage(const QDateTime& time, const QString& content);
	void installFinished(const QString& key);
	void installErrorOccur(const QString& key, int state);
	void upgradeAllItems();
	void recvMessage(const QDateTime& time, const QString& content);
	void recvDistributeMsg(const QString& msg, const QString& time);
	void getDistributeMsg(const QString& msg, const QString& time);
	void InstallSoftware(const QString& key);
	void downloadQuit(const QString& key);

Q_SIGNALS:
	void downloadNotify(const QString& key, QString& srcPath, const QString& desPath);
	void NotifyOperation(SMonitor::Type type, const QString& category, const QString& key);
	void upgradeNotify(SMonitor::Type type = SMonitor::Upgrade); 

private:
	void  serverStatus();
	void  removeMessage(const QString& time);
private:
	CFlowLayout*				m_scrollLayout;
	bool						m_bMsgScrolled;
	CListWidget*				m_msgListWidget;
	QMap<QString, CSoftItemWidget*>	m_subWidgetHash;

	CStorageMsgThread*			m_StorageThread;
	QMap<QDateTime, QString>	m_MsgMap;
	QMutex                      m_WidgetHashMutex;
	QMutex						m_qMutexForMessage;
};


class CStorageMsgThread : public QThread
{
public:
	CStorageMsgThread(QObject* parent = 0);
	virtual ~CStorageMsgThread();

	void Start();

	void AddMessage(const QDateTime&, const QString&);

protected:
	virtual void run();

private:
	void StorgeMessage();
	bool CheckMsgList();

private:
	bool m_bThreadExit;
	QString m_strMsgPath;
	QMutex m_qMutex;
	

	QMap<QDateTime, QString> m_MsgMap;
	QList<QPair<QString, QString> > m_MsgList;
};

#endif // CHOMEPAGEWIDGET_H
