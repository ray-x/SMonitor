#include "common.h"
#include "CLoginDialog.h"
#include "CTitleWidget.h"
#include "CSplashscreen.h"
#include "NetClt.h"
#include "NetClass.h"
#include "CMessageDataMediator.h"
#include "CMessageEventMediator.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>



CLoginDialog::CLoginDialog(QWidget* parent /* = 0 */)
	: QDialog(parent)
	, m_bPressed(false)
	, m_splashScreen(nullptr)
	, m_waitid(0)
{
	setupUi(this);
	setWindowIcon(QIcon(":/Resources/normal.png"));
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	connect(tbtn_signIn, &QToolButton::clicked, this, &CLoginDialog::clientLogin);
	connect(tbtn_close, &QToolButton::clicked, this, &CLoginDialog::reject);
	connect(NetClass->m_pNetClt, &CNetClt::sigSendResult, this, &CLoginDialog::msgSendResult);
	connect(NetClass->m_pMessageEventMediator, &CMessageEventMediator::sigRegisterSuccess, this, &CLoginDialog::msgLoginIn);
	connect(NetClass->m_pMessageEventMediator, &CMessageEventMediator::sigError, 
		this, &CLoginDialog::msgError);

	QRegExp regx("[0-9]+$");
	QValidator *validator = new QRegExpValidator(regx, this);
	lne_login->setValidator(validator);
}

CLoginDialog::~CLoginDialog()
{
	qDebug() << "~CLoginDialog()";
	if(m_splashScreen)
	{
		delete m_splashScreen;
		m_splashScreen = nullptr;
	}
}

void CLoginDialog::mouseMoveEvent(QMouseEvent *event)
{
	if(m_bPressed)
	{
		QPoint movePos = event->globalPos() - m_movePoint;
		int distance = movePos.manhattanLength();
		bool bMoved =  distance > QApplication::startDragDistance() ? true : false;
		if(bMoved)
		{
			this->move(movePos);
		}
	}
}

void CLoginDialog::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(event->pos().y() < 48)
			m_bPressed = true;
	}
	//窗口移动距离
	m_movePoint = event->globalPos() - frameGeometry().topLeft();
}

void CLoginDialog::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_bPressed = false;
	}
}

void CLoginDialog::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), QBrush(QPixmap(":/Resources/background_login.png")));
}

void CLoginDialog::timerEvent(QTimerEvent *event)
{
	int id = event->timerId();
	if(m_waitid == id)
	{
		msgError(EMSG_REGISTERID, QString::fromLocal8Bit("接受数据超时， 请稍后重试"));
	}
}

void CLoginDialog::clientLogin()
{
	if(lne_login->text().length() < 11)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("错误"), 
			QString::fromLocal8Bit("电话号码长度为11位，请确认后重试"), 
			QString::fromLocal8Bit("确定"));
	}
	else
	{
		MessageDataMediator->m_strNewId = lne_login->text().trimmed();
		if(NetClass->m_pNetClt->SendMsg(EMSG_REGISTERID, true))
		{
			m_waitid = startTimer(60000); //接收超时设为1分钟
			this->showMinimized();
			m_splashScreen = new CSplashScreen(this);
			m_splashScreen->showImage(":/Resources/loading6.gif");
		}
		else
		{
			QMessageBox::critical(this, 
				QString::fromLocal8Bit("错误"), 
				QString::fromLocal8Bit("服务器链接异常,发送失败"),
				QString::fromLocal8Bit("确定"));
		}
	}
}

void CLoginDialog::msgSendResult(int nMsghead, bool result, const QString& info)
{
	MsgHead head = (MsgHead)nMsghead;
	switch(head)
	{
	case EMSG_REGISTERID:
		if(!result)
		{
			killTimer(m_waitid);
			ClientLogger->AddLog(QString::fromLocal8Bit("注册失败%1 发送注册消息失败！").arg(info));
			m_splashScreen->hide();
			QMessageBox::information(this, QString::fromLocal8Bit("失败"), 
				QString::fromLocal8Bit("发送注册消息失败！"), 
				QString::fromLocal8Bit("确定"));

		}
		break;
	}
}

void CLoginDialog::msgError(int nMsghead, const QString& error)
{
	killTimer(m_waitid);
	m_splashScreen->hide();
	QMessageBox::information(this, QString::fromLocal8Bit("失败"), 
		QString::fromLocal8Bit("注册失败 [%1]").arg(error), 
		QString::fromLocal8Bit("确定"));
}

void CLoginDialog::msgLoginIn()
{
	killTimer(m_waitid);
	m_splashScreen->hide();
	QMessageBox::information(this, QString::fromLocal8Bit("成功"), 
		QString::fromLocal8Bit("注册成功"), 
		QString::fromLocal8Bit("确定"));
	saveSettings();
	accept();
}

void CLoginDialog::saveSettings()
{
	//QString path = QApplication::applicationDirPath() + "/configure.ini";
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec("UTF-8");
	settings.setValue("RegisterID/ID", MessageDataMediator->m_strNewId);
	ClientLogger->AddLog(QString::fromLocal8Bit("注册%1成功").arg(MessageDataMediator->m_strNewId));
}