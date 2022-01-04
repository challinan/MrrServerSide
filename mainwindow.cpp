#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Initialize UI
    ui->setupUi(this);
    QFont myFont( "Arial", 18, QFont::Bold);
    ui->appLabel->setFont(myFont);
    ui->appLabel->setText("Mac Remote Rig - Server Side");
    this->setWindowTitle("Mac Remote Rig - Server Side");
    ui->serialPortLabel->setText("Select Serial Port");
    ui->rigPortLabel->setText("Select Rig");
    ui->audioPortLabel->setText("Select Audio Port");

    // Initialize configuration subsystem
    config_p = new ConfigObject;

    // Discover list of audio devices
    audio_inputs = QMediaDevices::audioInputs();
    QList<QAudioDevice>::iterator a;
    int index=0;
    for ( a = audio_inputs.begin(); a < audio_inputs.end(); ++a, index++ ) {
        qDebug() << "Audio ports" << a->description();
        ui->audioPortComboBox->insertItem(index, a->description());
    }

    // Initialize serial port, etc.
    serialComms_p = new SerialComms();
    QList<QSerialPortInfo>::iterator i;
    QList<QSerialPortInfo> &p = serialComms_p->get_pList();
    index=0;
    for ( i = p.begin(); i < p.end(); ++i,index++ ) {
        QString s_tmp = i->portName();
        if ( s_tmp.startsWith("cu.usbserial") )
            ui->serialPortComboBox->insertItem(index, i->portName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_quit_pButton_clicked()
{
    QApplication::quit();
}


void MainWindow::on_config_pButton_clicked()
{
    config_p->open_config_dialog();
}

