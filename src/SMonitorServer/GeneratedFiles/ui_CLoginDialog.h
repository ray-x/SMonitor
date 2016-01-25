/********************************************************************************
** Form generated from reading UI file 'CLoginDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLOGINDIALOG_H
#define UI_CLOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CLoginDialog
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLineEdit *lne_username;
    QLineEdit *lne_passwd;
    QPushButton *tbtn_login;
    QToolButton *tbtn_close;

    void setupUi(QDialog *CLoginDialog)
    {
        if (CLoginDialog->objectName().isEmpty())
            CLoginDialog->setObjectName(QStringLiteral("CLoginDialog"));
        CLoginDialog->resize(302, 282);
        CLoginDialog->setMinimumSize(QSize(302, 282));
        CLoginDialog->setMaximumSize(QSize(302, 282));
        CLoginDialog->setStyleSheet(QLatin1String("QDialog#CLoginDialog{\n"
"	border-image: url(:/Resources/background_login.png);\n"
"}"));
        gridLayout = new QGridLayout(CLoginDialog);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(CLoginDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setStyleSheet(QStringLiteral("QFrame{border:1px solid #ddd;border-radius:5px;}"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        layoutWidget = new QWidget(frame);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 80, 243, 181));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(8);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(35, 2, 15, 50);
        lne_username = new QLineEdit(layoutWidget);
        lne_username->setObjectName(QStringLiteral("lne_username"));
        lne_username->setMinimumSize(QSize(0, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(10);
        lne_username->setFont(font);
        lne_username->setStyleSheet(QLatin1String("background-color: transparent;\n"
"border: none;"));

        verticalLayout->addWidget(lne_username);

        lne_passwd = new QLineEdit(layoutWidget);
        lne_passwd->setObjectName(QStringLiteral("lne_passwd"));
        lne_passwd->setMinimumSize(QSize(0, 30));
        lne_passwd->setFont(font);
        lne_passwd->setStyleSheet(QLatin1String("background-color: transparent;\n"
"border: none;"));
        lne_passwd->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(lne_passwd);


        verticalLayout_2->addLayout(verticalLayout);

        tbtn_login = new QPushButton(layoutWidget);
        tbtn_login->setObjectName(QStringLiteral("tbtn_login"));
        tbtn_login->setMinimumSize(QSize(241, 41));
        tbtn_login->setMaximumSize(QSize(241, 41));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(16);
        tbtn_login->setFont(font1);
        tbtn_login->setStyleSheet(QLatin1String("border-image: url(:/Resources/login.png);\n"
"color:white;"));

        verticalLayout_2->addWidget(tbtn_login);

        tbtn_close = new QToolButton(frame);
        tbtn_close->setObjectName(QStringLiteral("tbtn_close"));
        tbtn_close->setGeometry(QRect(264, 13, 27, 24));
        tbtn_close->setMinimumSize(QSize(27, 24));
        tbtn_close->setMaximumSize(QSize(27, 24));
        tbtn_close->setStyleSheet(QLatin1String("QToolButton#tbtn_close{\n"
"	border-image: url(:/Resources/close.png);\n"
"}\n"
"\n"
"QToolButton#tbtn_close:hover{\n"
"	border-image: url(:/Resources/close_enter.png);\n"
"}\n"
"\n"
"QToolButton#tbtn_close:pressed{\n"
"	border-image: url(:/Resources/close_pressed.png);\n"
"}"));
        tbtn_close->setAutoRaise(true);

        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(CLoginDialog);

        QMetaObject::connectSlotsByName(CLoginDialog);
    } // setupUi

    void retranslateUi(QDialog *CLoginDialog)
    {
        CLoginDialog->setWindowTitle(QApplication::translate("CLoginDialog", "CLoginDialog", 0));
        lne_username->setPlaceholderText(QApplication::translate("CLoginDialog", "\347\224\250\346\210\267\345\220\215", 0));
        lne_passwd->setPlaceholderText(QApplication::translate("CLoginDialog", "\345\257\206\347\240\201", 0));
        tbtn_login->setText(QApplication::translate("CLoginDialog", "\347\231\273     \345\275\225", 0));
        tbtn_close->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CLoginDialog: public Ui_CLoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLOGINDIALOG_H
