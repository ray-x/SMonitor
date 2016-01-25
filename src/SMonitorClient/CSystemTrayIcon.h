/**************************************************************************** 
** CopyRight(c) 2014, 
** All rights reserved 
** 
** �ļ�����:  CSystemTrayIcon.h 
** ժҪ:  ϵͳ�����࣬��Ҫ���ݷ�����״̬�л�ͼ��
** 
** ��ǰ�汾: 1.0.0.0 
** ����:  �α�� 
** 
** ������ڣ� 2014-11-23
** 
** ��ʷ�޸ļ�¼:�� 
** ����         ����                                 �޸�ʱ��                      
** 
****************************************************************************/
#ifndef CSYSTEMTRAYICON_H
#define CSYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include "common.h"

class QAction;
class QMenu;

class CSystemTrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	CSystemTrayIcon(QObject *parent);
	~CSystemTrayIcon();

public Q_SLOTS:
	void setState(SMonitor::Type state);
	void setHidden(bool hidden);
	void selfUpgrade();
 
Q_SIGNALS:
	//ϵͳ����״̬ת������֪ͨ
	void trayIconStateChanged(int state);

	//�˳���ǰӦ��
	void quitApplication();

	//����Ӧ��
	//void aboutApplication();

	//��ʾ/����Ӧ��������
	void hideOrShowWidget();

protected:
	virtual void timerEvent(QTimerEvent *);

protected Q_SLOTS:
	void downloadFileErrorOccur(const QString& errMsg);
	void downloadSuccess();

private:
	void setTrayIconContextMenu();
	void closeDownload();

private:
	QMenu*	m_TrayIconMenu;
	QAction* m_hideShowAct;
	//QAction* m_aboutAct;
	QAction* m_upgradeAct;
	QAction* m_exitAct;
	int      m_waitid;
	class CHTTPDownloader* m_pVersionFileDownloader;
	SMonitor::Type m_curIconType;
};

#endif // CSYSTEMTRAYICON_H
