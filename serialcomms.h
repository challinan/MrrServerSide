#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QThread>   // For msleep()

#define MAX_TX_BUFFER_SIZE 64
#define MAX_RX_BUFFER_SIZE 64

class NetworkComms;

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
    int write_serial_data();
    int read_serial_data();
    void setNetcommObjPointer(NetworkComms *pNetworkObj);

public slots:
    void net_data_2_serial_out(QByteArray &b);
    void slot_readyRead();

private:
    void close_serial_port();
    void read_and_write_data();
    void clear_serial_port_inbuffer();

private:
    QString config_serial_str;
    QSerialPortInfo selected_serial_port_from_config;
    QList<QSerialPortInfo> port_info_list;
    QSerialPort *active_serial_port_p;
    // char write_buffer[MAX_TX_BUFFER_SIZE];
    // char read_buffer[MAX_RX_BUFFER_SIZE];
    QByteArray read_buffer;
    QByteArray write_buffer;
    NetworkComms *network_comm_obj_p;

signals:
    void on_serial_port_detected(QString &s);
    void serial_rx(QByteArray &p);

};

#endif // SERIALCOMMS_H
