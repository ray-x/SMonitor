/********************************************************************************
** Form generated from reading UI file 'CHelpInfo.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHELPINFO_H
#define UI_CHELPINFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CHelpInfo
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QToolButton *tbtn_add;
    QSpacerItem *horizontalSpacer;
    QToolButton *tbtn_delete;
    QTableWidget *tableWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btn_Save;
    QPushButton *btn_SaveXml;

    void setupUi(QWidget *CHelpInfo)
    {
        if (CHelpInfo->objectName().isEmpty())
            CHelpInfo->setObjectName(QStringLiteral("CHelpInfo"));
        CHelpInfo->resize(512, 363);
        gridLayout = new QGridLayout(CHelpInfo);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setVerticalSpacing(2);
        frame = new QFrame(CHelpInfo);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 3, 0, 0);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        tbtn_add = new QToolButton(frame);
        tbtn_add->setObjectName(QStringLiteral("tbtn_add"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/plus.png"), QSize(), QIcon::Normal, QIcon::Off);
        tbtn_add->setIcon(icon);

        gridLayout_2->addWidget(tbtn_add, 0, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(317, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 1, 1, 1);

        tbtn_delete = new QToolButton(frame);
        tbtn_delete->setObjectName(QStringLiteral("tbtn_delete"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Resources/minus.png"), QSize(), QIcon::Normal, QIcon::Off);
        tbtn_delete->setIcon(icon1);

        gridLayout_2->addWidget(tbtn_delete, 0, 3, 1, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);

        tableWidget = new QTableWidget(CHelpInfo);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        QFont font;
        font.setPointSize(9);
        tableWidget->setFont(font);
        tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        tableWidget->setAlternatingRowColors(true);
        tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
        tableWidget->setTextElideMode(Qt::ElideRight);
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->verticalHeader()->setVisible(false);

        gridLayout->addWidget(tableWidget, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        btn_Save = new QPushButton(CHelpInfo);
        btn_Save->setObjectName(QStringLiteral("btn_Save"));

        horizontalLayout->addWidget(btn_Save);

        btn_SaveXml = new QPushButton(CHelpInfo);
        btn_SaveXml->setObjectName(QStringLiteral("btn_SaveXml"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btn_SaveXml->sizePolicy().hasHeightForWidth());
        btn_SaveXml->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn_SaveXml);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 1);


        retranslateUi(CHelpInfo);

        QMetaObject::connectSlotsByName(CHelpInfo);
    } // setupUi

    void retranslateUi(QWidget *CHelpInfo)
    {
        CHelpInfo->setWindowTitle(QApplication::translate("CHelpInfo", "CHelpInfo", 0));
        label->setText(QApplication::translate("CHelpInfo", "\345\256\242\346\210\267\347\253\257\347\273\264\346\212\244\344\277\241\346\201\257\347\274\226\350\276\221", 0));
#ifndef QT_NO_TOOLTIP
        tbtn_add->setToolTip(QApplication::translate("CHelpInfo", "\346\267\273\345\212\240\350\275\257\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        tbtn_add->setText(QString());
#ifndef QT_NO_TOOLTIP
        tbtn_delete->setToolTip(QApplication::translate("CHelpInfo", "\345\210\240\351\231\244", 0));
#endif // QT_NO_TOOLTIP
        tbtn_delete->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("CHelpInfo", "\346\240\207\351\242\230", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("CHelpInfo", "\345\206\205\345\256\271", 0));
        btn_Save->setText(QApplication::translate("CHelpInfo", "\344\277\235\345\255\230", 0));
        btn_SaveXml->setText(QApplication::translate("CHelpInfo", "\347\224\237\346\210\220 XML", 0));
    } // retranslateUi

};

namespace Ui {
    class CHelpInfo: public Ui_CHelpInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHELPINFO_H
