

#include <stdlib.h>
#include <string.h>
#include <gst/gst.h>
#include "record_play.h"


GstElement *bin;
extern int mCurrentState;

static GstElement *
create_video_output ()
{
  static gboolean have_video = FALSE;
  GstBin *bin;
  GstElement *queue, *sink;
  GstPad *pad;

  if (have_video) {
    //g_print ("Already playing a video stream. Ignoring this one\n");
    return NULL;
  }
  
  /* create a new bin to hold the elements */
  if((bin = (GstBin*) gst_bin_new (NULL)) == NULL)
  //bin = (GstBin*) gst_pipeline_new("pipeline");
  return NULL;

  /* Queue to ensure all streams can push data */
  queue = gst_element_factory_make ("queue", "q");
  if(queue == NULL)
      return NULL;/* Queue should always be available */
  /* Set the queue to buffer 1/10 of a second of raw video */
  g_object_set (queue, "max-size-time", (GstClockTime) GST_SECOND / 10,
      "max-size-bytes", 0, "max-size-buffers", 0, NULL);

//  cs = gst_element_factory_make ("ffmpegcolorspace", "cs");
//  if (!cs)
//    goto no_output;
    
  /* and a video sink */
  sink = gst_element_factory_make ("fakesink"/*autovideosink*/, "sink");
  if (!sink)
    goto no_output;

  /* add objects to the main pipeline */
  gst_bin_add_many (GST_BIN (bin), queue, sink, NULL);

  /* Link the elements */
  gst_element_link_many (queue, sink, NULL);

  /* Retrieve the sinkpad from the queue and 'ghost' it onto
   * the bin so that the caller can find it generically */
  pad = gst_element_get_pad (queue, "sink");
  gst_element_add_pad (GST_ELEMENT (bin), gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  //have_video = TRUE;

  return GST_ELEMENT (bin);

  /* ERRORS */
no_output:
  {
    //g_print ("Could not create either ffmpegcolorspace or autovideosink for output");
    return NULL;
  }
}

static GstElement *
create_audio_output ()
{
  static gboolean have_audio = FALSE;
  GstBin *bin;
  GstElement *queue, *audioconvert,*audioresample, *sink;
  GstPad *pad;

  if (have_audio) {
    //g_print ("Already playing an audio stream. Ignoring this one\n");
    return NULL;
  }

  /* create a new bin to hold the elements */
  bin = (GstBin*) gst_bin_new (NULL);
  if(!bin)
      goto no_output;
  
  /* Queue to ensure all streams can push data */
  queue = gst_element_factory_make ("queue", "q");
  if (!queue) /* Queue should always be available */
      goto no_output;
  /* Set the queue to buffer 1/10 of a second of raw audio */
  g_object_set (queue, "max-size-time", (GstClockTime) GST_SECOND / 10,
      "max-size-bytes", 0, "max-size-buffers", 0, NULL);

  /* an audio converter to convert floating-point audio samples to int format */
  audioconvert = gst_element_factory_make ("audioconvert", "ac");
  if (!audioconvert)
      goto no_output;

  /* an audio converter to convert floating-point audio samples to int format */
  audioresample = gst_element_factory_make ("audioresample", "audioresample");
  if (!audioresample)
      goto no_output;
  
  /* and an audio sink */
  sink = gst_element_factory_make ("autoaudiosink", "sink");
  if (!sink)
      goto no_output;

  /* add objects to the bin */
  gst_bin_add_many (GST_BIN (bin), queue, audioconvert,audioresample, sink, NULL);

  /* link the elements */
  gst_element_link_many (queue, audioconvert,audioresample, sink, NULL);

  /* Retrieve the sinkpad from the queue element and 'ghost' it onto
   * the bin so that the caller can find it generically */
  pad = gst_element_get_pad (queue, "sink");
  gst_element_add_pad (GST_ELEMENT (bin), gst_ghost_pad_new ("sink", pad));
  gst_object_unref (pad);

  //have_audio = TRUE;

  return GST_ELEMENT (bin);

  /* ERRORS */
no_output:
  { 
    //g_print ("Could not create either ffmpegcolorspace or autovideosink for output");
    return NULL;
  }
}

static void 
new_decoded_pad (GstElement * element, GstPad * pad, gboolean last,
      GstBin *top_pipeline)
{
  GstPad *out_pad;
  GstElement *output = NULL;
  GstCaps *caps;
  GstStructure *s;
  const gchar *stream_type;

  /* Decide which output we are creating based on the stream contents */
  caps = gst_pad_get_caps (pad);
  if (caps == NULL) {
    //g_print ("Decodebin produced an unknown stream - ignoring\n");
    return;
  }
  
  s = gst_caps_get_structure (caps, 0);
  if(s == NULL)/* Caps on a pad should always have exactly one entry */
      return;
  
  stream_type = gst_structure_get_name (s);
  
  if (g_str_has_prefix (stream_type, "video/x-raw-")) {
    /* Is a new video stream */
    //g_print ("Encountered a new video stream\n");
    output = create_video_output ();
  }
  else if (g_str_has_prefix (stream_type, "audio/x-raw-")) {
    //g_print ("Encountered a new audio stream\n");
    output = create_audio_output ();
  }
  else {
    //g_print ("Found unknown stream of type %s - ignoring\n", stream_type);
  }
  
  /* If no renderer was created, ignore this stream */
  if (output == NULL)
    return;

  /* Add the output into our pipeline */
  gst_bin_add (top_pipeline, output);
  
  /* If we created a output pipeline, retrieve the sink pad from it */
  out_pad = gst_element_get_pad (output, "sink");
  g_return_if_fail (out_pad != NULL);

  /* Attempt to link the new pad to the output */  
  if (gst_pad_link (pad, out_pad) != GST_PAD_LINK_OK) {
    //g_print ("Failed to add the rendering pipeline for this new data stream\n");
    gst_bin_remove (top_pipeline, output);
    gst_object_unref (out_pad);
    return;
  }
  gst_object_unref (out_pad);
  
  /* New output renderer is successfully linked in the pipeline.
   * Change its state to playing so it is ready to receive data */
  gst_element_set_state (output, GST_STATE_PLAYING);
}

static void
print_tag (const GstTagList * list, const gchar * tag, gpointer unused)
{
  gint i, count;

  count = gst_tag_list_get_tag_size (list, tag);

  for (i = 0; i < count; i++) {
    gchar *str;

    if (gst_tag_get_type (tag) == G_TYPE_STRING) {
      if (!gst_tag_list_get_string_index (list, tag, i, &str))
        g_assert_not_reached ();
    } else {
      str =
          g_strdup_value_contents (gst_tag_list_get_value_index (list, tag, i));
    }

    if (i == 0) {
      //g_print ("  %15s: %s\n", gst_tag_get_nick (tag), str);
    } else {
      //g_print ("                 : %s\n", str);
    }

    g_free (str);
  }
}



static gboolean
bus_call (GstBus     *bus,
          GstMessage *message,
          gpointer    data)
{
  switch (GST_MESSAGE_TYPE (message)){
      case GST_MESSAGE_EOS:
        gst_message_unref (message);
        gst_element_set_state (bin, GST_STATE_NULL);
        /* Unreffing the bin will clean up all its children too */
        gst_object_unref (bin);           
        mCurrentState = NONE;
        break;
      case GST_MESSAGE_ERROR:{
        GError *gerror;
        gchar *debug;

        gst_message_parse_error (message, &gerror, &debug);
        gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
        gst_message_unref (message);
        g_error_free (gerror);
        g_free (debug);
        gst_element_set_state (bin, GST_STATE_NULL);
        /* Unreffing the bin will clean up all its children too */
        gst_object_unref (bin);           
        mCurrentState = NONE;
        break;
      }
      case GST_MESSAGE_WARNING:{
        GError *gerror;
        gchar *debug;

        gst_message_parse_warning (message, &gerror, &debug);
        gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
        gst_message_unref (message);
        g_error_free (gerror);
        g_free (debug);
        break;
      }
      case GST_MESSAGE_TAG:
      {
        GstTagList *tags;

        gst_message_parse_tag (message, &tags);
        if (tags) {
          //g_print ("TAGS received from element \"%s\".\n",
           //   GST_STR_NULL (GST_ELEMENT_NAME (GST_MESSAGE_SRC (message))));

          gst_tag_list_foreach (tags, print_tag, NULL);
          gst_tag_list_free (tags);
          tags = NULL;
        }
        break;
      }
      default:
        gst_message_unref (message);
        break;
    }
  return TRUE;
}



//static void
//event_loop (GstElement * pipe)
//{
//  GstBus *bus;
//  GstMessage *message = NULL;
//
//  bus = gst_element_get_bus (GST_ELEMENT (pipe));
//
//  while (TRUE) {
//    message = gst_bus_poll (bus, GST_MESSAGE_ANY, -1);
//
//    g_assert (message != NULL);
//
//    switch (message->type) {
//      case GST_MESSAGE_EOS:
//        gst_message_unref (message);
//        return;
//      case GST_MESSAGE_ERROR:{
//        GError *gerror;
//        gchar *debug;
//
//        gst_message_parse_error (message, &gerror, &debug);
//        gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
//        gst_message_unref (message);
//        g_error_free (gerror);
//        g_free (debug);
//        return;
//      }
//      case GST_MESSAGE_WARNING:{
//        GError *gerror;
//        gchar *debug;
//
//        gst_message_parse_warning (message, &gerror, &debug);
//        gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
//        gst_message_unref (message);
//        g_error_free (gerror);
//        g_free (debug);
//        break;
//      }
//      case GST_MESSAGE_TAG:
//      {
//        GstTagList *tags;
//
//        gst_message_parse_tag (message, &tags);
//        if (tags) {
//          //g_print ("TAGS received from element \"%s\".\n",
//              GST_STR_NULL (GST_ELEMENT_NAME (GST_MESSAGE_SRC (message))));
//
//          gst_tag_list_foreach (tags, print_tag, NULL);
//          gst_tag_list_free (tags);
//          tags = NULL;
//        }
//        break;
//      }
//      default:
//        gst_message_unref (message);
//        break;
//    }
//  }
//}

int
gst_play_file (const char* file)
{
  GstElement *filesrc, *decodebin, *sink;
  GstCaps* caps;
  //int length = strlen( file );
  //gst_init (&argc, &argv);

  if (file == NULL) {
    //g_print ("file is not present");
    goto no_output;
  }

  //g_print ("Constructing pipeline\n");
  
  /* create a new bin to hold the elements */
  bin = gst_pipeline_new ("pipeline");
  if(!bin)
      goto no_output;

  /* create a disk reader */
  filesrc = gst_element_factory_make ("filesrc", "disk_source");
  if(!filesrc)
      goto no_output;
  
  g_object_set (G_OBJECT (filesrc), "location", file, NULL);

  if( g_str_has_suffix (file, "raw") )
  {
      sink = gst_element_factory_make ("devsoundsink", "sink");
      caps = gst_caps_new_simple ("audio/x-raw-int",
                   "width", G_TYPE_INT, 16,
                   "depth", G_TYPE_INT, 16,
                   "signed",G_TYPE_BOOLEAN, TRUE,
                   "endianness",G_TYPE_INT, G_BYTE_ORDER,
                   "rate", G_TYPE_INT,  8000,
                   "channels", G_TYPE_INT, 1, NULL);     
      gst_bin_add_many (GST_BIN (bin), filesrc, sink, NULL);
      
      gst_element_link_filtered (filesrc, sink, caps);      
  }
  else if( g_str_has_suffix (file, "g711") )
  {
      sink = gst_element_factory_make ("devsoundsink", "sink");
      caps = gst_caps_new_simple ("audio/x-alaw",
                   "width", G_TYPE_INT, 16,
                   "depth", G_TYPE_INT, 16,
                   "signed",G_TYPE_BOOLEAN, TRUE,
                   "endianness",G_TYPE_INT, G_BYTE_ORDER,
                   "rate", G_TYPE_INT,  8000,
                   "channels", G_TYPE_INT, 1, NULL);   
      gst_bin_add_many (GST_BIN (bin), filesrc, sink, NULL);
      
      gst_element_link_filtered (filesrc, sink, caps);     
  }
  else if( g_str_has_suffix (file, "g729") )
  {
      sink = gst_element_factory_make ("devsoundsink", "sink");
      caps = gst_caps_new_simple ("audio/g729",
                   "width", G_TYPE_INT, 16,
                   "depth", G_TYPE_INT, 16,
                   "signed",G_TYPE_BOOLEAN, TRUE,
                   "endianness",G_TYPE_INT, G_BYTE_ORDER,
                   "rate", G_TYPE_INT,  8000,
                   "channels", G_TYPE_INT, 1, NULL);  
      gst_bin_add_many (GST_BIN (bin), filesrc, sink, NULL);
      
      gst_element_link_filtered (filesrc, sink, caps);   
  }
  else if( g_str_has_suffix (file, "ilbc") )
  {
      sink = gst_element_factory_make ("devsoundsink", "sink");
      
      caps = gst_caps_new_simple ("audio/ilbc",
                   "width", G_TYPE_INT, 16,
                   "depth", G_TYPE_INT, 16,
                   "signed",G_TYPE_BOOLEAN, TRUE,
                   "endianness",G_TYPE_INT, G_BYTE_ORDER,
                   "rate", G_TYPE_INT,  8000,
                   "channels", G_TYPE_INT, 1, NULL);   
      
      gst_bin_add_many (GST_BIN (bin), filesrc, sink, NULL);

      gst_element_link_filtered (filesrc, sink, caps);   
  }
  else if( g_str_has_suffix (file, "amr") )
  {
      sink = gst_element_factory_make ("devsoundsink", "sink");
      caps = gst_caps_new_simple ("audio/amr",
                   "width", G_TYPE_INT, 8,
                   "depth", G_TYPE_INT, 8,
                   "signed",G_TYPE_BOOLEAN, TRUE,
                   "endianness",G_TYPE_INT, G_BYTE_ORDER,
                   "rate", G_TYPE_INT,  8000,
                   "channels", G_TYPE_INT, 1, NULL);   
      gst_bin_add_many (GST_BIN (bin), filesrc, sink, NULL);
      
      //gst_element_link (source, sink);
      gst_element_link_filtered (filesrc, sink, caps);     
  }
  
  else
  {
      /* Create the decodebin */
      decodebin = gst_element_factory_make ("decodebin", NULL);
      if (!decodebin) {
        //g_print ("could not find the \"decodebin\" element\n");
        return -1;
      }
    
      /* add objects to the main pipeline */
      gst_bin_add_many (GST_BIN (bin), filesrc, decodebin, NULL);
    
      /* link the elements. */
      gst_element_link_many (filesrc, decodebin, NULL);
      
      /* Connect to decodebin's 'new-decoded-pad' signal to detect when it produces
       * a new stream */
      g_signal_connect (G_OBJECT (decodebin), "new-decoded-pad", 
          G_CALLBACK (new_decoded_pad), bin);
  }

  gst_bus_add_watch( gst_pipeline_get_bus (GST_PIPELINE (bin)), bus_call, NULL);
  //g_print ("Starting playback\n");   
  /* start playing */
  gst_element_set_state (bin, GST_STATE_PLAYING);
  return 0;
  
no_output:
      return -1;
  
}


int
gst_record_file (int type)
{
    GstElement *audiosrc, *filesink;
    char* carray = NULL;
    GstCaps* caps;
    //g_print ("Constructing pipeline\n");
    
    /* switch case for recording type*/
    switch( type )
    {
        case RECORD_RAW: 
            caps = gst_caps_new_simple ("audio/x-raw-int",
                       "width", G_TYPE_INT, 16,
                       "depth", G_TYPE_INT, 16,
                       "signed",G_TYPE_BOOLEAN, TRUE,
                       "endianness",G_TYPE_INT, G_BYTE_ORDER,
                       "rate", G_TYPE_INT,  8000,
                       "channels", G_TYPE_INT, 1, NULL);
            
            carray = "c:\\data\\sounds\\Digital\\record.raw";
            break;
            
        case RECORD_AMR: 
            {
            return gst_record_amr();
            }
            break;

        case RECORD_G711:  

            caps = gst_caps_new_simple ("audio/x-alaw",
                       "width", G_TYPE_INT, 8,
                       "depth", G_TYPE_INT, 8,
                       "signed",G_TYPE_BOOLEAN, TRUE,
                       "endianness",G_TYPE_INT, G_BYTE_ORDER,
                       "rate", G_TYPE_INT,  8000,
                       "channels", G_TYPE_INT, 1, NULL);

            carray = "c:\\data\\sounds\\Digital\\record.g711";
            break;
            
        case RECORD_G729: // 

            caps = gst_caps_new_simple ("audio/g729",
                       "width", G_TYPE_INT, 16,
                       "depth", G_TYPE_INT, 16,
                       "signed",G_TYPE_BOOLEAN, TRUE,
                       "endianness",G_TYPE_INT, G_BYTE_ORDER,
                       "rate", G_TYPE_INT,  8000,
                       "channels", G_TYPE_INT, 1, NULL);
            
            carray = "c:\\data\\sounds\\Digital\\record.g729";
            break;
            
        case RECORD_ILBC: // 

            caps = gst_caps_new_simple ("audio/ilbc",
                       "width", G_TYPE_INT, 16,
                       "depth", G_TYPE_INT, 16,
                       "signed",G_TYPE_BOOLEAN, TRUE,
                       "endianness",G_TYPE_INT, G_BYTE_ORDER,
                       "rate", G_TYPE_INT,  8000,
                       "channels", G_TYPE_INT, 1, NULL);
            
            carray = "c:\\data\\sounds\\Digital\\record.ilbc";
            break;
        case RECORD_WAV:
            {
                return gst_record_wav();
            }
            break;
        case RECORD_AAC:
            {
                return gst_record_aac();
            }
            break;
        default:
            return -1;
            break;            
    }
    /* create a new bin to hold the elements */
    bin = gst_pipeline_new ("pipeline");
    if(!bin)
        goto no_output;
    
    /* create a disk reader */
    audiosrc = gst_element_factory_make ("devsoundsrc", "audio_source");
    if(!audiosrc)
        goto no_output;
   
    /* Create the decodebin */
    filesink = gst_element_factory_make ("filesink", NULL);
    if(!filesink)
        goto no_output;
    
    g_object_set (G_OBJECT (audiosrc),
              "blocksize", 1280,
               NULL);
    
    g_object_set (G_OBJECT (filesink), "location", carray,"buffer-size",1280, NULL);
   
    /* add objects to the main pipeline */
    gst_bin_add_many (GST_BIN (bin), audiosrc, filesink, NULL);

    /* link the elements. */
    gst_element_link_filtered (audiosrc, filesink, caps);
    //gst_element_link_many (audiosrc, filesink, NULL);
    gst_bus_add_watch( gst_pipeline_get_bus (GST_PIPELINE (bin)), bus_call, NULL);
    
    //g_print ("Starting recoring\n");   
    /* start playing */
    gst_element_set_state (bin, GST_STATE_PLAYING);

    /* Run event loop listening for bus messages until EOS or ERROR */
    //event_loop (bin);
    
//    //g_print ("Finished playback - stopping pipeline\n");
//    
//    /* stop the bin */
//    gst_element_set_state (bin, GST_STATE_NULL);
//    
//    /* Unreffing the bin will clean up all its children too */
//    gst_object_unref (bin);
//    
    return 0;
no_output:
    return -1;

}


int
gst_record_wav ()
{
    GstElement *audiosrc, *filesink, *wavenc;
    char* carray = NULL;
    GstCaps* caps;
    
    //g_print ("Constructing pipeline\n");
    
    /* create a new bin to hold the elements */
    bin = gst_pipeline_new ("pipeline");
    if(!bin)
        goto no_output;

    /* create a disk reader */
    audiosrc = gst_element_factory_make ("devsoundsrc", "audio_source");
    if(!audiosrc)
        goto no_output;

    /* Create the decodebin */
    filesink = gst_element_factory_make ("filesink", NULL);
    if(!filesink)
        goto no_output;

    wavenc = gst_element_factory_make ("wavenc", NULL);
    if(!wavenc)
        goto no_output;
    
    caps = gst_caps_new_simple ("audio/x-raw-int",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT, 16000,
              "channels", G_TYPE_INT, 1, NULL);
    
    carray = "c:\\data\\sounds\\Digital\\record.wav";
    
    g_object_set (G_OBJECT (audiosrc),
              "blocksize", 1280,
               NULL);
    
    g_object_set (G_OBJECT (filesink), "location", carray,"buffer-size",1280, NULL);
    
    /* add objects to the main pipeline */
    gst_bin_add_many (GST_BIN (bin), audiosrc,wavenc, filesink, NULL);

    /* link the elements. */
    gst_element_link_filtered (audiosrc, wavenc, caps);
    gst_element_link (wavenc, filesink);
    gst_bus_add_watch( gst_pipeline_get_bus (GST_PIPELINE (bin)), bus_call, NULL);
    //g_print ("Starting recoring\n");   
    /* start playing */
    gst_element_set_state (bin, GST_STATE_PLAYING);

    return 0;
    
no_output:
    return -1;
}

int gst_pause()
{
    gst_element_set_state (bin, GST_STATE_PAUSED);
    return 0;
}

int gst_resume()
{
    gst_element_set_state (bin, GST_STATE_PLAYING);
    return 0;
}


int gst_record_stop()
{
    gst_element_send_event (bin, gst_event_new_eos ());
    //gst_element_set_state (bin, GST_STATE_NULL);
    return 0;
}


int gst_seek()
{
// need to implement..
}

int gst_get_events()
{
    return g_main_context_iteration(NULL, FALSE);
    //return 0;
}

int gst_unref()
{
    //g_print ("Finished playback - stopping pipeline\n");
    /* stop the bin */
    gst_element_set_state (bin, GST_STATE_NULL);
    /* Unreffing the bin will clean up all its children too */
    gst_object_unref (bin);   
    return 0;
}

int gst_record_aac()
{
    GstElement *audiosrc,*filesink,*aacenc, *mp4mux;
    GstBus *bus;
    GstPad *mp4sinkpad,*aacencsrcpad;
    char* carray = NULL;
    GstCaps* caps;
    
    /*create a pipeline*/
    bin = gst_pipeline_new ("pipeline");
    if(!bin)
        goto no_output;
    /* create a disk reader */
    audiosrc = gst_element_factory_make ("devsoundsrc", "audio_source");
    if(!audiosrc)
        goto no_output;

    /* Create the decodebin */
    filesink = gst_element_factory_make ("filesink", NULL);
    if(!filesink)
        goto no_output;
    //setting num-buffers
    //g_object_set (G_OBJECT (audiosrc), "num-buffers", 5000 , NULL);
    g_object_set (G_OBJECT (audiosrc),
            "blocksize", 1280,
            NULL);

    aacenc = gst_element_factory_make("nokiaaacenc", "nokiaaacenc");
    if(!aacenc)
        goto no_output;
    mp4mux = gst_element_factory_make("mp4mux", "mp4mux");
    if(!mp4mux)
        goto no_output;
    caps = gst_caps_new_simple("audio/x-raw-int",
            "width", G_TYPE_INT, 16,
            "depth", G_TYPE_INT, 16,
            "signed",G_TYPE_BOOLEAN, TRUE,
            "endianness",G_TYPE_INT, G_BYTE_ORDER,
            "rate", G_TYPE_INT, 8000,
            "channels", G_TYPE_INT, 1, NULL);
    carray = "c:\\data\\sounds\\Digital\\record.mp4";

   
    g_object_set(G_OBJECT (filesink), "location", carray, NULL);
    bus = gst_pipeline_get_bus(GST_PIPELINE (bin));
    
    gst_bus_add_watch(bus, bus_call, NULL);
   
    gst_object_unref(bus);
    

    //add objects to the main pipeline 
    gst_bin_add_many(GST_BIN (bin),audiosrc,aacenc,mp4mux,filesink, NULL);
   
    gst_element_link_filtered (audiosrc, aacenc, caps);
   
    mp4sinkpad  = gst_element_get_request_pad( mp4mux, "audio_%d");
    
    aacencsrcpad  = gst_element_get_pad( aacenc, "src");  
    if (gst_pad_link (aacencsrcpad,mp4sinkpad) != GST_PAD_LINK_OK) {

    g_print("gst_pad_link (aacencsrcpad,mp4sinkpad) failed");
    
    return -1;
    }       
    //gst_element_link (aacenc, filesink);
    gst_element_link (mp4mux, filesink);
    
    gst_caps_unref (caps);
    

    gst_element_set_state(bin, GST_STATE_PLAYING);
    
    return 0;
no_output:
    return -1;
}

int gst_record_amr()
{
      GstElement *audiosrc, *filesink, *amrmux;
      GstBus *bus;
      char* carray = NULL;
      GstCaps* caps;
       /* create a new bin to hold the elements */
      bin = gst_pipeline_new ("pipeline");
      if(!bin)
          goto no_output;
      //g_print ("pipeline created");
      audiosrc = gst_element_factory_make ("devsoundsrc", "audio_source"); 
     // encoder = gst_element_factory_make ("wavenc", NULL); 
      if(!audiosrc)
          goto no_output;

      amrmux = gst_element_factory_make ("amrmux", "muxer"); 
      if(!amrmux)
          goto no_output;

      filesink = gst_element_factory_make("filesink", "filesink");
      if(!filesink)
          goto no_output;
      
      caps = gst_caps_new_simple ("audio/amr",
                 "width", G_TYPE_INT, 8,
                 "depth", G_TYPE_INT, 8,
                 "signed",G_TYPE_BOOLEAN, TRUE,
                 "endianness",G_TYPE_INT, G_BYTE_ORDER,
                 "rate", G_TYPE_INT,  8000,
                 "channels", G_TYPE_INT, 1, NULL);
      carray = "c:\\data\\sounds\\Digital\\record.amr";
      
      g_object_set (G_OBJECT (audiosrc),
                "blocksize", 1280,
                 NULL);
      
      g_object_set(G_OBJECT (filesink), "location", carray, NULL);
       
      bus = gst_pipeline_get_bus (GST_PIPELINE (bin));
      gst_bus_add_watch (bus, bus_call, NULL);
      gst_object_unref (bus);
      

      /* add objects to the main pipeline */
      gst_bin_add_many(GST_BIN (bin),audiosrc,amrmux,filesink , NULL);
      /* link the elements */
      gst_element_link_filtered (audiosrc, amrmux, caps);
      
      gst_element_link( amrmux, filesink );
      
      gst_element_set_state (bin, GST_STATE_PLAYING);
      
      return 0;
no_output:
      return -1;
    
}
