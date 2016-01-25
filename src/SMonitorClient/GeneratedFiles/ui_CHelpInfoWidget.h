/********************************************************************************
** Form generated from reading UI file 'CHelpInfoWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHELPINFOWIDGET_H
#define UI_CHELPINFOWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CHelpInfoWidget
{
public:
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_5;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_12;
    QLabel *label_CurNum;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_11;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit_Num;
    QPushButton *btn_SetNum;
    QSpacerItem *horizontalSpacer;
    QLabel *label_Version;

    void setupUi(QWidget *CHelpInfoWidget)
    {
        if (CHelpInfoWidget->objectName().isEmpty())
            CHelpInfoWidget->setObjectName(QStringLiteral("CHelpInfoWidget"));
        CHelpInfoWidget->resize(552, 382);
        gridLayout_3 = new QGridLayout(CHelpInfoWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        groupBox = new QGroupBox(CHelpInfoWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_5 = new QGridLayout(groupBox);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(groupBox);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setStyleSheet(QStringLiteral("QScrollArea{border:none;}"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 532, 241));
        gridLayout_4 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));

        gridLayout_4->addLayout(gridLayout, 0, 0, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout_5->addWidget(scrollArea, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(CHelpInfoWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMaximumSize(QSize(16777215, 85));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(12);
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(40);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QStringLiteral("label_12"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_12->sizePolicy().hasHeightForWidth());
        label_12->setSizePolicy(sizePolicy);
        label_12->setMaximumSize(QSize(160, 16777215));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_7->addWidget(label_12);

        label_CurNum = new QLabel(groupBox_2);
        label_CurNum->setObjectName(QStringLiteral("label_CurNum"));
        sizePolicy.setHeightForWidth(label_CurNum->sizePolicy().hasHeightForWidth());
        label_CurNum->setSizePolicy(sizePolicy);
        label_CurNum->setMinimumSize(QSize(225, 0));

        horizontalLayout_7->addWidget(label_CurNum);


        gridLayout_2->addLayout(horizontalLayout_7, 0, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(40);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QStringLiteral("label_11"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_11->sizePolicy().hasHeightForWidth());
        label_11->setSizePolicy(sizePolicy1);
        label_11->setMaximumSize(QSize(160, 16777215));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_6->addWidget(label_11);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lineEdit_Num = new QLineEdit(groupBox_2);
        lineEdit_Num->setObjectName(QStringLiteral("lineEdit_Num"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lineEdit_Num->sizePolicy().hasHeightForWidth());
        lineEdit_Num->setSizePolicy(sizePolicy2);
        lineEdit_Num->setMaxLength(11);

        horizontalLayout->addWidget(lineEdit_Num);

        btn_SetNum = new QPushButton(groupBox_2);
        btn_SetNum->setObjectName(QStringLiteral("btn_SetNum"));

        horizontalLayout->addWidget(btn_SetNum);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        horizontalLayout_6->addLayout(horizontalLayout);


        gridLayout_2->addLayout(horizontalLayout_6, 1, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_2, 0, 0, 1, 1);

        label_Version = new QLabel(CHelpInfoWidget);
        label_Version->setObjectName(QStringLiteral("label_Version"));
        label_Version->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_Version, 2, 0, 1, 1);


        retranslateUi(CHelpInfoWidget);

        QMetaObject::connectSlotsByName(CHelpInfoWidget);
    } // setupUi

    void retranslateUi(QWidget *CHelpInfoWidget)
    {
        CHelpInfoWidget->setWindowTitle(QApplication::translate("CHelpInfoWidget", "CHelpInfoWidget", 0));
        groupBox->setTitle(QApplication::translate("CHelpInfoWidget", "\345\270\256\345\212\251\344\277\241\346\201\257", 0));
        groupBox_2->setTitle(QApplication::translate("CHelpInfoWidget", "\351\227\250\345\272\227\347\224\265\350\257\235", 0));
        label_12->setText(QApplication::translate("CHelpInfoWidget", "\345\267\262\346\263\250\345\206\214\351\227\250\345\272\227\347\224\265\350\257\235\357\274\232", 0));
        label_CurNum->setText(QString());
        label_11->setText(QApplication::translate("CHelpInfoWidget", "\350\256\276\347\275\256\346\226\260\347\224\265\350\257\235\357\274\232", 0));
        lineEdit_Num->setPlaceholderText(QApplication::translate("CHelpInfoWidget", "\350\276\223\345\205\245\347\224\265\350\257\235\345\217\267\347\240\201", 0));
#ifndef QT_NO_TOOLTIP
        btn_SetNum->setToolTip(QApplication::translate("CHelpInfoWidget", "\350\256\276\347\275\256\351\227\250\345\272\227\347\274\226\345\217\267", 0));
#endif // QT_NO_TOOLTIP
        btn_SetNum->setText(QApplication::translate("CHelpInfoWidget", "\350\256\276\347\275\256", 0));
        label_Version->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CHelpInfoWidget: public Ui_CHelpInfoWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHELPINFOWIDGET_H
