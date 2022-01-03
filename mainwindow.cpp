#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFont myFont( "Arial", 18, QFont::Bold);
    ui->appLabel->setFont(myFont);
    ui->appLabel->setText("Mac Remote Rig - Server Side");
    serialComms_p = new SerialComms();
}

MainWindow::~MainWindow()
{
    delete ui;
}

