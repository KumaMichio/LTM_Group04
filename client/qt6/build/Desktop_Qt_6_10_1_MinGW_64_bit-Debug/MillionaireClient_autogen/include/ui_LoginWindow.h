/********************************************************************************
** Form generated from reading UI file 'LoginWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *serverGroupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *hostLabel;
    QLineEdit *hostLineEdit;
    QLabel *portLabel;
    QSpinBox *portSpinBox;
    QPushButton *connectButton;
    QLabel *connectionStatusLabel;
    QTabWidget *tabWidget;
    QWidget *loginTab;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QLabel *loginUsernameLabel;
    QLineEdit *loginUsernameEdit;
    QLabel *loginPasswordLabel;
    QLineEdit *loginPasswordEdit;
    QPushButton *loginButton;
    QSpacerItem *verticalSpacer_2;
    QWidget *registerTab;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_3;
    QLabel *registerUsernameLabel;
    QLineEdit *registerUsernameEdit;
    QLabel *registerPasswordLabel;
    QLineEdit *registerPasswordEdit;
    QLabel *registerConfirmPasswordLabel;
    QLineEdit *registerConfirmPasswordEdit;
    QPushButton *registerButton;
    QSpacerItem *verticalSpacer_4;
    QPushButton *logoutButton;
    QLabel *statusLabel;

    void setupUi(QMainWindow *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->resize(500, 600);
        centralwidget = new QWidget(LoginWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        serverGroupBox = new QGroupBox(centralwidget);
        serverGroupBox->setObjectName("serverGroupBox");
        horizontalLayout = new QHBoxLayout(serverGroupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        hostLabel = new QLabel(serverGroupBox);
        hostLabel->setObjectName("hostLabel");

        horizontalLayout->addWidget(hostLabel);

        hostLineEdit = new QLineEdit(serverGroupBox);
        hostLineEdit->setObjectName("hostLineEdit");

        horizontalLayout->addWidget(hostLineEdit);

        portLabel = new QLabel(serverGroupBox);
        portLabel->setObjectName("portLabel");

        horizontalLayout->addWidget(portLabel);

        portSpinBox = new QSpinBox(serverGroupBox);
        portSpinBox->setObjectName("portSpinBox");
        portSpinBox->setMaximum(65535);

        horizontalLayout->addWidget(portSpinBox);

        connectButton = new QPushButton(serverGroupBox);
        connectButton->setObjectName("connectButton");

        horizontalLayout->addWidget(connectButton);


        verticalLayout->addWidget(serverGroupBox);

        connectionStatusLabel = new QLabel(centralwidget);
        connectionStatusLabel->setObjectName("connectionStatusLabel");

        verticalLayout->addWidget(connectionStatusLabel);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        loginTab = new QWidget();
        loginTab->setObjectName("loginTab");
        verticalLayout_2 = new QVBoxLayout(loginTab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        loginUsernameLabel = new QLabel(loginTab);
        loginUsernameLabel->setObjectName("loginUsernameLabel");

        verticalLayout_2->addWidget(loginUsernameLabel);

        loginUsernameEdit = new QLineEdit(loginTab);
        loginUsernameEdit->setObjectName("loginUsernameEdit");

        verticalLayout_2->addWidget(loginUsernameEdit);

        loginPasswordLabel = new QLabel(loginTab);
        loginPasswordLabel->setObjectName("loginPasswordLabel");

        verticalLayout_2->addWidget(loginPasswordLabel);

        loginPasswordEdit = new QLineEdit(loginTab);
        loginPasswordEdit->setObjectName("loginPasswordEdit");

        verticalLayout_2->addWidget(loginPasswordEdit);

        loginButton = new QPushButton(loginTab);
        loginButton->setObjectName("loginButton");

        verticalLayout_2->addWidget(loginButton);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        tabWidget->addTab(loginTab, QString());
        registerTab = new QWidget();
        registerTab->setObjectName("registerTab");
        verticalLayout_3 = new QVBoxLayout(registerTab);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_3);

        registerUsernameLabel = new QLabel(registerTab);
        registerUsernameLabel->setObjectName("registerUsernameLabel");

        verticalLayout_3->addWidget(registerUsernameLabel);

        registerUsernameEdit = new QLineEdit(registerTab);
        registerUsernameEdit->setObjectName("registerUsernameEdit");

        verticalLayout_3->addWidget(registerUsernameEdit);

        registerPasswordLabel = new QLabel(registerTab);
        registerPasswordLabel->setObjectName("registerPasswordLabel");

        verticalLayout_3->addWidget(registerPasswordLabel);

        registerPasswordEdit = new QLineEdit(registerTab);
        registerPasswordEdit->setObjectName("registerPasswordEdit");

        verticalLayout_3->addWidget(registerPasswordEdit);

        registerConfirmPasswordLabel = new QLabel(registerTab);
        registerConfirmPasswordLabel->setObjectName("registerConfirmPasswordLabel");

        verticalLayout_3->addWidget(registerConfirmPasswordLabel);

        registerConfirmPasswordEdit = new QLineEdit(registerTab);
        registerConfirmPasswordEdit->setObjectName("registerConfirmPasswordEdit");

        verticalLayout_3->addWidget(registerConfirmPasswordEdit);

        registerButton = new QPushButton(registerTab);
        registerButton->setObjectName("registerButton");

        verticalLayout_3->addWidget(registerButton);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_4);

        tabWidget->addTab(registerTab, QString());

        verticalLayout->addWidget(tabWidget);

        logoutButton = new QPushButton(centralwidget);
        logoutButton->setObjectName("logoutButton");
        logoutButton->setEnabled(false);

        verticalLayout->addWidget(logoutButton);

        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(statusLabel);

        LoginWindow->setCentralWidget(centralwidget);

        retranslateUi(LoginWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QMainWindow *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "\304\220\304\203ng nh\341\272\255p - Ai l\303\240 tri\341\273\207u ph\303\272", nullptr));
        serverGroupBox->setTitle(QCoreApplication::translate("LoginWindow", "C\303\240i \304\221\341\272\267t Server", nullptr));
        hostLabel->setText(QCoreApplication::translate("LoginWindow", "Host:", nullptr));
        portLabel->setText(QCoreApplication::translate("LoginWindow", "Port:", nullptr));
        connectButton->setText(QCoreApplication::translate("LoginWindow", "K\341\272\277t n\341\273\221i", nullptr));
        connectionStatusLabel->setText(QCoreApplication::translate("LoginWindow", "Tr\341\272\241ng th\303\241i: Ch\306\260a k\341\272\277t n\341\273\221i", nullptr));
        connectionStatusLabel->setStyleSheet(QCoreApplication::translate("LoginWindow", "color: red;", nullptr));
        loginUsernameLabel->setText(QCoreApplication::translate("LoginWindow", "T\303\252n \304\221\304\203ng nh\341\272\255p:", nullptr));
        loginPasswordLabel->setText(QCoreApplication::translate("LoginWindow", "M\341\272\255t kh\341\272\251u:", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginWindow", "\304\220\304\203ng nh\341\272\255p", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(loginTab), QCoreApplication::translate("LoginWindow", "\304\220\304\203ng nh\341\272\255p", nullptr));
        registerUsernameLabel->setText(QCoreApplication::translate("LoginWindow", "T\303\252n \304\221\304\203ng nh\341\272\255p:", nullptr));
        registerPasswordLabel->setText(QCoreApplication::translate("LoginWindow", "M\341\272\255t kh\341\272\251u:", nullptr));
        registerConfirmPasswordLabel->setText(QCoreApplication::translate("LoginWindow", "X\303\241c nh\341\272\255n m\341\272\255t kh\341\272\251u:", nullptr));
        registerButton->setText(QCoreApplication::translate("LoginWindow", "\304\220\304\203ng k\303\275", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(registerTab), QCoreApplication::translate("LoginWindow", "\304\220\304\203ng k\303\275", nullptr));
        logoutButton->setText(QCoreApplication::translate("LoginWindow", "\304\220\304\203ng xu\341\272\245t", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
