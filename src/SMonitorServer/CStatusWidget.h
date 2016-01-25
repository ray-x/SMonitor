#ifndef CSTATUSWIDGET_H
#define CSTATUSWIDGET_H

#include <QWidget>
#include "ui_CStatusWidget.h"

class CStatusWidget : public QWidget, public Ui::CStatusWidget
{
	Q_OBJECT

public:
	CStatusWidget(QWidget *parent = 0);
	~CStatusWidget();

private Q_SLOTS:
	void changePasswd();
	void checkLength();
	void distributeMessage();

private:
	void clearInput();
	void StorageLastMessage(const QDateTime& time, const QString& msg);
	void LoadLastMessage();

private:
	int	m_nMsgMaxLength; //推送消息最大字数

};

#endif // CSTATUSWIDGET_H
