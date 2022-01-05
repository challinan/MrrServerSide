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
    ~SerialComms();
    QList<QSerialPortInfo> &get_pList();
    void enumerate_serial_devices();
    void setConfigSerialStr(QString s);
    int openSerialPort();

private:
    void close_serial_port();


private:
    QString config_serial_str;
    QSerialPortInfo selected_serial_port_from_config;
    QList<QSerialPortInfo> port_info_list;
    QSerialPort *active_serial_port_p;


signals:
    void on_serial_port_detected(QString &s);

};

#endif // SERIALCOMMS_H
