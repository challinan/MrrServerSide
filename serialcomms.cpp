#include "serialcomms.h"

SerialComms::SerialComms(QObject *parent)
    : QObject{parent}
{
    serial_port = "/dev/cu.usbserial-1430";
    port_info = QSerialPortInfo::availablePorts();
    qDebug() << "SerialComms::SerialComms() entered";
    QList<QSerialPortInfo> list;
    QList<QSerialPortInfo>::iterator i;
    for (i = list.begin(); i != list.end(); ++i) {
        qDebug() << i->vendorIdentifier() << Qt::endl;
        qDebug() << i->manufacturer();
        qDebug() << i->portName();
        qDebug() << i->serialNumber();
    }
}

