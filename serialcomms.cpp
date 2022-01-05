#include "serialcomms.h"

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
}

QList<QSerialPortInfo> &SerialComms::get_pList() {
    return port_info_list;
}

void SerialComms::enumerate_serial_devices() {

    qDebug() << "SerialComms::enumerate_serial_devices() entered";
    QList<QSerialPortInfo>::iterator i;
    for (i = port_info_list.begin(); i != port_info_list.end(); ++i) {
        QString s_tmp = i->portName();
        if ( s_tmp.startsWith("cu.usbserial") ) {
            qDebug() << i->portName();
            emit on_serial_port_detected(s_tmp);


            qDebug() << "SerialComms::enumerate_serial_devices(): config_serial_str =" << config_serial_str << "s_tmp =" << s_tmp;
            if ( s_tmp.compare(config_serial_str) == 0 ) {        // This is our configured value
               selected_serial_port_from_config = *i;
               qDebug() << "SerialComms::SerialComms() Constructor: selected_serial_port_from_config = " << selected_serial_port_from_config.description();
               break;
           }



        }
    }
}

int SerialComms::openSerialPort() {

    // Qt Default values: No Parity, 8-Bit data, 1 Stop bit
    active_serial_port_p = new QSerialPort;
    active_serial_port_p->setPort(selected_serial_port_from_config);
    qDebug() << "SerialComms::openSerialPort(): selected serial port is " << selected_serial_port_from_config.description();
    active_serial_port_p->setBaudRate(QSerialPort::Baud38400);
    bool retcode = active_serial_port_p->open(QIODeviceBase::ReadWrite);
    qDebug() << "SerialComms::openSerialPort(): serial port open() returned " << retcode;
    int h = active_serial_port_p->handle();
    qDebug() << "SerialComms::openSerialPort(): open - handle - " << h;
    return 0;
}

void SerialComms::setConfigSerialStr(QString s) {
    qDebug() << "SerialComms::setConfigSerialStr: s = " << s;
    config_serial_str = s;
}
