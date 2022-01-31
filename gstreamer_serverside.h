#ifndef GSTREAMERSERVERSIDE_H
#define GSTREAMERSERVERSIDE_H

#include <gst/gst.h>
#include <QThread>
#include <QDebug>

class GstreamerServerSide : public QThread
{
public:
    explicit GstreamerServerSide(QObject *parent = nullptr);
    virtual void run();

private:
    GstElement *pipeline;
    GError *gst_error = NULL;
    GstBus *bus;
    GstMessage *msg;
    int argc = 0;
    char **argv;

signals:

};

#endif // GSTREAMERSERVERSIDE_H
