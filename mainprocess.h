#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include <QAudioDevice>
#include <QMediaDevices>
#include <QCoreApplication>
#include <QProcess>
#include "serialcomms.h"
#include "networkcomms.h"


// QT_BEGIN_NAMESPACE
// namespace Ui { class MainWindow; }
// QT_END_NAMESPACE

class MainProcess : public QCoreApplication
{
    Q_OBJECT

public:
    MainProcess(QCoreApplication *mainapp = nullptr, int argc = 0, char**argv = nullptr);
    ~MainProcess();

private:
    void startServices();

private slots:

private:
    SerialComms *serial_comms_p;
    NetworkComms *network_comms_p;
    QMediaDevices *media_dev_p;
    QList<QAudioDevice> audio_inputs;
    QString s_tmp;
    int serial_comboxbox_index;

public slots:
    void serial_port_detected(QString &s);
};
#endif // MAINPROCESS_H
