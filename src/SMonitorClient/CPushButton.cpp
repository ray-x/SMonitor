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

#include "CPushButton.h"

#include <QPainter>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QMouseEvent>

#include "CMisc.h"

CPushButton::CPushButton(const QChar& fchar, const QString& text, QWidget *parent)
	: QPushButton(parent)
	, m_btnIconChar(fchar)
	, m_btnText(text)
	, m_curState(Normal)
	, m_bShowTips(false)
	, m_tipsNumber(0)
{
	setFlat(true);
	setCheckable(true);
	/*setFixedSize(100, 120);*/
	setObjectName("sideBarButton");
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	m_btnIconFont = CMisc::FontAwesome(28);
	QFontMetrics metricsIcon(m_btnIconFont);
	m_btnIconCharSize = metricsIcon.size(Qt::TextSingleLine, m_btnIconChar);

	m_btnTextFont = QFont("Microsoft yahei", 10);
	QFontMetrics metricsText(m_btnTextFont);
	m_btnTextSize = metricsText.size(Qt::TextSingleLine, m_btnText);
}

CPushButton::~CPushButton()
{

}

void CPushButton::setState(State state)
{
	m_curState = state;
	update();
}

void CPushButton::setShowTips(bool bTips /* = false */)
{
	m_bShowTips = bTips;
	update();
}

void CPushButton::setTipsNumber(int num)
{
	m_tipsNumber = num;
	update();
}

QLinearGradient CPushButton::linearGraident(State state)
{
	QLinearGradient linear(0, 0, 0, 1);
	switch(state)
	{
	case Normal:
		{
			linear.setColorAt(0, QColor(27, 137, 202));
			linear.setColorAt(1, QColor(16, 119, 181));
		}
		break;
	case Enter:
		{
			linear.setColorAt(0, QColor(12, 98, 162));
			linear.setColorAt(1, QColor(13, 94, 154));
		}
		break;
	case Pressed:
		{
			linear.setColorAt(0, QColor(5, 59, 101));
			linear.setColorAt(1, QColor(5, 58, 98));
		}
		break;
	case Disable:
		break;
	default:
		break;
	}
	return linear;
}

void CPushButton::enterEvent(QEvent *)
{
	setState(Enter);
}

void CPushButton::leaveEvent(QEvent *)
{
	setState(Normal);
}

void CPushButton::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		setState(Pressed);
	}
	QPushButton::mousePressEvent(event);
}

void CPushButton::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton && !this->isChecked())
	{
		setState(Normal);
	}
	QPushButton::mouseReleaseEvent(event);
}

void CPushButton::paintEvent(QPaintEvent* event)
{
	QPushButton::paintEvent(event);
	QPainter painter(this);
	painter.save();
	//计算图标所在的矩形
	QRect iconRect = QRect(rect().center().x() - m_btnIconCharSize.rwidth() / 2,
		rect().center().y() - m_btnIconCharSize.rheight() / 2 - 12, 
		m_btnIconCharSize.rwidth(), m_btnIconCharSize.rheight());

	//计算文字所在的矩形
	QRect textRect = QRect(rect().center().x() - m_btnTextSize.rwidth() / 2,
		rect().center().y() + 12, m_btnTextSize.rwidth(), m_btnTextSize.rheight());

	QLinearGradient linear = linearGraident(m_curState);
	QColor color = QColor(Qt::white);
	if(isChecked())
	{	
		linear = linearGraident(Pressed);
		color = QColor(44, 162, 252);
	}
	//绘制背景色
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(linear));
	painter.drawRect(rect());

	//绘制下部分割线
	painter.setPen(QColor(12, 101, 169));
	painter.drawLine(rect().bottomLeft(), rect().bottomRight());

	//绘制图标
	painter.setPen(QPen(color, 1));
	painter.setFont(m_btnIconFont);
	painter.drawText(iconRect,m_btnIconChar); 

	//绘制文字
	painter.setPen(QPen(color, 1));
	painter.setFont(m_btnTextFont);
	painter.drawText(textRect, m_btnText); 
	painter.restore();
	//绘制提醒
	if(m_bShowTips && m_tipsNumber > 0)
	{
		QString strNum = QString::number(m_tipsNumber);
		if(m_tipsNumber > 99)
			strNum = "99";
		
		//绘制文字
		painter.setRenderHint(QPainter::Antialiasing);

		painter.setPen(Qt::red);
		painter.setBrush(Qt::red);

		QFontMetrics fm(m_btnTextFont);
		QSize fmSize = fm.size(Qt::TextSingleLine, strNum);

		QRect elRect = QRect(iconRect.topRight().x(), iconRect.topRight().y() - 10, 20, 20);
		painter.drawEllipse(elRect);

		QRect txtRect = QRect(elRect.center().x() - fmSize.width() / 2 + 1, elRect.center().y() - fmSize.height() / 2,
			fmSize.width(), fmSize.height());

		painter.setPen(QPen(Qt::white));
		painter.setFont(m_btnTextFont);
		painter.drawText(txtRect, strNum); 
	}
}
