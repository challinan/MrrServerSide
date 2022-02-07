#include "gstreamer_serverside.h"

GstreamerServerSide::GstreamerServerSide(QObject *parent)
    : QThread{parent}
{
    qDebug() << "GstreamerServerSide::GstreamerServerSide() constructor entered";
}

void GstreamerServerSide::run() {

    /* Initialize GStreamer */
    gst_init (&argc, &argv);  // Ugh - this is 'C' afterall

    /* Build the pipeline */
    // From notes:  This works as of Jan 16/22 with this gst-launch command on imac:
    // gst-launch-1.0 -v osxaudiosrc device=44 ! audioconvert ! audioresample ! audio/x-raw, rate=48000, channels=1 ! rtpL16pay  ! udpsink host=chris-mbp16 port=5000
    //

    pipeline = gst_parse_launch("osxaudiosrc device=44 ! audio/x-raw, rate=48000, channels=1 ! audioresample ! \
                                 audio/x-raw, rate=44100 ! rtpL16pay ! rtpjitterbuffer mode=synced ! udpsink host=chris-mbp16 port=5000", &gst_error);

    qDebug() << "gst_parse_launcher() returned" << gst_error;

    /* Start playing */
    qDebug() << "GstreamerServerSide::run() called - starting pipeline";
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus (pipeline);
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
      g_error ("An error occurred! Re-run with the GST_DEBUG=*:WARN environment variable set for more details.");
    }

    /* Free resources */
    gst_message_unref (msg);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
}
