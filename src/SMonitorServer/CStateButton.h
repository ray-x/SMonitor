#ifndef CSTATEBUTTON_H
#define CSTATEBUTTON_H

#include <QToolButton>

class CStateButton : public QToolButton
{
	Q_OBJECT

public:
	enum State{
		Normal,
		Enter,
		Pressed,
		Disable
	};

	CStateButton(const QString& icon, QWidget *parent = 0, bool bSinglePix = true);
	~CStateButton();

	void setState(State state);
	void setPixDescOrder(bool desc);

protected:
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void paintEvent(QPaintEvent *event);

private:
	QVector<QPixmap> m_pixMapVec;
	State m_curState;
	bool m_bDescOrder;
	bool m_bSinglePixmap;
	QString m_mutiPixmapPreffix;
};

#endif // CSTATEBUTTON_H
