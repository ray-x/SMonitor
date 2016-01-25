/********************************************************************************
** Form generated from reading UI file 'CStatusWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSTATUSWIDGET_H
#define UI_CSTATUSWIDGET_H

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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CStatusWidget
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_5;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_serverIP;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLabel *label_download;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QLabel *label_statistics;
    QGroupBox *groupBox_changepasswd;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QLineEdit *lne_originalPasswd;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_8;
    QLineEdit *lne_newPasswd;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_9;
    QLineEdit *lne_affirmPasswd;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_Ok;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_4;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;
    QHBoxLayout *horizontalLayout_9;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btn_Send;

    void setupUi(QWidget *CStatusWidget)
    {
        if (CStatusWidget->objectName().isEmpty())
            CStatusWidget->setObjectName(QStringLiteral("CStatusWidget"));
        CStatusWidget->resize(549, 413);
        gridLayout = new QGridLayout(CStatusWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(20, 20, 20, 20);
        groupBox = new QGroupBox(CStatusWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_5 = new QGridLayout(groupBox);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        gridLayout_5->setContentsMargins(20, 20, 20, 20);
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        groupBox_2 = new QGroupBox(groupBox);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        groupBox_2->setMinimumSize(QSize(0, 190));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(label);

        label_serverIP = new QLabel(groupBox_2);
        label_serverIP->setObjectName(QStringLiteral("label_serverIP"));
        sizePolicy1.setHeightForWidth(label_serverIP->sizePolicy().hasHeightForWidth());
        label_serverIP->setSizePolicy(sizePolicy1);
        label_serverIP->setMinimumSize(QSize(80, 0));

        horizontalLayout->addWidget(label_serverIP);


        gridLayout_2->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(label_3);

        label_download = new QLabel(groupBox_2);
        label_download->setObjectName(QStringLiteral("label_download"));
        sizePolicy1.setHeightForWidth(label_download->sizePolicy().hasHeightForWidth());
        label_download->setSizePolicy(sizePolicy1);
        label_download->setMinimumSize(QSize(80, 0));

        horizontalLayout_2->addWidget(label_download);


        gridLayout_2->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(label_5);

        label_statistics = new QLabel(groupBox_2);
        label_statistics->setObjectName(QStringLiteral("label_statistics"));
        sizePolicy1.setHeightForWidth(label_statistics->sizePolicy().hasHeightForWidth());
        label_statistics->setSizePolicy(sizePolicy1);
        label_statistics->setMinimumSize(QSize(80, 0));

        horizontalLayout_3->addWidget(label_statistics);


        gridLayout_2->addLayout(horizontalLayout_3, 2, 0, 1, 1);


        horizontalLayout_5->addWidget(groupBox_2);

        groupBox_changepasswd = new QGroupBox(groupBox);
        groupBox_changepasswd->setObjectName(QStringLiteral("groupBox_changepasswd"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_changepasswd->sizePolicy().hasHeightForWidth());
        groupBox_changepasswd->setSizePolicy(sizePolicy2);
        groupBox_changepasswd->setCheckable(true);
        gridLayout_3 = new QGridLayout(groupBox_changepasswd);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_7 = new QLabel(groupBox_changepasswd);
        label_7->setObjectName(QStringLiteral("label_7"));
        sizePolicy1.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy1);

        horizontalLayout_6->addWidget(label_7);

        lne_originalPasswd = new QLineEdit(groupBox_changepasswd);
        lne_originalPasswd->setObjectName(QStringLiteral("lne_originalPasswd"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(lne_originalPasswd->sizePolicy().hasHeightForWidth());
        lne_originalPasswd->setSizePolicy(sizePolicy3);
        QFont font;
        font.setPointSize(6);
        lne_originalPasswd->setFont(font);
        lne_originalPasswd->setMaxLength(6);
        lne_originalPasswd->setEchoMode(QLineEdit::Password);

        horizontalLayout_6->addWidget(lne_originalPasswd);


        gridLayout_3->addLayout(horizontalLayout_6, 0, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_8 = new QLabel(groupBox_changepasswd);
        label_8->setObjectName(QStringLiteral("label_8"));
        sizePolicy1.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(label_8);

        lne_newPasswd = new QLineEdit(groupBox_changepasswd);
        lne_newPasswd->setObjectName(QStringLiteral("lne_newPasswd"));
        sizePolicy3.setHeightForWidth(lne_newPasswd->sizePolicy().hasHeightForWidth());
        lne_newPasswd->setSizePolicy(sizePolicy3);
        lne_newPasswd->setFont(font);
        lne_newPasswd->setMaxLength(6);
        lne_newPasswd->setEchoMode(QLineEdit::Password);

        horizontalLayout_4->addWidget(lne_newPasswd);


        gridLayout_3->addLayout(horizontalLayout_4, 1, 0, 1, 1);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, -1, -1, -1);
        label_9 = new QLabel(groupBox_changepasswd);
        label_9->setObjectName(QStringLiteral("label_9"));
        sizePolicy1.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy1);

        horizontalLayout_8->addWidget(label_9);

        lne_affirmPasswd = new QLineEdit(groupBox_changepasswd);
        lne_affirmPasswd->setObjectName(QStringLiteral("lne_affirmPasswd"));
        sizePolicy3.setHeightForWidth(lne_affirmPasswd->sizePolicy().hasHeightForWidth());
        lne_affirmPasswd->setSizePolicy(sizePolicy3);
        lne_affirmPasswd->setFont(font);
        lne_affirmPasswd->setMaxLength(6);
        lne_affirmPasswd->setEchoMode(QLineEdit::Password);
        lne_affirmPasswd->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_8->addWidget(lne_affirmPasswd);


        gridLayout_3->addLayout(horizontalLayout_8, 2, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        btn_Ok = new QPushButton(groupBox_changepasswd);
        btn_Ok->setObjectName(QStringLiteral("btn_Ok"));

        horizontalLayout_7->addWidget(btn_Ok);


        gridLayout_3->addLayout(horizontalLayout_7, 3, 0, 1, 1);


        horizontalLayout_5->addWidget(groupBox_changepasswd);


        gridLayout_5->addLayout(horizontalLayout_5, 0, 0, 1, 1);

        groupBox_3 = new QGroupBox(groupBox);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_4 = new QGridLayout(groupBox_3);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        textEdit = new QTextEdit(groupBox_3);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        sizePolicy.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy);
        textEdit->setMaximumSize(QSize(16777215, 90));
        textEdit->setUndoRedoEnabled(false);
        textEdit->setAcceptRichText(false);

        verticalLayout->addWidget(textEdit);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_2);

        btn_Send = new QPushButton(groupBox_3);
        btn_Send->setObjectName(QStringLiteral("btn_Send"));

        horizontalLayout_9->addWidget(btn_Send);


        verticalLayout->addLayout(horizontalLayout_9);


        gridLayout_4->addLayout(verticalLayout, 0, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_3, 1, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);


        retranslateUi(CStatusWidget);

        QMetaObject::connectSlotsByName(CStatusWidget);
    } // setupUi

    void retranslateUi(QWidget *CStatusWidget)
    {
        CStatusWidget->setWindowTitle(QApplication::translate("CStatusWidget", "CStatusWidget", 0));
        groupBox->setTitle(QString());
        groupBox_2->setTitle(QApplication::translate("CStatusWidget", "\346\234\215\345\212\241\347\212\266\346\200\201", 0));
        label->setText(QApplication::translate("CStatusWidget", "\346\234\215\345\212\241\345\231\250IP\357\274\232", 0));
        label_serverIP->setText(QApplication::translate("CStatusWidget", "192.168.0.1", 0));
        label_3->setText(QApplication::translate("CStatusWidget", "\344\270\213\350\275\275\346\234\215\345\212\241\357\274\232", 0));
        label_download->setText(QApplication::translate("CStatusWidget", "\346\255\243\345\270\270", 0));
        label_5->setText(QApplication::translate("CStatusWidget", "\347\212\266\346\200\201\347\273\237\350\256\241\357\274\232", 0));
        label_statistics->setText(QApplication::translate("CStatusWidget", "\346\255\243\345\270\270", 0));
        groupBox_changepasswd->setTitle(QApplication::translate("CStatusWidget", "\347\256\241\347\220\206\345\221\230\345\257\206\347\240\201\344\277\256\346\224\271", 0));
        label_7->setText(QApplication::translate("CStatusWidget", "\345\216\237\345\247\213\345\257\206\347\240\201\357\274\232", 0));
        lne_originalPasswd->setInputMask(QString());
        label_8->setText(QApplication::translate("CStatusWidget", "\346\226\260 \345\257\206 \347\240\201\357\274\232", 0));
        label_9->setText(QApplication::translate("CStatusWidget", "\347\241\256\350\256\244\345\257\206\347\240\201\357\274\232", 0));
        lne_affirmPasswd->setInputMask(QString());
        btn_Ok->setText(QApplication::translate("CStatusWidget", "\347\241\256\350\256\244", 0));
        groupBox_3->setTitle(QApplication::translate("CStatusWidget", "\346\266\210\346\201\257\346\216\250\351\200\201", 0));
        textEdit->setPlaceholderText(QApplication::translate("CStatusWidget", "\350\276\223\345\205\245\351\234\200\350\246\201\346\216\250\351\200\201\347\232\204\346\266\210\346\201\257\357\274\214\346\234\200\345\244\232\345\217\252\350\203\275\350\276\223\345\205\245128\345\255\227", 0));
        btn_Send->setText(QApplication::translate("CStatusWidget", "\346\216\250\351\200\201", 0));
    } // retranslateUi

};

namespace Ui {
    class CStatusWidget: public Ui_CStatusWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSTATUSWIDGET_H
