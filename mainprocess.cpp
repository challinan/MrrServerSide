#include "mainprocess.h"

MainProcess::MainProcess(QCoreApplication *mainapp, int argc, char**argv)
    : QCoreApplication(argc, argv)

{
    (void) mainapp;

    qDebug() << "Command line args:" << QCoreApplication::arguments();

    // Read configuration data from .macrr .ini file
    // (For now, we will initialize the variables by hand)

    // Initialize serial port object
    serial_comms_p = new SerialComms();
    connect(serial_comms_p, &SerialComms::on_serial_port_detected, this, &MainProcess::serial_port_detected);

#if 0

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

    // Initialize network TCP Listener for CAT communications
    network_comms_p = new NetworkComms();
    network_comms_p->setSerialObjectPointer(serial_comms_p);
    serial_comms_p->setNetcommObjPointer(network_comms_p);

    // Misc variable initialization
    serial_comboxbox_index = 0;
#endif

    // Start it running
    startServices();
}

MainProcess::~MainProcess()
{
    delete serial_comms_p;
    delete network_comms_p;
}

void MainProcess::startServices()
{
    static bool running = false;

    if ( running ) return;
    serial_comms_p->openSerialPort();
    // network_comms_p->openNetworkListener();
    // connect(network_comms_p, &NetworkComms::serial_out, serial_comms_p, &SerialComms::net_data_2_serial_out);


    running = true;
}

void MainProcess::serial_port_detected(QString &s) {

    qDebug() << "MainProcess::serial_port_detected()" << s;
}

