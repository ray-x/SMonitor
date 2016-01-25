/********************************************************************************
** Form generated from reading UI file 'CDataImport.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CDATAIMPORT_H
#define UI_CDATAIMPORT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CDataImportClass
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QToolButton *toolButton;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_db;
    QToolButton *toolButton_db;
    QLabel *label;
    QGroupBox *groupBox_im;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton;
    QProgressBar *progressBar;

    void setupUi(QWidget *CDataImportClass)
    {
        if (CDataImportClass->objectName().isEmpty())
            CDataImportClass->setObjectName(QStringLiteral("CDataImportClass"));
        CDataImportClass->resize(359, 189);
        QIcon icon;
        icon.addFile(QStringLiteral(":/CDataImport/Resources/import.png"), QSize(), QIcon::Normal, QIcon::Off);
        CDataImportClass->setWindowIcon(icon);
        gridLayout = new QGridLayout(CDataImportClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        buttonBox = new QDialogButtonBox(CDataImportClass);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 1);

        groupBox = new QGroupBox(CDataImportClass);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(3);
        gridLayout_3->setVerticalSpacing(9);
        gridLayout_3->setContentsMargins(9, 9, 9, 9);
        toolButton = new QToolButton(groupBox);
        toolButton->setObjectName(QStringLiteral("toolButton"));

        gridLayout_3->addWidget(toolButton, 0, 2, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 1, 0, 1, 1);

        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setReadOnly(true);

        gridLayout_3->addWidget(lineEdit, 0, 1, 1, 1);

        lineEdit_db = new QLineEdit(groupBox);
        lineEdit_db->setObjectName(QStringLiteral("lineEdit_db"));
        lineEdit_db->setReadOnly(true);

        gridLayout_3->addWidget(lineEdit_db, 1, 1, 1, 1);

        toolButton_db = new QToolButton(groupBox);
        toolButton_db->setObjectName(QStringLiteral("toolButton_db"));

        gridLayout_3->addWidget(toolButton_db, 1, 2, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        toolButton->raise();
        lineEdit->raise();
        lineEdit_db->raise();
        toolButton_db->raise();
        label->raise();
        label_2->raise();

        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_im = new QGroupBox(CDataImportClass);
        groupBox_im->setObjectName(QStringLiteral("groupBox_im"));
        groupBox_im->setEnabled(false);
        gridLayout_2 = new QGridLayout(groupBox_im);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        pushButton = new QPushButton(groupBox_im);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout_2->addWidget(pushButton, 0, 1, 1, 1);

        progressBar = new QProgressBar(groupBox_im);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setMinimumSize(QSize(0, 0));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        progressBar->setOrientation(Qt::Horizontal);
        progressBar->setInvertedAppearance(false);
        progressBar->setTextDirection(QProgressBar::TopToBottom);

        gridLayout_2->addWidget(progressBar, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox_im, 1, 0, 1, 1);


        retranslateUi(CDataImportClass);

        QMetaObject::connectSlotsByName(CDataImportClass);
    } // setupUi

    void retranslateUi(QWidget *CDataImportClass)
    {
        CDataImportClass->setWindowTitle(QApplication::translate("CDataImportClass", "\346\225\260\346\215\256\345\257\274\345\205\245", 0));
        groupBox->setTitle(QApplication::translate("CDataImportClass", "\351\200\211\346\213\251\346\225\260\346\215\256\346\272\220", 0));
#ifndef QT_NO_TOOLTIP
        toolButton->setToolTip(QApplication::translate("CDataImportClass", "\351\200\211\346\213\251XML\346\225\260\346\215\256\346\226\207\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        toolButton->setText(QApplication::translate("CDataImportClass", "...", 0));
        label_2->setText(QApplication::translate("CDataImportClass", "\346\225\260\346\215\256\345\272\223\350\267\257\345\276\204\357\274\232", 0));
        lineEdit->setPlaceholderText(QApplication::translate("CDataImportClass", "XML\346\226\207\344\273\266\350\267\257\345\276\204", 0));
        lineEdit_db->setPlaceholderText(QApplication::translate("CDataImportClass", "\346\225\260\346\215\256\345\272\223\346\226\207\344\273\266\350\267\257\345\276\204", 0));
#ifndef QT_NO_TOOLTIP
        toolButton_db->setToolTip(QApplication::translate("CDataImportClass", "\351\200\211\346\213\251\346\225\260\346\215\256\345\272\223\346\226\207\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        toolButton_db->setText(QApplication::translate("CDataImportClass", "...", 0));
        label->setText(QApplication::translate("CDataImportClass", "\346\272\220\346\225\260\346\215\256\350\267\257\345\276\204\357\274\232", 0));
        groupBox_im->setTitle(QApplication::translate("CDataImportClass", "\345\257\274\345\205\245\346\225\260\346\215\256", 0));
        pushButton->setText(QApplication::translate("CDataImportClass", "\345\257\274\345\205\245", 0));
    } // retranslateUi

};

namespace Ui {
    class CDataImportClass: public Ui_CDataImportClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CDATAIMPORT_H
