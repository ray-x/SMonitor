/********************************************************************************
** Form generated from reading UI file 'CSoftUpgredeWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSOFTUPGREDEWIDGET_H
#define UI_CSOFTUPGREDEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CSoftUpgredeWidget
{
public:
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox_brand;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QLineEdit *lineEdit_brand;
    QToolButton *tbtn_addbrand;
    QToolButton *tbtn_Passwd;
    QTableWidget *tableWidget;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *tbtn_add;
    QToolButton *tbtn_delete;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btn_Init;
    QPushButton *btn_SaveData;
    QPushButton *btn_Save;

    void setupUi(QWidget *CSoftUpgredeWidget)
    {
        if (CSoftUpgredeWidget->objectName().isEmpty())
            CSoftUpgredeWidget->setObjectName(QStringLiteral("CSoftUpgredeWidget"));
        CSoftUpgredeWidget->resize(613, 392);
        gridLayout_2 = new QGridLayout(CSoftUpgredeWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(CSoftUpgredeWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        comboBox_brand = new QComboBox(CSoftUpgredeWidget);
        comboBox_brand->setObjectName(QStringLiteral("comboBox_brand"));
        comboBox_brand->setMinimumSize(QSize(110, 0));

        horizontalLayout->addWidget(comboBox_brand);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_2 = new QLabel(CSoftUpgredeWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        lineEdit_brand = new QLineEdit(CSoftUpgredeWidget);
        lineEdit_brand->setObjectName(QStringLiteral("lineEdit_brand"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit_brand->sizePolicy().hasHeightForWidth());
        lineEdit_brand->setSizePolicy(sizePolicy);
        lineEdit_brand->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(lineEdit_brand);

        tbtn_addbrand = new QToolButton(CSoftUpgredeWidget);
        tbtn_addbrand->setObjectName(QStringLiteral("tbtn_addbrand"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/plus.png"), QSize(), QIcon::Normal, QIcon::Off);
        tbtn_addbrand->setIcon(icon);
        tbtn_addbrand->setToolButtonStyle(Qt::ToolButtonIconOnly);

        horizontalLayout->addWidget(tbtn_addbrand);

        tbtn_Passwd = new QToolButton(CSoftUpgredeWidget);
        tbtn_Passwd->setObjectName(QStringLiteral("tbtn_Passwd"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Resources/key.png"), QSize(), QIcon::Normal, QIcon::Off);
        tbtn_Passwd->setIcon(icon1);

        horizontalLayout->addWidget(tbtn_Passwd);


        gridLayout_2->addLayout(horizontalLayout, 0, 0, 1, 1);

        tableWidget = new QTableWidget(CSoftUpgredeWidget);
        if (tableWidget->columnCount() < 7)
            tableWidget->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        tableWidget->setAlternatingRowColors(true);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->verticalHeader()->setVisible(false);

        gridLayout_2->addWidget(tableWidget, 1, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        tbtn_add = new QToolButton(CSoftUpgredeWidget);
        tbtn_add->setObjectName(QStringLiteral("tbtn_add"));
        tbtn_add->setIcon(icon);

        horizontalLayout_2->addWidget(tbtn_add);

        tbtn_delete = new QToolButton(CSoftUpgredeWidget);
        tbtn_delete->setObjectName(QStringLiteral("tbtn_delete"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Resources/minus.png"), QSize(), QIcon::Normal, QIcon::Off);
        tbtn_delete->setIcon(icon2);

        horizontalLayout_2->addWidget(tbtn_delete);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        btn_Init = new QPushButton(CSoftUpgredeWidget);
        btn_Init->setObjectName(QStringLiteral("btn_Init"));

        horizontalLayout_2->addWidget(btn_Init);

        btn_SaveData = new QPushButton(CSoftUpgredeWidget);
        btn_SaveData->setObjectName(QStringLiteral("btn_SaveData"));

        horizontalLayout_2->addWidget(btn_SaveData);

        btn_Save = new QPushButton(CSoftUpgredeWidget);
        btn_Save->setObjectName(QStringLiteral("btn_Save"));

        horizontalLayout_2->addWidget(btn_Save);


        gridLayout_2->addLayout(horizontalLayout_2, 2, 0, 1, 1);


        retranslateUi(CSoftUpgredeWidget);

        QMetaObject::connectSlotsByName(CSoftUpgredeWidget);
    } // setupUi

    void retranslateUi(QWidget *CSoftUpgredeWidget)
    {
        CSoftUpgredeWidget->setWindowTitle(QApplication::translate("CSoftUpgredeWidget", "CSoftUpgredeWidget", 0));
        label->setText(QApplication::translate("CSoftUpgredeWidget", "\345\223\201\347\211\214:", 0));
        comboBox_brand->clear();
        comboBox_brand->insertItems(0, QStringList()
         << QApplication::translate("CSoftUpgredeWidget", "\351\200\211\346\213\251\345\223\201\347\211\214", 0)
        );
        label_2->setText(QApplication::translate("CSoftUpgredeWidget", "\346\267\273\345\212\240\345\255\220\345\223\201\347\211\214\357\274\232", 0));
        lineEdit_brand->setPlaceholderText(QApplication::translate("CSoftUpgredeWidget", "\350\276\223\345\205\245\345\223\201\347\211\214\345\220\215\347\247\260", 0));
#ifndef QT_NO_TOOLTIP
        tbtn_addbrand->setToolTip(QApplication::translate("CSoftUpgredeWidget", "\346\267\273\345\212\240\345\223\201\347\211\214", 0));
#endif // QT_NO_TOOLTIP
        tbtn_addbrand->setText(QString());
#ifndef QT_NO_TOOLTIP
        tbtn_Passwd->setToolTip(QApplication::translate("CSoftUpgredeWidget", "\345\223\201\347\211\214\345\257\206\351\222\245\351\205\215\347\275\256", 0));
#endif // QT_NO_TOOLTIP
        tbtn_Passwd->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("CSoftUpgredeWidget", "\350\275\257\344\273\266\345\220\215\347\247\260", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("CSoftUpgredeWidget", "\347\211\210\346\234\254\345\217\267", 0));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("CSoftUpgredeWidget", "Url", 0));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("CSoftUpgredeWidget", "\350\275\257\344\273\266\345\223\201\347\211\214", 0));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("CSoftUpgredeWidget", "\350\275\257\344\273\266\347\261\273\345\210\253", 0));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("CSoftUpgredeWidget", "\345\255\230\345\202\250\344\275\215\347\275\256", 0));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("CSoftUpgredeWidget", "\344\270\273\347\250\213\345\272\217", 0));
#ifndef QT_NO_TOOLTIP
        tbtn_add->setToolTip(QApplication::translate("CSoftUpgredeWidget", "\346\267\273\345\212\240\350\275\257\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        tbtn_add->setText(QString());
#ifndef QT_NO_TOOLTIP
        tbtn_delete->setToolTip(QApplication::translate("CSoftUpgredeWidget", "\345\210\240\351\231\244", 0));
#endif // QT_NO_TOOLTIP
        tbtn_delete->setText(QString());
        btn_Init->setText(QApplication::translate("CSoftUpgredeWidget", "\350\277\230\345\216\237", 0));
        btn_SaveData->setText(QApplication::translate("CSoftUpgredeWidget", "\344\277\235\345\255\230\351\205\215\347\275\256", 0));
        btn_Save->setText(QApplication::translate("CSoftUpgredeWidget", "\347\224\237\346\210\220 XML", 0));
    } // retranslateUi

};

namespace Ui {
    class CSoftUpgredeWidget: public Ui_CSoftUpgredeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSOFTUPGREDEWIDGET_H
