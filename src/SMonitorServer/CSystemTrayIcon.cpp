/*
This file is part of SMonitor.
Copyright  2015~2016  by: rayx  email rayx.cn@gmail.com
SMonitor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SMonitor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "CSystemTrayIcon.h"
#include "common.h"

#include <QDebug>
#include <QMenu>
#include <QAction>

CSystemTrayIcon::CSystemTrayIcon(QObject *parent)
	: QSystemTrayIcon(parent)
{
	setToolTip(QString::fromLocal8Bit("SMonitor软件安装/升级管理系统"));
	setState(SMonitor::Normal);

	m_TrayIconMenu = new QMenu;

	m_hideShowAct = new QAction(QIcon(":/Resources/hidden.png"), QString::fromLocal8Bit("隐藏"), m_TrayIconMenu);
	m_aboutAct = new QAction(QIcon(":/Resources/help.png"), QString::fromLocal8Bit("关于"), m_TrayIconMenu);
	m_exitAct = new QAction(QIcon(":/Resources/exit.png"), QString::fromLocal8Bit("退出"), m_TrayIconMenu);

	m_TrayIconMenu->addAction(m_hideShowAct);
	m_TrayIconMenu->addAction(m_aboutAct);
	m_TrayIconMenu->addSeparator();
	m_TrayIconMenu->addAction(m_exitAct);

	this->setContextMenu(m_TrayIconMenu);

	connect(m_hideShowAct, &QAction::triggered, this, &CSystemTrayIcon::hideOrShowWidget);
	connect(m_aboutAct, &QAction::triggered, this, &CSystemTrayIcon::aboutApplication);
	connect(m_exitAct, &QAction::triggered, this, &CSystemTrayIcon::quitApplication);
}

CSystemTrayIcon::~CSystemTrayIcon()
{
	qDebug() << "~CSystemTrayIcon()";
	DELETE_POINTER(m_hideShowAct);
	DELETE_POINTER(m_aboutAct);
	DELETE_POINTER(m_exitAct);
	DELETE_POINTER(m_TrayIconMenu);
}

void CSystemTrayIcon::setState(SMonitor::Type state)
{
	switch(state)
	{
	case SMonitor::Normal:
		setIcon(QIcon(":/Resources/normal.png"));
		break;
	case SMonitor::Upgrade:
		{
			setIcon(QIcon(":/Resources/update.png"));
			showMessage(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("本地软件可升级至更高版本，点击以查看详情。"),
				QSystemTrayIcon::Information, 5000);
		}
		break;
	default:
		break;
	}
}

void CSystemTrayIcon::setHidden(bool hidden)
{
	if(!hidden)
	{
		m_hideShowAct->setText(QString::fromLocal8Bit("显示"));
		m_hideShowAct->setIcon(QIcon(":/Resources/show.png"));
	}
	else
	{
		m_hideShowAct->setText(QString::fromLocal8Bit("隐藏"));
		m_hideShowAct->setIcon(QIcon(":/Resources/hidden.png"));
	}
}