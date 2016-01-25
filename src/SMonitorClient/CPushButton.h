#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H

#include <QPushButton>
#include <QLinearGradient>

class CPushButton : public QPushButton
{
	Q_OBJECT

public:
	enum State{
		Normal,
		Enter,
		Pressed,
		Disable
	};
	CPushButton(const QChar& fchar, const QString& text, QWidget *parent);
	~CPushButton();

	void setState(State state);

	void setShowTips(bool bTips = false);
	
public Q_SLOTS:
	void setTipsNumber(int num);

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

private:
	QLinearGradient linearGraident(State state);

private:
	QString m_btnText;
	QChar m_btnIconChar;
	QSize m_btnIconCharSize;
	QSize m_btnTextSize;
	State m_curState;
	QFont m_btnIconFont;
	QFont m_btnTextFont;

	bool m_bShowTips;
	int m_tipsNumber;
};

#endif // CPUSHBUTTON_H
