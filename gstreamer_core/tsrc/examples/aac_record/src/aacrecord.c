
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/gstelement.h>
#include <string.h>
#define LOG_FILE "c:\\logs\\launch_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

static guint _bitrate = 128000;
static guint _channels = 1;
static guint _sample_rate = 8000;
static guint _aac_profile = 2; // default is LC
static guint _enable_logs = 1;
static guint _record_duration = 10000; // recording duration
#define REC_FILENAME_LEN 256
static char rec_filename[REC_FILENAME_LEN];

GstElement *pipeline;

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
static inline const char* _str_aac_profile()
{
    if ( _aac_profile == 0) return "auto";
    if ( _aac_profile == 2) return "lc";
    if ( _aac_profile == 5) return "he";
    return "unknown";
}

static FILE* log_fp = 0;

static void open_log_fp()
{
    if (!log_fp)
    {
        snprintf(rec_filename, REC_FILENAME_LEN, "C://Data//gst_br%d_c%d_sr%d_%s.log", _bitrate, _channels, _sample_rate, _str_aac_profile());

        log_fp = fopen(rec_filename, "w");
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

    // if (  (level != GST_LEVEL_ERROR) /*&& (level != GST_LEVEL_DEBUG)*/ && (level != GST_LEVEL_WARNING) ) 
    //     return;

    open_log_fp();

    fprintf(log_fp, "%s : %s \n", _gst_err_cat(level), gst_debug_message_get(message));
    fflush(log_fp);

}
#endif // ENABLE_LOGS

//  Local Functions
static gboolean
bus_call (GstBus     *bus,
        GstMessage *msg,
        gpointer    data)
{
    
    GMainLoop *loop = (GMainLoop *) data;

    open_log_fp();

    fprintf(log_fp,"[msg] %s from %s\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME (msg));

    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_EOS:
            gst_element_set_state (pipeline, GST_STATE_NULL);
            gst_object_unref (GST_OBJECT (pipeline));
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR: {
                                    gchar *debug;
                                    GError *err;
                                    gst_message_parse_error (msg, &err, &debug);
                                    fprintf(log_fp, "[ERROR] %s\n", debug);
                                    g_free (debug);
                                    g_error_free (err);
                                    g_main_loop_quit(loop);
                                    break;
                                }
#if 0
        case GST_MESSAGE_STATE_CHANGED:
                                {
                                    GstState state;
                                    //        gst_element_get_state       (GstElement * element,
                                    //                                     GstState * state,
                                    //                                     GstState * pending,
                                    //                                     GstClockTime timeout);

                                    gst_element_get_state(GST_ELEMENT(pipeline),&state,NULL,-1);
                                    if(state == GST_STATE_PLAYING)
                                    {

                                    }

                                }
                                break;  
#endif
        default:
                                break;
    }

    return TRUE;
}

    static gboolean
quit_loop (gpointer data)
{
    GST_DEBUG("quiting loop");
    gst_element_send_event (pipeline, gst_event_new_eos ());
    return TRUE;
}
    

static void parse_args(int argc, char** argv)
{

    gint cur = 1;
    while ( argv[cur] && cur < argc )
    {
        if( !strcmp(argv[cur],"-br") ) _bitrate = atoi(argv[cur+1]);
        else if( !strcmp(argv[cur],"-c") ) _channels = atoi(argv[cur+1]);
        else if( !strcmp(argv[cur],"-sr") ) _sample_rate = atoi(argv[cur+1]);
        else if( !strcmp(argv[cur],"-p") ) _aac_profile = atoi(argv[cur+1]);
        else if( !strcmp(argv[cur],"-l") ) _enable_logs = atoi(argv[cur+1]);
        else if( !strcmp(argv[cur],"-d") ) _record_duration = atoi(argv[cur+1]);

        cur+=2;
    }
}

// Currently unused, TODO merge all recording usecases in this app.
#if 0
char gst_pipeline[4096];

static inline GstElement* __parse_wav_pipeline()
{
    snprintf(gst_pipeline, 4096, "devsoundsrc ! audio/x-raw-int, endianness=(int)1234, signed=(boolean)true, width=(int)16, depth=(int)16, rate=(int)%d, channels=(int)%d, endianness=(int)1234 ! wavenc ! filesink location=C://Data//wav_c%d_sr%d.wav",
            _sample_rate, _channels , _channels, _sample_rate );
    pipeline = gst_parse_launch( gst_pipeline,0);
    return pipeline;
}
static inline GstElement* __get_wav_pipeline()
{
    GstCaps* caps;
    GstElement *devsoundsrc,*filesink,*wavenc;
    GError *error = NULL;
    
    pipeline = gst_pipeline_new ("pipeline");
    devsoundsrc = gst_element_factory_make ("devsoundsrc", "devsoundsrc");
    wavenc = gst_element_factory_make ("wavenc", "wavenc");
    filesink = gst_element_factory_make ("filesink", "filesink");

    snprintf(rec_filename, REC_FILENAME_LEN, "C:\\\\data\\\\wav_c%d_sr%d.wav",  _channels, _sample_rate);
    g_object_set (G_OBJECT (filesink), "location", rec_filename, NULL);


    GST_DEBUG("filtered linking...");

    caps = gst_caps_new_simple ("audio/x-raw-int",
            "width", G_TYPE_INT, 16,
            "depth", G_TYPE_INT, 16,
            "signed",G_TYPE_BOOLEAN, TRUE,
            "endianness",G_TYPE_INT, G_BYTE_ORDER,
            "rate", G_TYPE_INT, _sample_rate,
            "channels", G_TYPE_INT, _channels, NULL);    

    gst_bin_add_many (GST_BIN (pipeline), devsoundsrc, wavenc, filesink,  NULL);
    gst_element_link_filtered (devsoundsrc, wavenc, caps);


    gst_element_link (wavenc, filesink);
    gst_caps_unref (caps);

    return pipeline;
    
}
#endif 

static inline GstElement* __get_aac_pipeline()
{
    GstCaps* caps;
    GstElement *devsoundsrc,*filesink,*nokiaaacenc,*mp4mux;
    GstPad *qtsinkpad,*aacencsrcpad;
        
    pipeline = gst_pipeline_new ("pipeline");
    devsoundsrc = gst_element_factory_make ("devsoundsrc", "devsoundsrc");
    nokiaaacenc = gst_element_factory_make ("nokiaaacenc", "nokiaaacenc");
    mp4mux = gst_element_factory_make ("mp4mux", "mp4mux");
    filesink = gst_element_factory_make ("filesink", "filesink");

    snprintf(rec_filename, REC_FILENAME_LEN, "C:\\\\data\\\\rec-aac_br%d_c%d_sr%d_%s.mp4", _bitrate, _channels, _sample_rate,
            _str_aac_profile());
    g_object_set (G_OBJECT (filesink), "location", rec_filename, NULL);

    //GST_DEBUG("set bitrate on aacenc");
    g_object_set (G_OBJECT (nokiaaacenc), "bitrate", _bitrate, NULL);

    //if ( _aac_profile )
    g_object_set (G_OBJECT (nokiaaacenc), "profile", _aac_profile, NULL);


    GST_DEBUG("filtered linking...");

    caps = gst_caps_new_simple ("audio/x-raw-int",
            "width", G_TYPE_INT, 16,
            "depth", G_TYPE_INT, 16,
            "signed",G_TYPE_BOOLEAN, TRUE,
            "endianness",G_TYPE_INT, G_BYTE_ORDER,
            "rate", G_TYPE_INT, _sample_rate,
            "channels", G_TYPE_INT, _channels, NULL);    

    gst_bin_add_many (GST_BIN (pipeline), devsoundsrc, nokiaaacenc, mp4mux, filesink,  NULL);
    gst_element_link_filtered (devsoundsrc, nokiaaacenc, caps);

    qtsinkpad  = gst_element_get_request_pad( mp4mux, "audio_%d");
    aacencsrcpad  = gst_element_get_pad( nokiaaacenc, "src");  
    if (gst_pad_link (aacencsrcpad,qtsinkpad) != GST_PAD_LINK_OK) {

        GST_ERROR("gst_pad_link (aacencsrcpad,qtsinkpad) failed");
        return NULL;
    }     
    gst_element_link (mp4mux, filesink);
    gst_caps_unref (caps);

    return pipeline;
}

int main (int argc, char *argv[])
{
    GMainLoop *loop;


#ifdef ENABLE_LOGS
    if ( _enable_logs )
    setenv("GST_DEBUG","2",1);
#endif // ENABLE_LOGS

    gst_init (NULL, NULL);

    parse_args(argc, argv);

#ifdef ENABLE_LOGS
    if ( _enable_logs )
    gst_debug_add_log_function( _gstLogFunction, 0);
#endif // ENABLE_LOGS

    GST_DEBUG("args : br %d chans %d sr %d ", _bitrate, _channels, _sample_rate);

    loop = g_main_loop_new (NULL, FALSE);


    //pipeline = __get_wav_pipeline();
    //pipeline = __parse_wav_pipeline();
    pipeline = __get_aac_pipeline();


    /* start playing */
    gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (pipeline)), bus_call, loop);
    // watchdog timer
    //g_timeout_add (_record_duration * 1.5, quit_program, 0);
    
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    
    g_timeout_add (_record_duration, quit_loop, loop);
    

    g_main_loop_run (loop);

    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));

#ifdef ENABLE_LOGS
    if ( _enable_logs )
    fclose(log_fp);
#endif // ENABLE_LOGS
}


