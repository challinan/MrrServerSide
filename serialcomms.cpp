#include "serialcomms.h"
#include "networkcomms.h"

SerialComms::SerialComms(QObject *parent)
    : QObject{parent}
{
    // Enumerate available usb serial ports
    port_info_list = QSerialPortInfo::availablePorts();
    qDebug() << Qt::endl << "SerialComms::SerialComms() entered";

}

SerialComms::~SerialComms() {
    close_serial_port();
}

void SerialComms::close_serial_port() {
    qDebug() << "SerialComms::close_serial_port(): closing serial port";
    active_serial_port_p->close();
    // Disconnect signals ???
}

QList<QSerialPortInfo> &SerialComms::get_pList() {
    return port_info_list;
}

void SerialComms::setNetcommObjPointer(NetworkComms *pNetworkObj) {
    network_comm_obj_p = pNetworkObj;
}

void SerialComms::enumerate_serial_devices() {

    qDebug() << "SerialComms::enumerate_serial_devices() entered";
    QList<QSerialPortInfo>::iterator i;
    for (i = port_info_list.begin(); i != port_info_list.end(); i++) {
        QString s_tmp = i->portName();
        if ( s_tmp.startsWith("cu.usbserial") ) {
            emit on_serial_port_detected(s_tmp);
            if ( s_tmp.compare(config_serial_str) == 0 ) {        // This is our configured value
                selected_serial_port_from_config = *i;
            }
        }
    }
}

int SerialComms::openSerialPort() {

    // Qt Default values: No Parity, 8-Bit data, 1 Stop bit
    active_serial_port_p = new QSerialPort;
    active_serial_port_p->setPort(selected_serial_port_from_config);
    active_serial_port_p->setBaudRate(QSerialPort::Baud38400);
    qDebug() << "SerialComms::openSerialPort(): selected serial port is " << selected_serial_port_from_config.portName();
    bool retcode = active_serial_port_p->open(QIODeviceBase::ReadWrite);
    if ( !retcode ) {
        qDebug() << "SerialComms::openSerialPort(): serial port open() failed with"
                 << retcode << active_serial_port_p->errorString();
        return -1;
    }
    clear_serial_port_inbuffer();
    connect(active_serial_port_p, &QIODevice::readyRead, this, &SerialComms::slot_readyRead);
    connect(this, &SerialComms::serial_rx, network_comm_obj_p, &NetworkComms::serial_data_in_for_net_out);

    // read_and_write_data();  // For debug only
    return 0;
}

#if 0
// For debug only
void SerialComms::read_and_write_data() {

    int rc;
    char cbuffer[8];
    rc = active_serial_port_p->write("FA;", 3);
    qDebug() << "SerialComms::read_and_write_data(): write returned" << rc;

    int loops = 0;
    bool b;
    while ( (b = active_serial_port_p->waitForReadyRead(5000)) && loops < 5 ) {
        // qDebug() << "SerialComms::read_and_write_data(): waitForReadReady returned" << b;
        if ( b == false ) {
            loops++;
            continue;
        }
        while ( (rc = active_serial_port_p->read(cbuffer, 8)) > 0) {
            // qDebug() << "SerialComms::read_and_write_data(): read returned" << rc;
            for ( int i=0; i<rc; i++ ) {
                qDebug() << "Data[" << i << "] =" << cbuffer[i];
            }
        }
    }
}
#endif

void SerialComms::setConfigSerialStr(QString s) {
    config_serial_str = s;
}

int SerialComms::write_serial_data() {
    int rc;
    rc = active_serial_port_p->write(write_buffer);
    if ( rc == -1 ) {
        qDebug() << "SerialComms::write_serial_data(): write error:" << active_serial_port_p->errorString();
        return rc;
    }
    return rc;
}

// Slot - called by signal serial_out() in networkcomms to indicate serial data ready to send out
void SerialComms::net_data_2_serial_out(QByteArray &b) {

#if 0
    qDebug() << "SerialComms::net_data_2_serial_out(): entered - message size" << b.size();
    fprintf(stderr, "\n");
    for ( int i=0; i< b.size(); i++ ) {
        fprintf(stderr, "%c", b.at(i));
    }
    fprintf(stderr, "\n");
#endif

    write_buffer.clear();
    write_buffer = b;
    write_serial_data();
}

// Slot
void SerialComms::slot_readyRead() {

    read_serial_data();
}

int SerialComms::read_serial_data() {

    int rc = -2;
    char b;
    for ( int i = 0; i < 10; i++) {
        if ( active_serial_port_p->QSerialPort::bytesAvailable() )
            while ( (rc = active_serial_port_p->read(&b, 1)) > 0) {
                // qDebug() << "SerialComms::read_serial_data(): read returned" << rc;
                read_buffer.append(b);
                if ( b == ';' )  {
                    goto rsd_finish;
                }
            }
        QThread::msleep(5); // Wait a tiny bit for more data
    }

rsd_finish:
    // qDebug() << "SerialComms::read_serial_data(): read" << read_buffer.size() << "bytes";
    // for ( int i=0; i<read_buffer.size(); i++ ) {
        // fprintf(stderr, "%c", read_buffer.at(i));
    // }
    // fprintf(stderr, "\n");
    emit serial_rx(read_buffer);        // Alert Network stack we have data ready to send out the Socket
    read_buffer.clear();

    return rc;
}

void SerialComms::clear_serial_port_inbuffer() {
    int rc = 0;
    char b[64];
    int count = 0;
    qDebug() << "SerialComms::clear_serial_port_inbuffer(): entered - waiting";

    // This function blocks until readReady - default time out 30 seconds, let's try 1 seconds
    if ( active_serial_port_p->waitForReadyRead(1000) == false ) { return; }
    while ( count < 500 ) {
        int i = 0;
        if ( active_serial_port_p->QSerialPort::bytesAvailable() )
            rc += active_serial_port_p->read(&b[i++], 1);
        count++;
        QThread::msleep(2); // Wait a tiny bit for more data
    }
    qDebug() << "SerialComms::clear_serial_port_inbuffer(): read" << rc << "bytes over" << count << "loops" << b;
}
