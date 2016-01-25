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

#include "CSplashscreen.h"

#include <QImageReader>
#include <QFont>

CSplashScreen::CSplashScreen(QWidget *parent) : QSplashScreen(parent)
{
	
}

CSplashScreen::~CSplashScreen()
{
	if (m_nTimerID > 0)
	{
		killTimer(m_nTimerID);
	}
}

CSplashScreen::CSplashScreen(QPixmap pixmap,const QString &text)
	: m_splashPixmap(pixmap)
{
	Qt::Alignment bottomLeft = Qt::AlignLeft | Qt::AlignBottom;
	setPixmap(pixmap);
	showMessage(text, bottomLeft, Qt::white);
	show();
}

void CSplashScreen::hide()
{
	if (m_nTimerID > 0)
	{
		killTimer(m_nTimerID);
	}
	QWidget::hide();
}

void CSplashScreen::showImage(const QString &strPath)
{
	/// ½âÎö¶¯»­Í¼Æ¬
	QImageReader oImageReader(strPath);
	if(oImageReader.canRead())
	{
		int nCount = oImageReader.imageCount();
		for (int nIndex = 0; nIndex < nCount; nIndex++)
		{
			QImage oImage = oImageReader.read();
			m_oImageList << oImage;
		}
		if (m_oImageList.count() > 0)
		{
			setPixmap(QPixmap::fromImage(m_oImageList.at(0)));
			m_nCurrFrame = 0;
			m_nTimerID = startTimer(200);
			//show();
		}
	}

	show();
}

void CSplashScreen::setText(const QString &text)
{
	Qt::Alignment bottomLeft = Qt::AlignLeft | Qt::AlignBottom;
	showMessage(text, bottomLeft, Qt::black);
}

void CSplashScreen::waitMainWindow(QWidget *widget)
{
	if (m_nTimerID > 0)
	{
		killTimer(m_nTimerID);
	}
	finish(widget);
}

void CSplashScreen::timerEvent(QTimerEvent *event)
{
	if (m_nTimerID == event->timerId())
	{
		setPixmap(QPixmap::fromImage(m_oImageList.at(m_nCurrFrame)));
		m_nCurrFrame++;
		if (m_nCurrFrame >= m_oImageList.count())
		{
			m_nCurrFrame = 0;
		}
	}
}