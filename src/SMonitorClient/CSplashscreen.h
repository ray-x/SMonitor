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
	/// ��̬ͼƬ
	QPixmap m_splashPixmap;
	/// gifͼƬ
	QList<QImage> m_oImageList;
	/// ʱ�����
	int m_nTimerID;
	/// ��ǰͼƬ��ʾ
	int m_nCurrFrame;
};