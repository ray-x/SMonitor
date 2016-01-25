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

#include "CStateButton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

#include <QDebug>

CStateButton::CStateButton(const QString& icon, QWidget *parent /* = 0 */, bool bSinglePix /* = true */)
	: QToolButton(parent)
	, m_bDescOrder(false)
	, m_pixMapVec(QVector<QPixmap> ())
	, m_mutiPixmapPreffix(QString())
	, m_bSinglePixmap(bSinglePix)
{
	setAutoRaise(true);
	setMouseTracking(true);
	setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	setAttribute(Qt::WA_TranslucentBackground);

	if(m_bSinglePixmap)
	{
		QPixmap pixmap(icon);
		for(int i=0; i != 4; i++){
			m_pixMapVec.append(pixmap.copy(i * (pixmap.width()/4), 0, pixmap.width()/4, pixmap.height()));
		}
		setFixedSize(QSize(27, 24));
		setIconSize(QSize(27, 24));
	}
	else
	{
		m_mutiPixmapPreffix = icon;
	}

	setState(Normal);
}

CStateButton::~CStateButton()
{
	qDebug() << "~CStateButton()";
}

void CStateButton::setState(State state)
{
	if(m_bDescOrder)
		m_curState = (state == Normal) ? Disable : state;
	else
		m_curState = state;
	update();
}

void CStateButton::setPixDescOrder(bool desc)
{
	m_bDescOrder = desc;
	setState(Normal);
}

void CStateButton::enterEvent(QEvent *)
{
	setState(Enter);
}

void CStateButton::leaveEvent(QEvent *)
{
	setState(Normal);
}

void CStateButton::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		setState(Pressed);
	}

	QToolButton::mousePressEvent(event);
}

void CStateButton::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		setState(Normal);
	}
	QToolButton::mouseReleaseEvent(event);
}

void CStateButton::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	if(m_bSinglePixmap)
	{
		painter.drawPixmap(this->rect(), m_pixMapVec.at(m_curState));
	}
	else
	{
		QString path = "";
		switch(m_curState)
		{
		case Normal:
			path += m_mutiPixmapPreffix + "_normal.png";
			break;
		case Enter:
			path = m_mutiPixmapPreffix + "_enter.png";
			break;
		case Pressed:
			path = m_mutiPixmapPreffix + "_pressed.png";
			break;
		default:
			break;
		}
		setFixedSize(QSize(27, 24));
		painter.drawPixmap(this->rect(), QPixmap(path));
	}
}