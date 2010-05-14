
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/gstelement.h>
#include <string.h>
#define LOG_FILE "c:\\logs\\launch_logs.txt" 
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

GstElement *pipeline, *source, *wavparse,*sink,*conv,*resample,*decoder,*audioconvert,*audioresample ;
GstBus *bus;
GMainLoop *loop;
//gboolean    link_with_devsoundsink;

//audioconvert ! audioresample 
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
        create_xml(0); 
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug;
      GError *err;
      gst_message_parse_error (msg, &err, &debug);
      g_free (debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      //std_log(LOG_FILENAME_LINE, "Test Failed");
      create_xml(1); 
      break;
    }
    default:
      break;
  }

  return TRUE;
}

//static void 
//new_pad_cb (GstElement *wavparse, GstPad *new_pad, gpointer pipeline)
// {
//   
//   gst_element_set_state (pipeline, GST_STATE_PAUSED);
//
//   if (!gst_element_link (wavparse, audioconvert))
//      g_error ("link(wavparse, audioconvert) failed!\n");
//
//   gst_element_set_state (pipeline, GST_STATE_PLAYING);
// }


static void
cb_newpad (GstElement *decodebin,
       GstPad     *pad,
       gboolean    last,
       gpointer    data)
{
    GstCaps *caps;
    GstStructure *str;
    GstPadLinkReturn linkret= GST_PAD_LINK_OK ;
   

    GstPad* sinkpad = NULL;

    
    /* check media type */
    caps = gst_pad_get_caps (pad);
    str = gst_caps_get_structure (caps, 0);
    if (!g_strrstr (gst_structure_get_name (str), "audio")) {
    gst_caps_unref (caps);
    return;
    }
    gst_caps_unref (caps);
    
//    if( link_with_devsoundsink )
//        {
        sinkpad = gst_element_get_pad( sink, "sink");
        //gst_bin_add_many (GST_BIN (pipeline),source, decoder,sink, NULL);
            
//        }
//    else
//        {
//        audioconvert = gst_element_factory_make ("audioconvert", "audioconvert");
//        audioresample = gst_element_factory_make ("audioresample", "audioresample");        
//        sinkpad = gst_element_get_pad( audioconvert, "sink");
//        
//        gst_bin_add_many (GST_BIN (pipeline),source, decoder,audioconvert,audioresample,sink, NULL);
//        
//        if (!gst_element_link (audioconvert,audioresample))
//           g_error ("link(audioconvert,audioresample, sink) failed!\n");   
//        if (!gst_element_link (audioresample, sink))
//           g_error ("link(audioconvert,audioresample, sink) failed!\n");         
//        }
    
    linkret = gst_pad_link (pad, sinkpad);
}

gboolean  cb_autoplug( GstBin * *bin,
                        GstPad        *pad,
                        GstCaps       *caps,
                        gpointer       user_data)
{
    GstCaps* sinkcaps = NULL;    
    gboolean supported = FALSE;
    
    GstPad* sinkpad = gst_element_get_pad( sink, "sink");
    sinkcaps = gst_pad_get_caps( sinkpad );
    supported = gst_caps_is_subset( caps, sinkcaps );
    
    if( supported )
    {
        //link_with_devsoundsink = TRUE;
        return FALSE;
    }
    return TRUE;
}

int main (int argc, char *argv[])
{
	int is_mp3_file = 0;
  const char* ext = 0;  
    //xmlfile = "launch_logs";
    ////std_log(LOG_FILENAME_LINE, "Test Started launch");
 
    if (argc != 2) {
      g_print ("usage: %s <Test file>\n", argv[0]);
      //std_log(LOG_FILENAME_LINE, "Test Failed");
      create_xml(1); 
      exit (-1);
    }
    ext = strrchr(argv[1],'.');
    if ( ext && ( !strcmp(ext,".mp3") || !strcmp(ext,".Mp3") || !strcmp(ext,".MP3") || !strcmp(ext,".mP3") ) )
    	is_mp3_file = 1;
    //link_with_devsoundsink = FALSE;
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);
    
    /* create elements */
    pipeline = gst_pipeline_new ("audio-player");
    source = gst_element_factory_make ("filesrc", "file-source");
    if (is_mp3_file)
		  decoder = gst_element_factory_make ("mp3parse", "mp3parse");
    else
  		decoder = gst_element_factory_make ("decodebin2", "decodebin2-decoder");

  
    sink = gst_element_factory_make ("devsoundsink", "sink");    
//    audioconvert = gst_element_factory_make ("audioconvert", "audioconvert");
//    audioresample = gst_element_factory_make ("audioresample", "audioresample");    
//    sink = gst_element_factory_make ("devsoundsink", "sink");
    
    if (!pipeline || !source || !decoder || !sink) {
        g_print ("One element could not be created\n");
        return -1;
        }
    /* set filename property on the file source. Also add a message  handler. */
    g_object_set (G_OBJECT (source), "location", argv[1], NULL);
            /* put all elements in a bin */
    gst_bin_add_many (GST_BIN (pipeline),source, decoder,sink, NULL);
    if (is_mp3_file)
  		gst_element_link_many (source, decoder, sink, NULL);
		else
		{
      /* link together - note that we cannot link the parser and  decoder yet, because the parser uses dynamic pads. For that, we set a pad-added signal handler. */
    	gst_element_link (source, decoder);
    }


    gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (pipeline)), bus_call, loop);
    if (!is_mp3_file)
		{
    	g_signal_connect (decoder, "new-decoded-pad", G_CALLBACK (cb_newpad), NULL);
    	g_signal_connect (decoder, "autoplug-continue", G_CALLBACK (cb_autoplug), NULL);
		}
    
    //g_signal_connect (decoder, "pad-added", G_CALLBACK (new_pad_cb),pipeline);
            /* Now set to playing and iterate. */
    g_print ("Setting to PLAYING\n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_print ("Running\n");
    g_main_loop_run (loop);
            /* clean up nicely */
    g_print ("Returned, stopping playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);
    g_print ("Deleting pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));
    
    g_print ("completed palying audio\n");
    ////std_log(LOG_FILENAME_LINE, "Test Successful");
    //create_xml(0); 
    return 0;
}

