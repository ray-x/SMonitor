#pragma once

#include <QSplashScreen>
#include <QTimerEvent>

class CSplashScreen : public QSplashScreen
{
public:
	CSplashScreen(QPixmap pixmap,const QString &text = "");
	CSplashScreen(QWidget *parent = 0);
	~CSplashScreen();

	void setText(const QString &text);
	void waitMainWindow(QWidget *widget);
	void showImage(const QString &strPath);
	void hide();
protected:

	virtual void timerEvent(QTimerEvent *event);

private:
	/// 静态图片
	QPixmap m_splashPixmap;
	/// gif图片
	QList<QImage> m_oImageList;
	/// 时间控制
	int m_nTimerID;
	/// 当前图片标示
	int m_nCurrFrame;
};