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
    const char *getStateString(GstState state);
    const char *getResultString(GstStateChangeReturn ret);
    int setupPipeline();
    void releaseElements();
    void print_status_of_all();
    int factoryMakeAndAdd(GstElement **e, const char *name, const char *myname);

private:
    //
    //osxaudiosrc device=44 ! audio/x-raw, rate=48000, channels=1 ! audioresample ! \
    // audio/x-raw, rate=44100 ! rtpL16pay ! rtpjitterbuffer mode=synced ! udpsink host=chris-mbp16 port=5000
    //
    GstElement *pipeline;
    GstElement *source;
    GstElement *resample;
    GstElement *payloader;
    GstElement *jitterbuf;
    GstElement *sink;

    GstCaps *src_caps;
    GstCaps *resample_caps;

    GError *gst_error;
    GstBus *bus;
    GstMessage *msg;
    int argc;
    char **argv;

public slots:
    void startStopPlaying(bool start);

signals:

};

#endif // GSTREAMERSERVERSIDE_H
