#include "mainwindow.h"
#include "ui_mainwindow.h"
// For debug only

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "Command line args:" << QApplication::arguments();
    // Initialize UI
    ui->setupUi(this);
    initializeUiLabels();

    // Initialize configuration subsystem
    config_p = new ConfigObject;
    config_p->debug_display_map();  // Debug only

    // Initialize serial port object
    serialComms_p = new SerialComms();
    connect(serialComms_p, &SerialComms::on_serial_port_detected, this, &MainWindow::serial_port_detected);

    // Place our requested serial port config value in combox box (comes from our config file)
    QString config_serial_str = config_p->get_value_from_key("Serial Port");
    serialComms_p->setConfigSerialStr(config_serial_str);
    serialComms_p->enumerate_serial_devices();
    qDebug() << "MainWindow::MainWindow() Constructor: config_serial_str =" << config_serial_str;
    if ( !config_serial_str.isEmpty() ) {
        for ( int index=0; index < ui->serialPortComboBox->count(); index++ ) {
            qDebug() << config_serial_str << " == " << ui->serialPortComboBox->currentText() << ( config_serial_str == ui->serialPortComboBox->currentText() );
            if ( config_serial_str == ui->serialPortComboBox->itemText(index) ) {
                ui->serialPortComboBox->setCurrentIndex(index);
                break;
            }
        }
    }

    // Discover list of audio devices *******************
    // Place our config value in combox box at index[0]
    // Note ***** use isDefault() to see if a given device is set as the MAC OS X default
    s_tmp = QString("Audio Port");
    QString config_audio_str = config_p->get_value_from_key(s_tmp);
    int index=0;
    if ( !config_audio_str.isEmpty() )
        ui->audioPortComboBox->insertItem(index++, config_audio_str);

    audio_inputs = QMediaDevices::audioInputs();
    QList<QAudioDevice>::iterator a;
    for ( a = audio_inputs.begin(); a < audio_inputs.end(); ++a ) {
        QString d_tmp = a->description().trimmed();
        if ( d_tmp != config_audio_str ) {   // Skip our configured value
            ui->audioPortComboBox->insertItem(index++, a->description());
        }
    }

    // Setup default rig name
    QString config_rig_str = config_p->get_value_from_key(QString("Rig Model"));
    ui->rigComboBox->insertItem(0, config_rig_str);

    // Misc variable initialization
    serial_comboxbox_index = 0;
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

void MainWindow::initializeUiLabels() {
    QFont myFont( "Arial", 18, QFont::Bold);
    ui->appLabel->setFont(myFont);
    ui->appLabel->setText("Mac Remote Rig - Server Side");
    this->setWindowTitle("Mac Remote Rig - Server Side");
    ui->serialPortLabel->setText("Select Serial Port");
    ui->rigPortLabel->setText("Select Rig");
    ui->audioPortLabel->setText("Select Audio Port");
    ui->run_pButton->setDefault(true);
    ui->run_pButton->setAutoDefault(true);
}

void MainWindow::runNetcatProcess() {

    // nc -6 -l 4532 </dev/cu.usbserial-1430 >/dev/cu.usbserial-1420
    QString serial_port_device = config_p->get_value_from_key("Serial Port");
    if ( serial_port_device.isEmpty() ) {
        qDebug() << "MainWindow::runNetcatProcess() failed to get config value - exiting";
        QApplication::quit();
    }
#if 0
    QString program = "/usr/bin/nc";
    QStringList arguments;
    arguments << "blue";

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
    int res = myProcess->exitCode();
#endif
}

void MainWindow::on_run_pButton_clicked()
{
    qDebug() << "MainWindow::on_run_pButton_clicked()";
    serialComms_p->openSerialPort();
}

void MainWindow::serial_port_detected(QString &s) {
    ui->serialPortComboBox->insertItem(serial_comboxbox_index++, s);
}

void MainWindow::on_serialPortComboBox_activated(int index)
{
    qDebug() << "MainWindow::on_serialPortComboBox_activated: " << index;
}

