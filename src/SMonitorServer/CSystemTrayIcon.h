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
 
Q_SIGNALS:
	//ϵͳ����״̬ת������֪ͨ
	void trayIconStateChanged(int state);

	//�˳���ǰӦ��
	void quitApplication();

	//����Ӧ��
	void aboutApplication();

	//��ʾ/����Ӧ��������
	void hideOrShowWidget();

private:
	void setTrayIconContextMenu();

private:
	QMenu*	m_TrayIconMenu;
	QAction* m_hideShowAct;
	QAction* m_aboutAct;
	QAction* m_exitAct;
};

#endif // CSYSTEMTRAYICON_H
