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

#include "CTitleWidget.h"
#include "CStateButton.h"
#include "common.h"

#include <QLabel>
#include <QFrame>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QFontDatabase>
#include <QApplication>

#include <QDebug>

#define NEW_TOOLBUTTON(ptr, icon, tip)						\
	ptr = new CStateButton(icon, this, false);						\
	ptr->setToolTip(tip);									\
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);	\


CTitleWidget::CTitleWidget(QWidget *parent)
	: QWidget(parent)
{
	setObjectName("titleWidget");
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	int fontId = QFontDatabase::addApplicationFont(":/Resources/fontawesome-webfont.ttf");
	QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);

	m_iconLabel = new QLabel(this);
	m_iconLabel->setContentsMargins(4, 0, 4, 0);
	m_iconLabel->setFont(QFont(fontName, 11));
	m_iconLabel->setText(QChar(0xf015));
	m_iconLabel->setObjectName("iconLabel");
	m_iconLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

	m_titleLabel = new QLabel(this);
	m_titleLabel->setObjectName("titleLabel");
	m_titleLabel->setFont(QFont("Microsoft Yahei", 10));
	m_titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	NEW_TOOLBUTTON(m_minimumBtn, ":/Resources/mini", QString::fromLocal8Bit("最小化"));
	NEW_TOOLBUTTON(m_closeBtn, ":/Resources/close", QString::fromLocal8Bit("关闭"));

	connect(m_minimumBtn, &QToolButton::clicked, this, &CTitleWidget::minimumzed);
	connect(m_closeBtn, &QToolButton::clicked, this, &CTitleWidget::closed);

	m_titleFrame = new QFrame(this);
	m_titleFrame->setFixedHeight(26);
	m_titleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_titleFrame->setObjectName("titleFrame");

	QHBoxLayout* titleLayout = new QHBoxLayout(m_titleFrame);
	titleLayout->setMargin(0);
	titleLayout->setSpacing(0);
	titleLayout->addWidget(m_iconLabel, 0, Qt::AlignLeft);
	titleLayout->addWidget(m_titleLabel, 1, Qt::AlignLeft);
	titleLayout->addWidget(m_minimumBtn, 1, Qt::AlignRight);
	titleLayout->addWidget(m_closeBtn, 0, Qt::AlignRight);

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(m_titleFrame);
}

CTitleWidget::~CTitleWidget()
{
	DELETE_POINTER(m_titleLabel);
	DELETE_POINTER(m_iconLabel);
	DELETE_POINTER(m_minimumBtn);
	DELETE_POINTER(m_closeBtn);
	qDebug() << "~CTitleWidget()";
}

void CTitleWidget::setTitle(const QString& title)
{
	m_titleLabel->setText(title);
}

void CTitleWidget::setMinimumHint(bool minimum)
{
	m_minimumBtn->setVisible(minimum);
}

void CTitleWidget::setCloseButtonHint(bool closeable)
{
	m_closeBtn->setVisible(closeable);
}