#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QProcess>
#include "serialcomms.h"
#include "config_object.h"
#include "networkcomms.h"
#include "gstreamer_serverside.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void startServices();

private slots:
    void on_quit_pButton_clicked();
    void on_config_pButton_clicked();
    void on_run_pButton_clicked();
    void on_serialPortComboBox_activated(int index);

    void on_netcat_pButton_clicked();

private:
    Ui::MainWindow *ui;
    SerialComms *serial_comms_p;
    NetworkComms *network_comms_p;
    ConfigObject *config_p;
    QMediaDevices *media_dev_p;
    QList<QAudioDevice> audio_inputs;
    QString s_tmp;
    void initializeUiLabels();
    void runNetcatProcess();
    int serial_comboxbox_index;
    GstreamerServerSide *GST_Server_p;

public slots:
    void serial_port_detected(QString &s);
};
#endif // MAINWINDOW_H
