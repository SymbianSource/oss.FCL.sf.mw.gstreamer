
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/gstelement.h>
#include <string.h>
#define LOG_FILE "c:\\logs\\wav_record_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{

    if(result)
    {
        assert_failed = 1;
    } 

    testResultXml(xmlfile);
    close_log_file();

    if(result)
    {
        exit (-1);
    }    

}

GstElement *pipeline, *source, *wavenc,*sink;
GstBus *bus;
GMainLoop *loop;
gint error;
static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        gst_element_set_state (pipeline, GST_STATE_NULL);
        g_main_loop_quit(loop);
        gst_object_unref (GST_OBJECT (pipeline));
        //std_log(LOG_FILENAME_LINE, "Test Successful");
        create_xml(error); 
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug;
      GError *err;
      gst_message_parse_error (msg, &err, &debug);
      g_free (debug);
      g_print ("Error: %s\n", err->message);
      error = err->code;
      g_error_free (err);
      //std_log(LOG_FILENAME_LINE, "Test Failed");

      break;
    }
    default:
      break;
  }

  return TRUE;
}

static gboolean
quit_loop (gpointer data)
{
    gst_element_send_event (pipeline, gst_event_new_eos ());
    
}


/// exe name record_duration sampling_rate chans width
int main (int argc, char *argv[])
{
    char filename[1024];
    GstCaps* caps;
    int sampling_rate = 8000;
    int chans = 1; 
    int width = 16;
    guint record_duration = 10000;
    
    error = 0;
    if (argc > 1) {
    record_duration = (guint) atoi( argv[1] ) * 1000;
    }

    if (argc > 2) {
    sampling_rate = (int) atoi( argv[2] );
    }
    
    if (argc > 3) {
    chans = (int) atoi( argv[3] );
    }
    
    if (argc > 4) {
    width = (int) atoi( argv[4] );
    }
    
    
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);
    
    /* create elements */
    pipeline = gst_pipeline_new ("audio-player");
    source = gst_element_factory_make ("devsoundsrc", "audio-source");
    wavenc = gst_element_factory_make ("wavenc", "mp3parse");
    sink = gst_element_factory_make ("filesink", "sink");    
    
    if (!pipeline || !source || !wavenc || !sink) {
        g_print ("One element could not be created\n");
        return -1;
        }
    snprintf(filename, 1024, "C:\\data\\wav_record_samp-%d-chans-%d-duration-%dsec.wav",
            sampling_rate, chans,record_duration/1000 );
    
    /* set filename property on the file source. Also add a message  handler. */
    g_object_set (G_OBJECT (sink), "location", filename, NULL);
            /* put all elements in a bin */
    gst_bin_add_many (GST_BIN (pipeline),source, wavenc,sink, NULL);

    caps = gst_caps_new_simple ("audio/x-raw-int",
              "width", G_TYPE_INT, width,
              "depth", G_TYPE_INT, width,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT, sampling_rate,
              "channels", G_TYPE_INT, chans, NULL);  
    
    gst_element_link_filtered (source, wavenc, caps);
    gst_element_link (wavenc, sink);

    gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (pipeline)), bus_call, loop);
    //g_signal_connect (decoder, "pad-added", G_CALLBACK (new_pad_cb),pipeline);
            /* Now set to playing and iterate. */
    g_print ("Setting to PLAYING\n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_timeout_add (record_duration, quit_loop, loop);
    g_print ("Running\n");
    g_main_loop_run (loop);
            /* clean up nicely */
    g_print ("Returned, stopping playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);
    g_print ("Deleting pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));
    
    g_print ("completed palying audio\n");
    return 0;
}

