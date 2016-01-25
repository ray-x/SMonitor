#ifndef CHELPINFOWIDGET_H
#define CHELPINFOWIDGET_H

#include <QWidget>
#include "ui_CHelpInfoWidget.h"
#include "CMessageEventMediator.h"

class CHelpInfoWidget : public QWidget, public Ui::CHelpInfoWidget
{
	Q_OBJECT

public:
	CHelpInfoWidget(QWidget *parent = 0);
	~CHelpInfoWidget();

public Q_SLOTS:
	void updateHelpInfo();

private Q_SLOTS:
	void modifyId();
	void SendResult(int nMsgHead, bool bResult, const QString& strInfo);
	void serverReqError(int nMsgHead, const QString& strError);
	void updateInfomation(const StringMap& data);
	void modifySuccess();

private:
	QString getId();
	void GetHelpInfo();
	void StorageHelpInfo(const StringMap& data);
	void LoadHelpInfo();

	QString GetDecryptHelpInfo();
	void SetEncryHelpInfo(const QString& data);
	void resetInformation(const StringMap& data);
};

#endif // CHELPINFOWIDGET_H
