/********************************************************************************
** Form generated from reading UI file 'basic_gui.ui'
**
** Created by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BASIC_GUI_H
#define UI_BASIC_GUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_TableDialog
{
public:
    QPushButton *pbEnterData;

    void setupUi(QDialog *TableDialog)
    {
        if (TableDialog->objectName().isEmpty())
            TableDialog->setObjectName(QString::fromUtf8("TableDialog"));
        TableDialog->resize(900, 281);
        pbEnterData = new QPushButton(TableDialog);
        pbEnterData->setObjectName(QString::fromUtf8("pbEnterData"));
        pbEnterData->setGeometry(QRect(390, 150, 108, 25));

        retranslateUi(TableDialog);
        QObject::connect(pbEnterData, SIGNAL(clicked()), TableDialog, SLOT(addNewData()));

        QMetaObject::connectSlotsByName(TableDialog);
    } // setupUi

    void retranslateUi(QDialog *TableDialog)
    {
        TableDialog->setWindowTitle(QApplication::translate("TableDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        pbEnterData->setText(QApplication::translate("TableDialog", "Enter Data", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TableDialog: public Ui_TableDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BASIC_GUI_H
