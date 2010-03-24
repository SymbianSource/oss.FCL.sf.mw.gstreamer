
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>


#define LOG_FILE "c:\\logs\\hello_world_logs.txt" 
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

static void
event_loop (GstElement * pipe)
{
  GstBus *bus;
  GstMessage *message = NULL;

  bus = gst_element_get_bus (GST_ELEMENT (pipe));

  while (TRUE) {
    message = gst_bus_poll (bus, GST_MESSAGE_ANY, -1);

    g_assert (message != NULL);

    switch (message->type) {
      case GST_MESSAGE_EOS:
        gst_message_unref (message);
        return;
      case GST_MESSAGE_WARNING:
      case GST_MESSAGE_ERROR:{
        GError *gerror;
        gchar *debug;

        gst_message_parse_error (message, &gerror, &debug);
        gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
        gst_message_unref (message);
        g_error_free (gerror);
        g_free (debug);
        return;
      }
      default:
        gst_message_unref (message);
        break;
    }
  }
}

int
main (int argc, char *argv[])
{
  GstElement *bin, *filesrc, *decoder, *audiosink;
  GstElement *conv, *resample;
	xmlfile = "helloworld_logs";
  std_log(LOG_FILENAME_LINE, "Test Started hellowworld");
  
  gst_init (&argc, &argv);
 
  if (argc != 2) {
    g_print ("usage: %s <mp3 file>\n", argv[0]);
    std_log(LOG_FILENAME_LINE, "Test Failed argument need to be passed");
    create_xml(1); 
    exit (-1);
  }

  /* create a new bin to hold the elements */
  bin = gst_pipeline_new ("pipeline");
  g_assert (bin);

  /* create a disk reader */
  filesrc = gst_element_factory_make (/*"filesrc"*/"audiotestsrc", "disk_source");
  g_assert (filesrc);
  g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);

  /* now it's time to get the decoder */
  //decoder = gst_element_factory_make ("mad", "decode");
//  if (!decoder) {
//    std_log(LOG_FILENAME_LINE, "could not find plugin mad");
//    g_print ("could not find plugin \"mad\"");
//    return -1;
//  }

  /* also, we need to add some converters to make sure the audio stream
   * from the decoder is converted into a format the audio sink can
   * understand (if necessary) */
//  conv = gst_element_factory_make ("audioconvert", "audioconvert");
//  if (!conv) {
//    std_log(LOG_FILENAME_LINE, "could not create \"audioconvert\" element!");
//    g_print ("could not create \"audioconvert\" element!");
//    return -1;
//  }
//  resample = gst_element_factory_make ("audioresample", "audioresample");
//  if (!conv) {
//    std_log(LOG_FILENAME_LINE, "could not create \"audioresample\" element!");
//    g_print ("could not create \"audioresample\" element!");
//    return -1;
//  }

  /* and an audio sink */
  audiosink = gst_element_factory_make ("devsoundsink", "play_audio");
  g_assert (audiosink);

  /* add objects to the main pipeline */
  gst_bin_add_many (GST_BIN (bin), filesrc/*, decoder, conv,
      resample*/, audiosink, NULL);

  /* link the elements */
  gst_element_link_many (filesrc, /*decoder, conv, resample,*/ audiosink, NULL);

  /* start playing */
  std_log(LOG_FILENAME_LINE, "START PLAYING");
  gst_element_set_state (bin, GST_STATE_PLAYING);
  std_log(LOG_FILENAME_LINE, "STOP PLAYING ");
  /* Run event loop listening for bus messages until EOS or ERROR */
  event_loop (bin);

  /* stop the bin */
  std_log(LOG_FILENAME_LINE, "START BIN");
  gst_element_set_state (bin, GST_STATE_NULL);
  std_log(LOG_FILENAME_LINE, "START BIN");

	std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  exit (0);
}
