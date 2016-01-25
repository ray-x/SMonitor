#ifndef CLOGININDIALOG_H
#define CLOGININDIALOG_H

#include <QDialog>
#include "ui_LoginIn.h"

//class CTitleWidget;
//class CLoginForm;
class CSplashScreen;

class CLoginDialog : public QDialog, public Ui::CLoginDialog
{
	Q_OBJECT
public:
	CLoginDialog(QWidget* parent = 0);
	~CLoginDialog();

protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void paintEvent(QPaintEvent *);
	virtual void timerEvent(QTimerEvent *);

Q_SIGNALS:
	void login(const QString& id);

private Q_SLOTS:
	void clientLogin();
	void msgSendResult(int nMsghead, bool result, const QString& info);
	void msgError(int nMsghead, const QString& error);
	void msgLoginIn();


private:
	void saveSettings();

private:

	bool				m_bPressed;
	QPoint				m_movePoint;
	CSplashScreen*		m_splashScreen;
	int                 m_waitid; 
};


#endif