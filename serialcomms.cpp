#include "serialcomms.h"

SerialComms::SerialComms(QObject *parent)
    : QObject{parent}
{
    serial_port = "/dev/cu.usbserial-1430";
    port_info = QSerialPortInfo::availablePorts();
    qDebug() << "SerialComms::SerialComms() entered";
    QList<QSerialPortInfo>::iterator i;
    for (i = port_info.begin(); i != port_info.end(); ++i) {
        QString s_tmp = i->portName();
        if ( s_tmp.startsWith("cu.usbserial") )
            qDebug() << i->portName();
    }
}

QList<QSerialPortInfo> &SerialComms::get_pList() {
    return port_info;
}


