#include "./ui_mainwindow.h"
#include "Error in " Util.relativeFilePath('D:/code/Who' sTheMillionare / LTM_Group04 / mainwindow.h ', ' D : / code / Who 'sTheMillionare/LTM_Group04' + '/' + Util.path('mainwindow.cpp')) ": SyntaxError: Expected token `)'"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
