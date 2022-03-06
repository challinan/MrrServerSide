#include "gstreamer_serverside.h"

GstreamerServerSide::GstreamerServerSide(QObject *parent)
    : QThread{parent}
{
    qDebug() << "GstreamerServerSide::GstreamerServerSide() constructor entered: &source =" << &source;
    pipeline = NULL;
    source = NULL;
    resample = NULL;
    payloader = NULL;
    jitterbuf = NULL;
    sink = NULL;
    src_caps = NULL;
    resample_caps = NULL;
    gst_error = NULL;
    argc = 0;
}

void GstreamerServerSide::run() {

    /* Initialize GStreamer */
    gst_init(&argc, &argv);  // Ugh - this is 'C' afterall

    /* Build the pipeline */
    // From notes:  This works as of Jan 16/22 with this gst-launch command on imac:
    // gst-launch-1.0 -v osxaudiosrc device=44 ! audioconvert ! audioresample ! audio/x-raw, rate=48000, channels=1 ! rtpL16pay  ! udpsink host=chris-mbp16 port=5000
    //

#if 0
    pipeline = gst_parse_launch("osxaudiosrc device=44 ! audio/x-raw, rate=48000, channels=1 ! audioresample ! \
                                 audio/x-raw, rate=44100 ! rtpL16pay ! rtpjitterbuffer mode=synced ! udpsink host=chris-mbp16 port=5000", &gst_error);

    qDebug() << "gst_parse_launcher() returned" << gst_error;
#endif

    pipeline = gst_pipeline_new("mrr_pipeline");
    setupPipeline();

    /* Start playing */
    qDebug() << "GstreamerServerSide::run() called - creating pipeline in paused state";
    gst_element_set_state(pipeline, GST_STATE_PAUSED);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);

    // This function blocks until EOS or an error occurs
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
      g_error("An error occurred! Re-run with the GST_DEBUG=*:WARN environment variable set for more details.");
    }

    /* Free resources */
    gst_message_unref(msg);
    gst_object_unref(bus);
    releaseElements();
}

const char *GstreamerServerSide::getStateString(GstState state) {
    static const char *state_str[] = {"GST_STATE_VOID_PENDING", "GST_STATE_NULL", "GST_STATE_READY", "GST_STATE_PAUSED", "GST_STATE_PLAYING"};
    return state_str[state];
}

const char *GstreamerServerSide::getResultString(GstStateChangeReturn ret) {
    static const char *result_str[] = {"GST_STATE_CHANGE_FAILURE", "GST_STATE_CHANGE_SUCCESS", "GST_STATE_CHANGE_ASYNC", "GST_STATE_CHANGE_NO_PREROLL"};
    return result_str[ret];
}

void GstreamerServerSide::startStopPlaying(bool start) {

    GstStateChangeReturn result;
    GstState state, pending;
#define TE pipeline

    result = gst_element_get_state(TE, &state, &pending, 2000000);     // Nanoseconds
    qDebug() << "GstreamerServerSide::startStopPlaying(): get_state reports: result =" << getResultString(result) << "state =" << getStateString(state) << "pending =" << getStateString(pending);

    if ( start ) {
        qDebug() << "GstreamerServerSide::startStopPlaying(): changing state to PLAYING";
        result = gst_element_set_state(TE, GST_STATE_PLAYING);
        if ( result != GST_STATE_CHANGE_SUCCESS ) {
            // qDebug() << "GstreamerServerSide::startStopPlaying(): error going to PLAYING state" << getResultString(result);
            result = gst_element_get_state(TE, &state, &pending, 2000000);     // Nanoseconds
            // qDebug() << "GstreamerServerSide::startStopPlaying(): gst_element_get_state, result =" << getResultString(result) << "state =" << getStateString(state) << "pending =" << getStateString(pending);
        }
    }
    else
    {   // Stop
        qDebug() << "GstreamerServerSide::startStopPlaying(): changing state to PAUSE";
        result = gst_element_set_state(TE, GST_STATE_PAUSED);
        if ( result != GST_STATE_CHANGE_SUCCESS ) {
            qDebug() << "GstreamerServerSide::startStopPlaying(): error going to PAUSED state" << getResultString(result);
            result = gst_element_get_state(TE, &state, &pending, 1000000);     // Nanoseconds
            qDebug() << "GstreamerServerSide::startStopPlaying(): gst_element_get_state, result =" << getResultString(result) << "state =" << getStateString(state) << "pending =" << getStateString(pending);
        }
    }
}

