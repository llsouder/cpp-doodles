/********************************************************************************
** Form generated from reading UI file 'tims.ui'
**
** Created by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMS_H
#define UI_TIMS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_TimsGui
{
public:
    QPushButton *pttNot;

    void setupUi(QDialog *TimsGui)
    {
        if (TimsGui->objectName().isEmpty())
            TimsGui->setObjectName(QString::fromUtf8("TimsGui"));
        TimsGui->resize(400, 300);
        pttNot = new QPushButton(TimsGui);
        pttNot->setObjectName(QString::fromUtf8("pttNot"));
        pttNot->setGeometry(QRect(130, 100, 108, 25));

        retranslateUi(TimsGui);
        QObject::connect(pttNot, SIGNAL(clicked()), TimsGui, SLOT(shutUp()));

        QMetaObject::connectSlotsByName(TimsGui);
    } // setupUi

    void retranslateUi(QDialog *TimsGui)
    {
        TimsGui->setWindowTitle(QApplication::translate("TimsGui", "Dialog", 0, QApplication::UnicodeUTF8));
        pttNot->setText(QApplication::translate("TimsGui", "Shut Up", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TimsGui: public Ui_TimsGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMS_H
