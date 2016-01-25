#ifndef CPASSWDCONFIG_H
#define CPASSWDCONFIG_H

#include <QDialog>

class CTitleWidget;
class QTableWidget;
class QDialogButtonBox;

#define SEMIRPASSWD ("smfwk")
#define BALABALAPASSWD ("balaitbalait")

class CPasswdConfig : public QDialog
{
	Q_OBJECT

public:
	CPasswdConfig(QWidget *parent = 0);
	~CPasswdConfig();

	void SetBrands(const QStringList& brands);

public	Q_SLOTS:
	void acceptPasswd();
	void dataChanged();

protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

private:
	void setItem(const QString& brand);

private:
	CTitleWidget*		m_titleWidget;
	QTableWidget*		m_tableWidget;
	QDialogButtonBox*	m_buttonBox;

	bool				m_bPressed;
	QPoint				m_movePoint;

	QStringList			m_pBrandList;

	bool				m_bAnyChanged;
};

#endif // CPASSWDCONFIG_H
