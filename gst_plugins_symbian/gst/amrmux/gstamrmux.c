/*
 *  Copyright © 2010 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer progrs, is protected by copyright controlled by
 *  Nokia Corporation. All rights are reserved. Copying,
 *  including reproducing, storing, adapting or translating, any
 *  or all of this material requires the prior written consent of
 *  Nokia Corporation. This material also contains confidential
 *  information which may not be disclosed to others without the
 *  prior written consent of Nokia Corporation.
 * ============================================================================
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "gstamrmux.h"

#include <stdlib.h>
#include <string.h>


#ifdef __SYMBIAN32__
#include <gst/gstinfo.h>
#endif

/* AMR nb header value */
const char* amrnbmagicnumber = "#!AMR\n";

static void gst_amrmux_base_init (gpointer g_class);
static void gst_amrmux_class_init (GstAmrMuxClass * klass);
static void gst_amrmux_init ( GstAmrMux * filter, GstAmrMuxClass *filter_klass);

static GstFlowReturn gst_amrmux_chain (GstPad * pad, GstBuffer * buf);

static void gst_amrmux_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static void gst_amrmux_set_property (GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec);
                                     
static GstStateChangeReturn gst_amrmux_change_state (GstElement * element,
    GstStateChange transition);

static void gst_amrmux_dispose(GObject * object);

static GstElementClass *parent_class= NULL;

static const GstElementDetails gst_amrmux_details =
GST_ELEMENT_DETAILS ("AMR MUX Details",
        "Codec/Muxer/Audio",
        "Adaptive Multi-Rate Narrow-Band Muxer",
        "");

static GstStaticPadTemplate sink_factory =
GST_STATIC_PAD_TEMPLATE (
  "sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS ("audio/amr")
);

static GstStaticPadTemplate source_factory =
GST_STATIC_PAD_TEMPLATE (
  "src",
  GST_PAD_SRC,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS ("audio/amr")
);

enum {
  ARG_0,
  ARG_HEADER
};


GType
gst_amrmux_get_type (void)
{
  static GType gst_amrmux_type = 0;

  if (!gst_amrmux_type) {
    static const GTypeInfo amrmux_info = {
      sizeof (GstAmrMuxClass),
      gst_amrmux_base_init,
      NULL,
      (GClassInitFunc) gst_amrmux_class_init,
      NULL,
      NULL,
      sizeof (GstAmrMux),
      0,
      (GInstanceInitFunc) gst_amrmux_init,
    };

    gst_amrmux_type =
        g_type_register_static (GST_TYPE_ELEMENT, "GstAmrMux",
        &amrmux_info, (GTypeFlags)0);
  }
  return gst_amrmux_type;
}


static void
gst_amrmux_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);
   
  gst_element_class_add_pad_template(element_class,
    gst_static_pad_template_get(&sink_factory)); 
  
  gst_element_class_add_pad_template(element_class,
  gst_static_pad_template_get(&source_factory));
          
  gst_element_class_set_details (element_class, &gst_amrmux_details);
}

static void
gst_amrmux_class_init ( GstAmrMuxClass * klass )
{
  GObjectClass *object_class;
  GstElementClass *gstelement_class;

  gstelement_class = (GstElementClass *) klass;
  object_class = (GObjectClass *) klass;

  
  object_class->set_property = gst_amrmux_set_property;
  object_class->get_property = gst_amrmux_get_property;
  
  gstelement_class->change_state = gst_amrmux_change_state;
  
  object_class->dispose = gst_amrmux_dispose;
  
  parent_class = g_type_class_peek_parent(klass);
  
  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_HEADER,
        g_param_spec_boolean ("header", "header",
            "For writing AMR header", TRUE, (GParamFlags)G_PARAM_READWRITE));
  
 
 }

