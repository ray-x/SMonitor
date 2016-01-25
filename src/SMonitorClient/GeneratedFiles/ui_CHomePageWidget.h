/********************************************************************************
** Form generated from reading UI file 'CHomePageWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHOMEPAGEWIDGET_H
#define UI_CHOMEPAGEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CHomePageWidget
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *tbtn_onekeyApp;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QToolButton *tbtn_msgMore;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_IP;
    QLabel *label_Svr;

    void setupUi(QWidget *CHomePageWidget)
    {
        if (CHomePageWidget->objectName().isEmpty())
            CHomePageWidget->setObjectName(QStringLiteral("CHomePageWidget"));
        CHomePageWidget->resize(526, 422);
        gridLayout_2 = new QGridLayout(CHomePageWidget);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(9, 9, 9, 9);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(CHomePageWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        horizontalSpacer_2 = new QSpacerItem(241, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        tbtn_onekeyApp = new QPushButton(CHomePageWidget);
        tbtn_onekeyApp->setObjectName(QStringLiteral("tbtn_onekeyApp"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tbtn_onekeyApp->sizePolicy().hasHeightForWidth());
        tbtn_onekeyApp->setSizePolicy(sizePolicy);
        tbtn_onekeyApp->setMinimumSize(QSize(0, 26));
        tbtn_onekeyApp->setIconSize(QSize(16, 16));

        horizontalLayout_3->addWidget(tbtn_onekeyApp);


        gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(CHomePageWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        horizontalSpacer = new QSpacerItem(185, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        tbtn_msgMore = new QToolButton(CHomePageWidget);
        tbtn_msgMore->setObjectName(QStringLiteral("tbtn_msgMore"));
        sizePolicy.setHeightForWidth(tbtn_msgMore->sizePolicy().hasHeightForWidth());
        tbtn_msgMore->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        tbtn_msgMore->setFont(font);
        tbtn_msgMore->setStyleSheet(QStringLiteral(""));
        tbtn_msgMore->setIconSize(QSize(16, 13));
        tbtn_msgMore->setToolButtonStyle(Qt::ToolButtonTextOnly);
        tbtn_msgMore->setAutoRaise(false);

        horizontalLayout->addWidget(tbtn_msgMore);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 1);

        scrollArea = new QScrollArea(CHomePageWidget);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setMinimumSize(QSize(0, 130));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 506, 310));
        gridLayout_3 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 1, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_IP = new QLabel(CHomePageWidget);
        label_IP->setObjectName(QStringLiteral("label_IP"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_IP->sizePolicy().hasHeightForWidth());
        label_IP->setSizePolicy(sizePolicy2);
        label_IP->setMinimumSize(QSize(0, 25));
        label_IP->setMaximumSize(QSize(16777215, 25));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(9);
        label_IP->setFont(font1);
        label_IP->setStyleSheet(QLatin1String("QLabel{background-color:#F0F0F0;\n"
"border-top:1px solid lightgray;}"));
        label_IP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_IP);

        label_Svr = new QLabel(CHomePageWidget);
        label_Svr->setObjectName(QStringLiteral("label_Svr"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_Svr->sizePolicy().hasHeightForWidth());
        label_Svr->setSizePolicy(sizePolicy3);
        label_Svr->setMinimumSize(QSize(0, 25));
        label_Svr->setMaximumSize(QSize(16777215, 25));
        label_Svr->setFont(font1);
        label_Svr->setStyleSheet(QLatin1String("QLabel{background-color:#F0F0F0;\n"
"border-top:1px solid lightgray;}"));
        label_Svr->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_Svr);


        gridLayout_2->addLayout(horizontalLayout_2, 1, 0, 1, 1);


        retranslateUi(CHomePageWidget);

        QMetaObject::connectSlotsByName(CHomePageWidget);
    } // setupUi

    void retranslateUi(QWidget *CHomePageWidget)
    {
        CHomePageWidget->setWindowTitle(QApplication::translate("CHomePageWidget", "CHomePageWidget", 0));
        label_2->setText(QApplication::translate("CHomePageWidget", "\346\243\256\351\251\254\345\205\254\345\217\270\345\272\224\347\224\250\345\210\227\350\241\250", 0));
        tbtn_onekeyApp->setText(QApplication::translate("CHomePageWidget", "\344\270\200\351\224\256\345\256\211\350\243\205", 0));
        label->setText(QApplication::translate("CHomePageWidget", "\346\243\256\351\251\254\345\205\254\345\217\270\346\266\210\346\201\257\346\217\220\347\244\272", 0));
#ifndef QT_NO_TOOLTIP
        tbtn_msgMore->setToolTip(QApplication::translate("CHomePageWidget", "\346\237\245\347\234\213\345\205\250\351\203\250\346\266\210\346\201\257", 0));
#endif // QT_NO_TOOLTIP
        tbtn_msgMore->setText(QString());
        label_IP->setText(QString());
        label_Svr->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CHomePageWidget: public Ui_CHomePageWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHOMEPAGEWIDGET_H
