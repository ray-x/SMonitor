#ifndef CTITLEWIDGET_H
#define CTITLEWIDGET_H

#include <QWidget>

class CStateButton;
class QLabel;
class QFrame;

class CTitleWidget : public QWidget
{
	Q_OBJECT

public:
	CTitleWidget(QWidget *parent);
	~CTitleWidget();

	void setTitle(const QString& title);
	void setWindowIcon(const QString& icon);

	void setMinimumHint(bool minimum);
	void setCloseButtonHint(bool closeable);

Q_SIGNALS:
	void minimumzed();
	void closed();

private:
	QFrame*			m_titleFrame;
	QLabel*			m_iconLabel;
	QLabel*			m_titleLabel;
	CStateButton*	m_minimumBtn;
	CStateButton*	m_closeBtn;

};

#endif // CTITLEWIDGET_H
