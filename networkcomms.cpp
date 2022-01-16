#include "networkcomms.h"
#include <QApplication>

// Implement a network listener to listen for data from the remote UI app
// Radio commands arrive by network and need to go out the serial port
// Any incoming serial data needs to be sent back over the network connection

NetworkComms::NetworkComms(QObject *parent)
    : QObject{parent}
{
    qDebug() << "NetworkComms::NetworkComms(): constructor entered";
    tcp_server_p = new QTcpServer;
    connect(tcp_server_p, &QTcpServer::newConnection, this, &NetworkComms::NewConnection);
    // connect(tcp_server_p, &QAbstractSocket::connected, this, &NetworkComms::connected);

    // Initialize internal variables
    read_bytes_qstr.clear();
    read_buffer.clear();
    tcp_socket_p = nullptr;
    serial_comms_object_p = nullptr;
}

NetworkComms::~NetworkComms() {
    delete tcp_server_p;
}

void NetworkComms::openNetworkListener() {

    // This function called when "Run" button pushed.
    qDebug() << "NetworkComms::openNetworkListner(): entered";
    connect(this, &NetworkComms::serial_out, serial_comms_object_p, &SerialComms::net_data_2_serial_out);
    bool rc;
    rc = tcp_server_p->listen(MRR_SERVER_ADDR, MRR_NETWORK_PORT);
    if ( !rc ) {
        qDebug() << "NetworkComms::openListner(): listen() failed" << tcp_server_p->errorString();
        QApplication::exit(2);
    }
    qDebug() << "NetworkComms::openNetworkListener(): returned" << rc;
}

// Slot: Called from QTcpServer emitting newConnection signal
void NetworkComms::NewConnection() {
    qDebug() << "NetworkComms::NewConnection(): slot entered";
    // Create a socket to represent this new connection
    tcp_socket_p = tcp_server_p->nextPendingConnection();
    qDebug() << "NetworkComms::NewConnection(): slot returned" << tcp_socket_p;
    connect(tcp_socket_p, &QIODevice::readyRead, this, &NetworkComms::readData);
    connect(tcp_socket_p, &QAbstractSocket::errorOccurred, this, &NetworkComms::errorOccurred);
    connect(tcp_socket_p, &QAbstractSocket::disconnected, this, &NetworkComms::disconnected);

}

// Slot: QAbstractSocket::connected()
void NetworkComms::connected() {
    qDebug() << "NetworkComms::connected(): ******************************** slot entered";
}

// Slot: QAbstractSocket::disconnected()
void NetworkComms::disconnected() {
    qDebug() << "NetworkComms::disconnected(): slot entered";
}

// Slot: called by signal readyRead from QIODevice (TCPSocket)
void NetworkComms::readData() {
    qint64 rc;
    char buff[MAX_READ_SIZE];
    // qDebug() << "NetworkComms::readData(): entered";
    rc = tcp_socket_p->read(buff, MAX_READ_SIZE);
    // Check for error conditions, etc
    qDebug() << "NetworkComms::readData(): read" << rc << "bytes";
    if ( rc == 0 ) {
        // Nothing to read
        return;
    } else {
        if ( rc < 0 ) {     // Handle error case
            // QAbstractSocket::SocketError e = tcp_socket_p->error();
            qDebug() << "NetworkComms::readData(): error reading data: " << tcp_socket_p->errorString();
            return;
        }
    }

    // We have valid data - send it to serial port!
    for (int i=0; i<rc; i++) {
        if ( buff[i] == 0x0a ) continue; // discard \n
        read_buffer.append(buff[i]);
    }
    emit serial_out(read_buffer);
    read_buffer.clear();
}

void NetworkComms::writeData(QByteArray ba) {
    // qDebug() << "NetworkComms::writeData(): entered";
    if ( tcp_socket_p == nullptr ) {
        qDebug() << "NetworkComms::writeData(): called before socket available - exiting!!!!!!";
        QApplication::exit(3);
    } else {
    int rc = tcp_socket_p->write(ba.data(), ba.size());
    qDebug() << "NetworkComms::writeData(): writing" << rc << "bytes";
    }
}

void NetworkComms::errorOccurred(QAbstractSocket::SocketError socketError) {
    // Signal generated by QAbstractSocket on any error
    qDebug() << "NetworkComms::errorOccurred(): slot entered" << socketError;
}

void NetworkComms::bytesWritten(qint64 bytes) {
    // Signal generated by QIODevice when bytes are written
    qDebug() << "NetworkComms::bytesWritten(): slot entered:" << bytes << "written";
}

void NetworkComms::readyRead() {
    qDebug() << "NetworkComms::readyRead(): slot entered";
}

void NetworkComms::setSerialObjectPointer(SerialComms *serial_comms_p) {
    serial_comms_object_p = serial_comms_p;
}

void NetworkComms::serial_data_in_for_net_out(QByteArray &p) {
    writeData(p);
}