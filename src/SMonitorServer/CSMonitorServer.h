#ifndef CSMONITORSERVER_H
#define CSMONITORSERVER_H

#include <QtWidgets/QWidget>
#include "ui_CSMonitorServer.h"
#include "IOCPNet.h"
#include <QSystemTrayIcon>

class CSystemTrayIcon;
class CTitleWidget;
class QStackedWidget;
class CSideBarWidget;
class CSoftUpgradeWidget;
class CHelpInfoWidget;
class CStatusWidget;
class CStatisticsWidget;

class CSMonitorServer : public QFrame
{
	Q_OBJECT

public:
	CSMonitorServer(QWidget *parent = 0);
	~CSMonitorServer();

public slots:
	void startServer();
	void endServer();
	void setMinimumed();
	void setMaximumed();
	void hideOrShow();
	void about();
	void activedByTrayIcon(QSystemTrayIcon::ActivationReason reason);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

private:
	void setWidgetStyleSheet(const QString& style);
	void initTabWidget();

private:
	//Ui::CSMonitorServerClass ui;
	CIOCPNet*              m_pIOCPnet;              //通信模块
	bool                   m_bServerIsStarted;      //服务器已经启动
	CMessageEventMediator* m_pMessageEventMediator;

	CSystemTrayIcon*	m_trayIcon;
	CTitleWidget*		m_titleWidget;
	QStackedWidget*		m_stackedWidget;
	CSideBarWidget*		m_sideBarWidget;

	CSoftUpgradeWidget*	m_homePageWidget;
	CStatisticsWidget*	m_statisticsWidget;
	CStatusWidget*		m_statusWidget;
	CHelpInfoWidget*	m_helpPageWidget;

	bool				m_bPressed;
	QPoint				m_movePoint;
};

#endif // CSMONITORSERVER_H
