/********************************************************************************
** Form generated from reading UI file 'LoginIn.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGININ_H
#define UI_LOGININ_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CLoginDialog
{
public:
    QToolButton *tbtn_close;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label_login;
    QLineEdit *lne_login;
    QPushButton *tbtn_signIn;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QDialog *CLoginDialog)
    {
        if (CLoginDialog->objectName().isEmpty())
            CLoginDialog->setObjectName(QStringLiteral("CLoginDialog"));
        CLoginDialog->resize(302, 282);
        CLoginDialog->setMinimumSize(QSize(302, 282));
        CLoginDialog->setMaximumSize(QSize(302, 282));
        CLoginDialog->setStyleSheet(QStringLiteral("QDialog#CLoginDialog{border-image: url(:/Resources/background_login.png);}"));
        tbtn_close = new QToolButton(CLoginDialog);
        tbtn_close->setObjectName(QStringLiteral("tbtn_close"));
        tbtn_close->setGeometry(QRect(269, 14, 27, 24));
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
        layoutWidget = new QWidget(CLoginDialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 50, 298, 211));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_login = new QLabel(layoutWidget);
        label_login->setObjectName(QStringLiteral("label_login"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_login->sizePolicy().hasHeightForWidth());
        label_login->setSizePolicy(sizePolicy);
        label_login->setMinimumSize(QSize(200, 45));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        label_login->setFont(font);
        label_login->setStyleSheet(QStringLiteral(""));
        label_login->setMargin(5);
        label_login->setIndent(5);

        verticalLayout->addWidget(label_login);

        lne_login = new QLineEdit(layoutWidget);
        lne_login->setObjectName(QStringLiteral("lne_login"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lne_login->sizePolicy().hasHeightForWidth());
        lne_login->setSizePolicy(sizePolicy1);
        lne_login->setMinimumSize(QSize(200, 45));
        lne_login->setFont(font);
        lne_login->setStyleSheet(QStringLiteral(""));
        lne_login->setMaxLength(11);
        lne_login->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout->addWidget(lne_login);


        verticalLayout_2->addLayout(verticalLayout);

        tbtn_signIn = new QPushButton(layoutWidget);
        tbtn_signIn->setObjectName(QStringLiteral("tbtn_signIn"));
        tbtn_signIn->setMinimumSize(QSize(200, 40));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(16);
        font1.setBold(false);
        font1.setWeight(50);
        font1.setStrikeOut(false);
        font1.setKerning(false);
        font1.setStyleStrategy(QFont::PreferDefault);
        tbtn_signIn->setFont(font1);
        tbtn_signIn->setStyleSheet(QLatin1String("border-image: url(:/Resources/login.png);\n"
"color:white;"));

        verticalLayout_2->addWidget(tbtn_signIn);


        horizontalLayout_2->addLayout(verticalLayout_2);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        retranslateUi(CLoginDialog);

        QMetaObject::connectSlotsByName(CLoginDialog);
    } // setupUi

    void retranslateUi(QDialog *CLoginDialog)
    {
        CLoginDialog->setWindowTitle(QApplication::translate("CLoginDialog", "Dialog", 0));
        tbtn_close->setText(QString());
        label_login->setText(QApplication::translate("CLoginDialog", "\350\276\223\345\205\245\351\227\250\345\272\227\347\224\265\350\257\235\357\274\232", 0));
        lne_login->setInputMask(QString());
        tbtn_signIn->setText(QApplication::translate("CLoginDialog", "\346\263\250   \345\206\214", 0));
    } // retranslateUi

};

namespace Ui {
    class CLoginDialog: public Ui_CLoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGININ_H
