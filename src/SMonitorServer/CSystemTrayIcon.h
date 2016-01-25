/**************************************************************************** 
** CopyRight(c) 2014, 
** All rights reserved 
** 
** 文件名称:  CSystemTrayIcon.h 
** 摘要:  系统托盘类，需要根据服务器状态切换图标
** 
** 当前版本: 1.0.0.0 
** 作者:  宋斌斌 
** 
** 完成日期： 2014-11-23
** 
** 历史修改记录:　 
** 作者         描述                                 修改时间                      
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
	//系统托盘状态转换更新通知
	void trayIconStateChanged(int state);

	//退出当前应用
	void quitApplication();

	//关于应用
	void aboutApplication();

	//显示/隐藏应用主界面
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
