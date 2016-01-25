/********************************************************************************
** Form generated from reading UI file 'CPasswdConfig.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CPASSWDCONFIG_H
#define UI_CPASSWDCONFIG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_CConfPasswd
{
public:
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;
    QLabel *label;

    void setupUi(QDialog *CConfPasswd)
    {
        if (CConfPasswd->objectName().isEmpty())
            CConfPasswd->setObjectName(QStringLiteral("CConfPasswd"));
        CConfPasswd->resize(400, 300);
        gridLayout = new QGridLayout(CConfPasswd);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tableWidget = new QTableWidget(CConfPasswd);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->horizontalHeader()->setStretchLastSection(true);

        gridLayout->addWidget(tableWidget, 1, 0, 1, 1);

        label = new QLabel(CConfPasswd);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);


        retranslateUi(CConfPasswd);

        QMetaObject::connectSlotsByName(CConfPasswd);
    } // setupUi

    void retranslateUi(QDialog *CConfPasswd)
    {
        CConfPasswd->setWindowTitle(QApplication::translate("CConfPasswd", "CConfPasswd", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("CConfPasswd", "\345\223\201\347\211\214\345\220\215\347\247\260", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("CConfPasswd", "\345\257\206\347\240\201", 0));
        label->setText(QApplication::translate("CConfPasswd", "\350\257\267\350\276\223\345\205\245\351\234\200\350\246\201\351\205\215\347\275\256\347\232\204\345\223\201\347\211\214\347\232\204\345\257\206\347\240\201:", 0));
    } // retranslateUi

};

namespace Ui {
    class CConfPasswd: public Ui_CConfPasswd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CPASSWDCONFIG_H
