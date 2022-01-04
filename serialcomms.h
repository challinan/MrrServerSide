#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialComms : public QObject
{
    Q_OBJECT
public:
    explicit SerialComms(QObject *parent = nullptr);
   QList<QSerialPortInfo> &get_pList();

private:
    QString serial_port;
    QList<QSerialPortInfo> port_info;

signals:

};

#endif // SERIALCOMMS_H
