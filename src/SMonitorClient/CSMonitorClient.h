#ifndef CSMONITORCLIENT_H
#define CSMONITORCLIENT_H

#include <QtWidgets/QFrame>
#include <QSystemTrayIcon>
#include "common.h"

#include "../SMonitorServer/CNetMessage.h"

//#include "ui_CSMonitorClient.h"

class CSystemTrayIcon;
class CTitleWidget;
class QStackedWidget;
class CSideBarWidget;
class CPushButton;
class CHomePageWidget;
class CSoftTabWidget;
class CSoftUpdateWidget;
class QFileSystemWatcher;
class CNetClt;
class CMessageEventMediator;
class CHelpInfoWidget;
class CSplashScreen;
class CLoginDialog;
//class QFrame;

class CSMonitorClient : public QFrame
{
	Q_OBJECT

public:
	CSMonitorClient(QWidget *parent = 0);
	~CSMonitorClient();

	void init();

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual QSize sizeHint()const;
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void timerEvent(QTimerEvent *);

Q_SIGNALS:
	void sig_showIndependentItem(QString strKey, QString version, bool bShow);

private Q_SLOTS:
	void activedByTrayIcon(QSystemTrayIcon::ActivationReason reason);
	void maximized();
	void setWidgetStyleSheet(const QString& style);
	void hideOrShow();
	//void about();
	void setMinimumed();
	void updateArrived();
	void setServerXmlUrl(const QString& strUpdateXmlPath, const QString& strHelpInfoXmlPath, const QString& strVersionCheckFile);
	void downloadUpdateXMLErrorOccur(const QString& errMsg);
	void downloadHelpInfoXMLErrorOccur(const QString& errMsg);
	void downloadCheckFileErrorOccur(const QString& errMsg);
	void downloadStarted(SMonitor::Type type, const QString& category, const QString& name);
	void slot_checkFile();


private:
	void initTabWidget();
	void downloadUpdateXmlFromServer();
	void downloadHelpInfoXmlFromServer();
	void checkUpdate();
	QString getId();
	void setSoftInfoCanUpload();

private:
	CSystemTrayIcon*	m_trayIcon;
	CTitleWidget*		m_titleWidget;
	QStackedWidget*		m_stackedWidget;
	CSideBarWidget*		m_sideBarWidget;

	CHomePageWidget*	m_homePageWidget;
	CSoftTabWidget*		m_softListWidget;
	CSoftUpdateWidget*	m_updatePageWidget;
	CHelpInfoWidget*	m_helpPageWidget;

	bool				m_bPressed;
	QPoint				m_movePoint;

	QFileSystemWatcher* m_watcher;
	CNetClt*            m_pNetClient;
	CMessageEventMediator* m_pMessageEventMediator;

	int					m_regTimerId;//注册客户端ID
	int					m_reqXMLTimerId;//获取服务器xmlID
};

#endif // CSMONITORCLIENT_H
