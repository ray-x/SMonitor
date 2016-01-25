#ifndef CLOGINDIALOG_H
#define CLOGINDIALOG_H

#include <QDialog>
#include "ui_CLoginDialog.h"

class CLoginDialog : public QDialog, public Ui::CLoginDialog
{
	Q_OBJECT

public:
	CLoginDialog(QWidget *parent = 0);
	~CLoginDialog();

	void addUser();
	void LoadDataBase();

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mousePressEvent(QMouseEvent *);

private Q_SLOTS:
	void login();

private:
	void readSettings();
	void saveSettings(const QString& userName);

private:
	bool				m_bPressed;
	QPoint				m_movePoint;
};

#endif // CLOGINDIALOG_H