int GstreamerServerSide::factoryMakeAndAdd(GstElement **e, const char *element_name, const char *myname) {

    gboolean b_rc;

    g_print("GstreamerServerSide::factoryMakeAndAdd(): %s\n", element_name);
    *e = gst_element_factory_make(element_name, myname);
    if (!e) {
      g_print("Failed to create element of type %s\n", element_name);
      return -1;
    }

    b_rc = gst_bin_add(GST_BIN(pipeline), *e);
    if ( !b_rc ) {
        g_print("Failed to add element %s to pipeline\n", element_name);
        return -1;
    }

    return 0;
}

int GstreamerServerSide::setupPipeline() {

    gboolean link_ok;

#ifdef __APPLE__
    /* Create our elements */
    factoryMakeAndAdd(&source, "osxaudiosrc", "mysource");
    // Set the device property in our source
    g_object_set(source, "device", 44, NULL);
    g_print("G_IS_OBJECT: %d\n", G_IS_OBJECT(source));
#endif

    factoryMakeAndAdd(&resample, "audioresample", "myresampler");
    factoryMakeAndAdd(&payloader, "rtpL16pay", "mypayloader");
    factoryMakeAndAdd(&jitterbuf, "rtpjitterbuffer", "myjitterbuf");
    g_object_set(G_OBJECT(jitterbuf), "mode", 4, NULL);

    g_print("GstreamerServerSide::setupPipeline(): making element sink\n");
    factoryMakeAndAdd(&sink, "udpsink", "mysink");
    g_object_set(G_OBJECT(sink), "host", "chris-mbp16", NULL);
    g_object_set(G_OBJECT(sink), "port", 5000, NULL);

    g_print("GstreamerServerSide::setupPipeline(): all objects factory made\n");

    // "video/x-raw", "rate", G_TYPE_INT, 48000, "channels", G_TYPE_INT, 1, NULL);
    src_caps = gst_caps_new_simple("audio/x-raw", "rate", G_TYPE_INT, 48000, "channels", G_TYPE_INT, 1, NULL);

    g_print("GstreamerServerSide::setupPipeline(): About to link elements\n");

    link_ok = gst_element_link_filtered(source, resample, src_caps);
    if (!link_ok) {
      g_warning("Failed to link source and resample!");
    }
    gst_caps_unref(src_caps);

    resample_caps = gst_caps_new_simple("audio/x-raw", "rate", G_TYPE_INT, 44100, NULL);
    link_ok = gst_element_link_filtered(resample, payloader, resample_caps);
    if (!link_ok) {
      g_warning("Failed to link resample and payloader!");
    }
    gst_caps_unref(resample_caps);

    link_ok = gst_element_link(payloader, jitterbuf);
    if (!link_ok) {
      g_warning("Failed to link payloader and jitterbuf!");
    }

    link_ok = gst_element_link(jitterbuf, sink);
    if (!link_ok) {
      g_warning("Failed to link jitterbuf and sink!");
    }

    print_status_of_all();
    return link_ok;
}

void GstreamerServerSide::releaseElements() {

    // Pipeline itself will be removed by main()/run()
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(source));
    gst_object_unref(GST_OBJECT(resample));
    gst_object_unref(GST_OBJECT(payloader));
    gst_object_unref(GST_OBJECT(jitterbuf));
    gst_object_unref(GST_OBJECT(sink));
    gst_object_unref(GST_OBJECT(pipeline));

}

// From https://stackoverflow.com/questions/27547500/how-to-resume-playing-after-paused-using-gstreamer
void GstreamerServerSide::print_status_of_all()
{
    auto it  = gst_bin_iterate_elements(GST_BIN(pipeline));
    GValue item = G_VALUE_INIT;
    for(GstIteratorResult r = gst_iterator_next(it, &item); r != GST_ITERATOR_DONE; r = gst_iterator_next(it, &item))
    {
         if ( r == GST_ITERATOR_OK )
         {
             GstElement *e = static_cast<GstElement*>(g_value_peek_pointer(&item));
             GstState  current, pending;
             gst_element_get_state(e, &current, &pending, 100000);
             g_print("%s(%s), status = %s, pending = %s\n", G_VALUE_TYPE_NAME(&item), gst_element_get_name(e), \
                      gst_element_state_get_name(current), gst_element_state_get_name(pending));
         }
    }
}

