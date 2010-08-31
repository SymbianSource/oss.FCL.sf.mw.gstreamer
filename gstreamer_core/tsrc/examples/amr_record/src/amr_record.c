
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/gstelement.h>
#include <string.h>
#define LOG_FILE "c:\\logs\\amr_record_logs.txt" 
#include "std_log_result.h" 
#include <gst/interfaces/gstspeechencoderconfig.h>
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define LOG_FILENAME_LEN 256
#define _DEBUG 1

static char log_filename[LOG_FILENAME_LEN];

static guint bitrate = 0; 
static guint record_duration = 10000;
static guint _enable_logs = 0;

static GstSpeechEncoderConfigIntfc* iface;


#define ENABLE_LOGS

#ifdef ENABLE_LOGS
#define RET_GST_ERR_STR(var, level, str) \
    if ( level == var )\
return str;

static inline const char* _gst_err_cat( GstDebugLevel level)
{

    RET_GST_ERR_STR(level,GST_LEVEL_NONE,"");
    RET_GST_ERR_STR(level,GST_LEVEL_ERROR,"E ");
    RET_GST_ERR_STR(level,GST_LEVEL_WARNING,"W ");
    RET_GST_ERR_STR(level,GST_LEVEL_INFO,"I ");
    RET_GST_ERR_STR(level,GST_LEVEL_DEBUG,"D ");
    RET_GST_ERR_STR(level,GST_LEVEL_LOG, "L ");
    RET_GST_ERR_STR(level,GST_LEVEL_FIXME, "F ");
    RET_GST_ERR_STR(level,GST_LEVEL_MEMDUMP, "M ");
    return "";
}

static FILE* log_fp = 0;

static void open_log_fp()
{
    if (!log_fp)
    {
        snprintf(log_filename, LOG_FILENAME_LEN, "C:\\logs\\testframework\\gst_amr_br%d.log", bitrate);

        log_fp = fopen(log_filename, "w");
        if (!log_fp)
            return;
    }
}


static void _gstLogFunction (GstDebugCategory *category,
        GstDebugLevel level,
        const gchar *file,
        const gchar *function,
        gint line,
        GObject *object,
        GstDebugMessage *message,
        gpointer data)
{
    open_log_fp();

    fprintf(log_fp, "%s : %s \n", _gst_err_cat(level), gst_debug_message_get(message));
    fflush(log_fp);

}
#endif // ENABLE_LOGS

static void parse_args(int argc, char** argv)
{

    gint cur = 1;
    while ( argv[cur] && cur < argc )
    {
        GST_WARNING("br:%d, record_duration:%d \n",bitrate,record_duration);
        if( !strcmp(argv[cur],"-br") ) bitrate = atoi(argv[cur+1]);
        else if( !strcmp(argv[cur],"-l") ) _enable_logs = atoi(argv[cur+1]);
        else if( !strcmp(argv[cur],"-d") ) record_duration = atoi(argv[cur+1]);

        cur+=2;
    }
}


GstElement *pipeline, *source, *amrmux,*sink;
GstBus *bus;
GMainLoop *loop;

static gboolean
bus_call (GstBus     *bus,
        GstMessage *msg,
        gpointer    data)
    {
#ifdef ENABLE_LOGS
    if(_enable_logs)
        {
        open_log_fp();
        fprintf(log_fp,"[msg] %s from %s\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME (msg));
        }
#endif

    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_EOS:
            gst_element_set_state (pipeline, GST_STATE_NULL);
            g_main_loop_quit(loop);
            gst_object_unref (GST_OBJECT (pipeline));
            break;
            
        case GST_MESSAGE_ERROR: {
        gchar *debug;
        GError *err;
        gst_message_parse_error (msg, &err, &debug);
        g_free (debug);
        g_print ("Error: %s\n", err->message);
        g_error_free (err);
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
    GST_WARNING("Sending EOS\n");
    gst_element_send_event (pipeline, gst_event_new_eos ());
    return TRUE;
}

int main (int argc, char *argv[])
{
    char filename[1024];
    GstCaps* caps;
#ifdef ENABLE_LOGS
    if ( _enable_logs )
    setenv("GST_DEBUG","2",1);
#endif // ENABLE_LOGS
    
    parse_args(argc,argv);
    
    gst_init (&argc, &argv);
#ifdef ENABLE_LOGS
    if ( _enable_logs )
    gst_debug_add_log_function( _gstLogFunction, 0);
#endif // ENABLE_LOGS
    
    loop = g_main_loop_new (NULL, FALSE);
 
    /* create elements */
    pipeline = gst_pipeline_new ("audio-player");
    source = gst_element_factory_make ("devsoundsrc", "audio-source");
    amrmux = gst_element_factory_make ("amrmux", "amrmux");
    sink = gst_element_factory_make ("filesink", "sink");    
    
    if (!pipeline || !source || !amrmux || !sink) {
        g_print ("One element could not be created\n");
        GST_ERROR("One element could not be created\n");
        return -1;
        }
    if(bitrate > 0)
        {
        snprintf(filename, 1024, "C:\\data\\amr_record_%d.amr", bitrate );
        }
    else
        {
        snprintf(filename, 1024, "C:\\data\\amr_record.amr" );
        }
    
    /* set filename property on the file source. Also add a message  handler. */
    g_object_set (G_OBJECT (sink), "location", filename, NULL);
            /* put all elements in a bin */
    gst_bin_add_many (GST_BIN (pipeline),source, amrmux,sink, NULL);

    caps = gst_caps_new_simple ("audio/amr",
              "width", G_TYPE_INT, 8,
              "depth", G_TYPE_INT, 8,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT, 8000,
              "channels", G_TYPE_INT, 1, NULL);  
    
    if(!gst_element_link_filtered (source, amrmux, caps))
        {
        GST_ERROR("Linking source, amrmux failed\n");
        return -1;
        }
    
    if(!gst_element_link (amrmux, sink))
        {
        GST_ERROR("Linking amrmux,sink failed\n");
        return -1;
        }

    gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (pipeline)), bus_call, loop);
    iface = GST_SPEECH_ENCODER_CONFIG_GET_IFACE(source);
    if(!iface)
        {
        GST_ERROR("Speech Encoder Interface NULL \n");
        return -1;
        }
    if (bitrate > 0)
        {
        GST_WARNING("Setting bitrate %d \n",bitrate);
        iface->SetBitrate(bitrate);
        }
    
            /* Now set to playing and iterate. */
    g_print ("Setting to PLAYING\n");
    GST_WARNING("Setting to Playing \n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    iface->GetBitrate(&bitrate);
    
    g_timeout_add (record_duration, quit_loop, loop);
    
    g_main_loop_run (loop);
            /* clean up nicely */
    GST_WARNING("Returned, stopping playback\n");
    
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));
    
#ifdef ENABLE_LOGS
    if ( _enable_logs )
    fclose(log_fp);
#endif // ENABLE_LOGS
    
    g_print ("completed playing audio\n");
    return 0;
}

