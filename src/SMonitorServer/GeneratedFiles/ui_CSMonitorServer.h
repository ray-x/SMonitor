/********************************************************************************
** Form generated from reading UI file 'CSMonitorServer.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSMONITORSERVER_H
#define UI_CSMONITORSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CSMonitorServerClass
{
public:

    void setupUi(QWidget *CSMonitorServerClass)
    {
        if (CSMonitorServerClass->objectName().isEmpty())
            CSMonitorServerClass->setObjectName(QStringLiteral("CSMonitorServerClass"));
        CSMonitorServerClass->resize(600, 400);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/normal.png"), QSize(), QIcon::Normal, QIcon::Off);
        CSMonitorServerClass->setWindowIcon(icon);

        retranslateUi(CSMonitorServerClass);

        QMetaObject::connectSlotsByName(CSMonitorServerClass);
    } // setupUi

    void retranslateUi(QWidget *CSMonitorServerClass)
    {
        CSMonitorServerClass->setWindowTitle(QApplication::translate("CSMonitorServerClass", "CSMonitorServer", 0));
    } // retranslateUi

};

namespace Ui {
    class CSMonitorServerClass: public Ui_CSMonitorServerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSMONITORSERVER_H
