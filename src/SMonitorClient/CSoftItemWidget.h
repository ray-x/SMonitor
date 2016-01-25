#ifndef CSOFTITEMWIDGET_H
#define CSOFTITEMWIDGET_H

#include <QFrame>
#include "common.h"

class QToolButton;

class CSoftItemWidget : public QFrame
{
	Q_OBJECT

public:
	CSoftItemWidget(const SoftData& data, QWidget *parent);
	~CSoftItemWidget();

	const SoftData& getData() ;

Q_SIGNALS:
	void NotifyOperation(SMonitor::Type type, const QString&category, const QString& name);
	void downloadNotify(const QString& key, QString& srcPath, const QString& desPath);

public Q_SLOTS:
	void updateState(const SoftData&);
	void setOperation();
	void installFinished();
	void installErrorOccur();
	void downloadQuit();

private:
	bool confirmOpertion();
	bool IsBrandNeedPasswd(const QString& brand);

	QString gernateToolTips(const SoftData&);

	void OpenExternalProgram();

private:
	QToolButton*		m_tbtnAPP;
	QToolButton*		m_tbtnOpr;

	SoftData			m_curData;

	QMutex				m_dataMutex;
};

#endif // CSOFTITEMWIDGET_H