static void gst_amrmux_dispose(GObject * object)
{
    G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
gst_amrmux_init( GstAmrMux * amrmux, GstAmrMuxClass *amrmux_klass)
{  
   GstElementClass *klass = GST_ELEMENT_CLASS (amrmux_klass);
   
   //By default we have to write header   
   amrmux->writeheader= TRUE;
   
   //Add Sink pad to this element
      
   amrmux->sinkpad = gst_pad_new_from_template (
    gst_element_class_get_pad_template (klass, "sink"), "sink");  
  
  gst_element_add_pad (GST_ELEMENT (amrmux), amrmux->sinkpad);

    
  //Add Src pad to this element
  amrmux->srcpad = gst_pad_new_from_template (
    gst_element_class_get_pad_template (klass, "src"), "src");

  gst_element_add_pad (GST_ELEMENT (amrmux), amrmux->srcpad);  
    
  gst_pad_set_chain_function (amrmux->sinkpad, gst_amrmux_chain);

}


static void
gst_amrmux_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
    
  GstAmrMux *amrmux = GST_AMRMUX( object );
  
  switch (prop_id) {
      case ARG_HEADER:
            g_value_set_boolean (value, amrmux->writeheader);
            break;
          
          default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
  }
}


static void
gst_amrmux_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec)
{
GstAmrMux *amrmux = GST_AMRMUX( object );
  
  switch (prop_id) {
      case ARG_HEADER:
          amrmux->writeheader = g_value_get_boolean (value);
          break;
        default:
          G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
          break;
  }
}



static GstFlowReturn
gst_amrmux_chain (GstPad * pad, GstBuffer * buf)
{    
  
  gint8 *codecdata;
  guint8* amrdata;
  guint8* amrheaderstart; 
  
  int codecdatasize = GST_BUFFER_SIZE(buf); 
   
  int magicnumberlength = strlen( amrnbmagicnumber );   
  
  GstAmrMux *amrmux = GST_AMRMUX (GST_PAD_PARENT (pad)); 

  if ( amrmux->writeheader )
   {
     
      buf = gst_buffer_make_writable( buf );  
      
      //Allocate a buffer which will hold codec data + magic number
      
      amrdata = ( guint8* )g_malloc( GST_BUFFER_SIZE(buf) + magicnumberlength );
      
      //To save the starting address of amr data
      
      amrheaderstart = amrdata;
      
      //Copy magic number to newly allocated buffer
      
      memcpy( amrdata, amrnbmagicnumber, magicnumberlength);
      
      codecdata = (gint8*)GST_BUFFER_DATA (buf);  
      
      //Move the pointer to the end of magic number
           
      amrdata += magicnumberlength;
      
      //Copy codec data to newly allocated buffer
           
      memcpy( amrdata , codecdata, codecdatasize );
      
      // free codec data from GstBuffer
      
      g_free(  buf->malloc_data );
      
      //Copying the newly allocated buffer and size in GstBuffer
      
      buf->data = amrheaderstart;
      
      buf->malloc_data = amrheaderstart;
      
      buf->size+= magicnumberlength; 
      
      //Do it only first time
      
      amrmux->writeheader = FALSE;  
   }  
  
  return gst_pad_push( amrmux->srcpad , buf );
  
}

static GstStateChangeReturn
gst_amrmux_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret;
  GstAmrMux *amrmux = GST_AMRMUX (element);
  
  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  
  if (G_UNLIKELY (ret == GST_STATE_CHANGE_FAILURE))
      return ret; 

  switch (transition) {
        case GST_STATE_CHANGE_READY_TO_NULL: 
         {
            amrmux->writeheader = TRUE; 
            break;
         }               
      
         default:
            break;
  }
  
  return ret;    
}

static gboolean
plugin_init (GstPlugin * plugin)
{
 
  return gst_element_register (plugin, "amrmux", GST_RANK_PRIMARY,
          GST_TYPE_AMRMUX);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "amrmux",
    "Add header to amr-nb encoded stream",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)


EXPORT_C GstPluginDesc* _GST_PLUGIN_DESC()
{
	return &gst_plugin_desc;
}

