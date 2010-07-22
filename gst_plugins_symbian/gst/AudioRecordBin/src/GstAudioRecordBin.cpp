/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/

//  Include Files  

#include "GstAudioRecordBin.h"
#include "gstfactorylists.h"
#include <gst/gst.h>
#include <string.h>
#include <stdlib.h>

GST_DEBUG_CATEGORY_STATIC (aud_recbin_cat);     // define category (statically)
#ifdef GST_CAT_DEFAULT
#undef GST_CAT_DEFAULT
#endif

#define GST_CAT_DEFAULT aud_recbin_cat     // set as default

/** Macro to enable weak ref monitoring, used to track
 * ref / unref problems. 
 * NOTE : DO NOT USE IN HARDWARE, can be used in EMULATOR ONLY */
//#define WEAK_REF
#ifdef WEAK_REF
#define MONITOR_REF(gobj) if( G_IS_OBJECT(gobj) ) g_object_weak_ref( G_OBJECT(gobj), sWeakNotify, (gpointer) #gobj)
/** Weak Ref callback */
static void sWeakNotify (gpointer  data, GObject  *where_the_object_was);
#else
#define MONITOR_REF(gobj) /* NOP */
#endif // WEAK_REF

/** Enable the caps filter */
//#define ENABLE_CAPS_FILTER

/** The struct used to indicated the filter criteria for gst plugin factory */
typedef struct
{
    /** The type of the factory */
    GstFactoryListType type;
    /** The caps expected to be supported by a sink pad */
    GstCaps* sinkCaps;
    /** The caps expected to be supported by a src pad */
    GstCaps* srcCaps;
} FilterData;
#ifdef ENABLE_CAPS_FILTER
/** Create the capability filter */
static GstElement* sCreateCapsFilter( GstCaps* encCaps);
#endif

/** Creates the element , which accepts \a sinkcaps and 
 * provides \a srccaps on the src pad */
GstElement* sCreateCompatibleFilter(GstFactoryListType type,
        const gchar* name, GstCaps* sinkcaps, GstCaps* srccaps);


/** Utility function to create the caps depending on the mimetype, fills in the default
 * values
 * */
static GstCaps* sCreateCaps( const char* mimetype );

/**
 * Checks if a given element \a factory can accpet the \a caps on a pad
 * of given \a direction.
 */
gboolean gst_element_factory_can_accept_caps (GstElementFactory * factory,
    const GstCaps * caps, GstPadDirection direction )
{
  GList *templates;

  g_return_val_if_fail (factory != NULL, FALSE);
  g_return_val_if_fail (caps != NULL, FALSE);

  templates = factory->staticpadtemplates;

  while (templates) {
    GstStaticPadTemplate *template1 = (GstStaticPadTemplate *) templates->data;

    if (template1->direction == direction ) {
      if (gst_caps_is_subset( caps, gst_static_caps_get(&template1->static_caps)))
        return TRUE;
    }
    templates = g_list_next (templates);
  }

  return FALSE;
}
/**
 * Checks if the element factory has the \a klass_type and checks if the filter data
 * criteria ( accepts sinkCaps & srcCaps ) is met.
 */
static inline gboolean element_klass_filter (GstElementFactory * factory, const gchar *klass_type, FilterData* pData)
{
    const gchar *klass = NULL;
    gboolean sink_caps_supported = FALSE;
    gboolean src_caps_supported = FALSE;

    klass = gst_element_factory_get_klass (factory);
    GST_DEBUG("klass = %s\n", klass);

    if (strstr (klass, klass_type) != NULL)
    {
        sink_caps_supported = gst_element_factory_can_accept_caps( factory, pData->sinkCaps, GST_PAD_SINK );               
    
        src_caps_supported = gst_element_factory_can_accept_caps( factory, pData->srcCaps, GST_PAD_SRC );

        GST_INFO("%s fact name = %s, can sink %d can src %d \n",
                klass_type,
                GST_PLUGIN_FEATURE_NAME(factory),
                sink_caps_supported, src_caps_supported
                );

        return TRUE && sink_caps_supported && src_caps_supported;
    }

    return FALSE;
}

/**
 * gst_factory_list_is_type:
 * @factory: a #GstElementFactory
 * @type: a #GstFactoryListType
 *
 * Check if @factory if of the given types.
 *
 * Returns: %TRUE if @factory is of @type.
 */
gboolean gst_factory_list_is_type (GstElementFactory * factory, FilterData * pData)
{
    gboolean res = FALSE;
    const char* encoder_klass_str = "Encoder";
    const char* muxer_klass_str = "Muxer";

    /* NOTE : GST_FACTORY_LIST_SINK, GST_FACTORY_LIST_DECODER, GST_FACTORY_LIST_SRC
     * ignored.
     */
    if (!res && (pData->type & GST_FACTORY_LIST_ENCODER))
        res = element_klass_filter (factory,encoder_klass_str, pData);
    if (!res && (pData->type & GST_FACTORY_LIST_MUXER))
        res = element_klass_filter (factory,muxer_klass_str, pData);

    return res;
}

/** The element filter used along with gst registry feature filter;
 * The filter criteria is indicated in \a pData. */ 
static gboolean element_filter (GstPluginFeature * feature, FilterData * pData)
{
    gboolean res;

    /* we only care about element factories */
    if (G_UNLIKELY (!GST_IS_ELEMENT_FACTORY (feature)))
        return FALSE;

    res = gst_factory_list_is_type (GST_ELEMENT_FACTORY_CAST (feature), pData);

    return res;
}

/* function used to sort element features, by comparing the ranks.
 * If ranks are same, then the plugin name is used. */
static gint compare_ranks(GstPluginFeature * f1, GstPluginFeature * f2)
{
  gint diff;
  const gchar *rname1, *rname2;

  diff = gst_plugin_feature_get_rank (f2) - gst_plugin_feature_get_rank (f1);
  if (diff != 0)
    return diff;

  rname1 = gst_plugin_feature_get_name (f1);
  rname2 = gst_plugin_feature_get_name (f2);

  diff = strcmp (rname2, rname1);

  return diff;
}

/** Returns a GList of factory elements, of a given \a type and which accpets \a sinkCaps and
 * and provides \a srcCaps.
 * */
GList* gst_factory_list_get_elements (GstFactoryListType type, GstCaps* sinkCaps, GstCaps* srcCaps)
{
    FilterData* pData = NULL;
    GList *list = NULL;

    // construct the data
    pData = (FilterData*) calloc(sizeof(FilterData), 1);
    pData->sinkCaps = sinkCaps;
    pData->srcCaps = srcCaps;
    pData->type = type;

    /* get the feature list using the filter */
    list = gst_default_registry_feature_filter ((GstPluginFeatureFilter)
            element_filter, FALSE, pData);

    free( pData );
    
    // sort depending on the ranks
    if ( list )
        list = g_list_sort( list, (GCompareFunc) compare_ranks );

    GST_INFO("sort list done");

    return list;

}
/** Gets the pad depending on th pad template. */
static GstPad * gst_element_get_pad_from_template (GstElement * element, GstPadTemplate * templ)
{
    GstPad *ret = NULL;
    GstPadPresence presence;

    /* If this function is ever exported, we need check the validity of `element'
     * and `templ', and to make sure the template actually belongs to the
     * element. */

    presence = GST_PAD_TEMPLATE_PRESENCE (templ);

    switch (presence) {
        case GST_PAD_ALWAYS:
        case GST_PAD_SOMETIMES:
            GST_INFO("presence : always/sometimes");
            ret = gst_element_get_static_pad (element, templ->name_template);
            if (!ret && presence == GST_PAD_ALWAYS)
                g_warning
                    ("Element %s has an ALWAYS template %s, but no pad of the same name",
                     GST_OBJECT_NAME (element), templ->name_template);
            break;

        case GST_PAD_REQUEST:
            GST_INFO("presence : request audio_1");
            //ret = gst_element_request_pad (element, templ, NULL);
            ret = gst_element_get_request_pad( element, "audio_%d");
            break;
    }

    return ret;
}

/* Finds compatible muxer sink pad, for a given \a encoder \ref GstElement */
static inline GstPad * get_compatible_muxer_sink_pad (GstElement * muxer, GstElement * encoder) //pass in the right arguments here
{
    GstPad *srcpad = 0, *sinkpad = 0;
    GstPadTemplate *srctempl = 0;
    GstPadTemplate *sinktempl = 0;

    srcpad = gst_element_get_static_pad (encoder, "src");

    srctempl = gst_pad_get_pad_template (srcpad);

    GST_DEBUG (
            "Attempting to find pad from muxer %s compatible with %s:%s",
            GST_ELEMENT_NAME (muxer), GST_DEBUG_PAD_NAME (srcpad));
    
    gst_object_unref( srcpad );

    sinktempl = gst_element_get_compatible_pad_template (muxer, srctempl);
    if (G_UNLIKELY (sinktempl == NULL))
        goto no_template;

    sinkpad = gst_element_get_pad_from_template (muxer, sinktempl);
    
    return sinkpad;

no_template:
    {
        GST_ERROR ("No compatible pad available on muxer");
        return NULL;
    }
}


/**
 * Creates the encoder bin.
 * If the function is successful, A encoder bin with a ghost sink pad 
 * which accepts the \a encCaps and a ghost source pad which gives
 * \a muxCaps type is available.
 * 
 * Algo:
 * 
 * 1. Check if any pads of src give out encCaps
 * 1.1 If yes, 
 *      -> Create a list of muxers available in the gst plugin repository and select the appropriate muxer
 *      -> connect src -> muxer, create encbin.
 * 1.2 If no,
 *      -> Create a list of encoders availabe in the gst plugin repo and  select a appropriate enc
 *      -> connect src -> capsfilter (optional) -> enc -> muxer and return encbin
 *      
 * 
 * \param [in] src The source gst element
 * \param [in] encCaps The capability of the encoder being created.
 * \param [in] muxCaps The capability type of the muxer to create.
 * 
 * \return On successful the ptr to encoder bin gst element is returned, else NULL.
 */
EXPORT_C GstElement* CreateAudioRecordBin( GstElement* src, GstCaps* encCaps, GstCaps* muxCaps)
{
    GstElement *encbin = NULL; // The top level encoder bin
    GstPad *ghostsrc = NULL, *ghostsink = NULL; // Ghost src, sink pads of the enc bin
    GstElement *muxer = NULL; // The muxer
    GstElement *encoder = NULL; // The encoder
#ifdef ENABLE_CAPS_FILTER
    GstElement * audiofilter = NULL; // Audio filter
#endif // ENABLE_CAPS_FILTER
    GstPad* src_pad = 0;
    gboolean accept_src_pad = 0;
    GstCaps* src_caps = 0;
    
    if ( !src || !encCaps || !muxCaps )
    {
    		GST_ERROR("invalid args");
    		return NULL;
    }    

    /* gst initialization */
    if ( FALSE == gst_init_check(NULL, NULL,NULL) )
    {
        GST_ERROR("gst_init_check FAIL");
        return NULL;
    }

    /* Initialize the debug category */
    GST_DEBUG_CATEGORY_INIT (aud_recbin_cat, "AudioRecordBin",
            0, "Audio record bin plugin");


    /* 1. Create the bin */
    encbin = gst_bin_new("encbin");
    if ( !encbin )
    {
        GST_ERROR("encbin creation FAIL");
        return NULL;
    }
    MONITOR_REF(encbin);

    /* 2. Get the src pad, of the source element */
    src_pad = gst_element_get_pad (src, "src");

    src_caps = gst_pad_get_caps( src_pad );

    /* 2.1 Check if the src gst element can directly give the encCaps */
    accept_src_pad = gst_caps_is_subset( encCaps ,src_caps);   

    GST_INFO(" accept caps %d ", accept_src_pad );
    gst_object_unref (src_pad);
    gst_caps_unref( src_caps );

    if ( !accept_src_pad )
    {
        GstCaps* enc_in_caps = 0;

        /* The src pad does not directly give the encoded format,
         * fallback to x-raw-int / x-raw-float and construct the pipeline
         * */
        enc_in_caps = sCreateCaps("audio/x-raw-int");

        encoder = sCreateCompatibleFilter( GST_FACTORY_LIST_ENCODER, "encoder", enc_in_caps, encCaps);
        if (!encoder )
        {
            GST_ERROR("encoder creation FAIL");
            gst_caps_unref( enc_in_caps );
            goto CLEANUP;
        }
        
#ifdef ENABLE_CAPS_FILTER
        audiofilter = sCreateCapsFilter(enc_in_caps);
#endif // ENABLE_CAPS_FILTER
        
        gst_caps_unref( enc_in_caps );

        // add elements to the enc bin
        if (gst_bin_add(GST_BIN(encbin), encoder))
        {
            GST_INFO("Added encoder to bin");
        }
        else
        {
            GST_ERROR("Could not add encoder to bin");
            goto CLEANUP;
        }

    }

    /* construct the muxer which can accept the encCaps and give out muxCaps */
#ifdef ENABLE_CAPS_FILTER
    /* Add a caps filter, if not created above */
    if ( !audiofilter )
        audiofilter = sCreateCapsFilter(encCaps);

    if ( !audiofilter)
    {
        GST_ERROR("audio filter creation FAIL ");
        goto CLEANUP;
    }
    MONITOR_REF(audiofilter);
    
    // add elements to the enc bin
    if (gst_bin_add(GST_BIN(encbin), audiofilter))
    {
        GST_INFO("Added audio filter to bin");
    }
    else
    {
        GST_ERROR("Could not add audio filter to bin");
        goto CLEANUP;
    }
#endif // ENABLE_CAPS_FILTER


    // Create the muxer 
    muxer = sCreateCompatibleFilter( GST_FACTORY_LIST_MUXER, "muxer", encCaps, muxCaps );

    if ( !muxer )
    {
        GST_ERROR("no suitable muxer found");
        goto CLEANUP;
    }

    MONITOR_REF(muxer);

    if (gst_bin_add(GST_BIN(encbin), muxer))
    {
        GST_INFO("Added muxer to bin");
    }
    else
    {
        GST_ERROR("Could not add muxer to bin");
        goto CLEANUP;
    }

    // link the added elements.
    // If encoder is present, then the bin will have,
    // capsfilter -> encoder -> muxer
    // or else
    // capsfilter -> muxer.
    // Note that caps filter is optionally controlled by macro ENABLE_CAPS_FILTER
    if ( encoder )
    {
        GstPad* srcpad      = 0;
        GstPad* muxerpad    = 0;
#ifdef ENABLE_CAPS_FILTER
        GST_INFO("audiofilter -> encoder");
        if(!gst_element_link(audiofilter, encoder))
        {
            GST_ERROR("Could not link audiofilter -> encoder!!");
            goto CLEANUP;
        }
#endif //ENABLE_CAPS_FILTER
        
        // get the src pad
        srcpad = gst_element_get_static_pad (encoder, "src");
        
        // get the sink pad of muxer
        GST_INFO("get compatible mux sink pad ... ");
        muxerpad = get_compatible_muxer_sink_pad (muxer, encoder);

        // link 
        if (G_UNLIKELY (gst_pad_link (srcpad, muxerpad) != GST_PAD_LINK_OK))
        {
            GST_ERROR("gst_pad_link fail");
            goto CLEANUP;
        }
        gst_object_unref( srcpad );
        gst_object_unref( muxerpad );
        GST_INFO("gst_pad_link successful");

    }
#ifdef ENABLE_CAPS_FILTER
    else 
    {
        GST_INFO("audiofilter -> muxer ");
        if(!gst_element_link(audiofilter, muxer))
        {
            GST_ERROR("Could not link audiofilter to muxer!!");
            goto CLEANUP;
        }
    }
#endif //ENABLE_CAPS_FILTER
    GST_INFO("linking complete");

    // create the ghost pads
    // The ghost src pad is provided by the muxer
    ghostsrc = gst_element_get_static_pad(muxer, "src");
    // The ghost sink pad is determined by the availability
    // of audiofilter, encoder.
#ifdef ENABLE_CAPS_FILTER
    ghostsink = gst_element_get_static_pad(audiofilter, "sink");
#else
    if ( encoder )
        ghostsink = gst_element_get_static_pad(encoder, "sink");
    else
        ghostsink = gst_element_get_static_pad(muxer, "sink");
#endif //ENABLE_CAPS_FILTER

    MONITOR_REF( ghostsrc );
    MONITOR_REF( ghostsink );

    if(ghostsrc)
    {
        gst_element_add_pad(encbin, gst_ghost_pad_new("src",ghostsrc));
        gst_object_unref(GST_OBJECT(ghostsrc));
        GST_INFO("+ghost src");
    }
    if(ghostsink)
    {
        gst_element_add_pad(encbin, gst_ghost_pad_new("sink",ghostsink));
        gst_object_unref(GST_OBJECT(ghostsink));
        GST_INFO("+ghost sink");
    }
    return encbin;
CLEANUP:
    // release only if the gst element has not been added to the bin;
    // when added to a bin, the bin ( i.e the parent ) takes care of
    // releasing.
    if ( encoder && !gst_element_get_parent(encoder) )
        gst_object_unref( encoder );
    if ( muxer && !gst_element_get_parent(muxer))
        gst_object_unref( muxer );
    #ifdef ENABLE_CAPS_FILTER
    if ( audiofilter && !gst_element_get_parent(audiofilter))
        gst_object_unref( audiofilter );
    #endif //ENABLE_CAPS_FILTER
    if ( encbin )
        gst_object_unref( encbin );
    return NULL;
}
#ifdef ENABLE_CAPS_FILTER
/** Create the caps filter */
GstElement* sCreateCapsFilter( GstCaps* encCaps)
{
    GstElement* audiofilter = NULL;

    audiofilter = gst_element_factory_make("capsfilter", "audiofilter");
    if( !audiofilter )
    {
        return NULL;
    }

    g_object_set( G_OBJECT(audiofilter), "caps",encCaps,NULL);

    return audiofilter;
}
#endif
/** Create a filter, which accepts \a encCaps on sink pad and 
 * provides \a muxCaps on the src pad */
GstElement* sCreateCompatibleFilter(GstFactoryListType type,
        const gchar* name, GstCaps* sinkcaps, GstCaps* srccaps)
{

    GList* factory_lst = NULL;
    GstElementFactory * factory = NULL;
    GstElement* element = NULL;
    guint nFactory = 0;
    
    GST_DEBUG("before gst_factory_list_get_elements");

    factory_lst = gst_factory_list_get_elements(type, sinkcaps, srccaps);
    
    GST_DEBUG("gst_factory_list_get_elements");
    
    nFactory = g_list_length ( factory_lst );   
    
    if ( !nFactory )
    {
        GST_ERROR("no matching %s found", name );
        return NULL;
    }
    GST_INFO("nElement %d", nFactory);

    // The first element factory in the sorted list is chosen
    factory = (GstElementFactory*) g_list_nth_data (factory_lst, 0);
    if ( factory )
    {
        GST_INFO("%s selected = %s can sink %d src %d ", 
                name,
                GST_PLUGIN_FEATURE_NAME(factory),
                gst_element_factory_can_sink_caps(factory,sinkcaps),
                gst_element_factory_can_src_caps(factory,srccaps)
                );

        element =  gst_element_factory_create(factory, name);
        if (!element)
        {
            g_print("could not create element!");
        }
    }

    // free the muxers list
    gst_plugin_feature_list_free( factory_lst );

    return element;
}


/** Utility function to create the caps depending on the mimetype, fills in the default
 * values
 * */
static GstCaps* sCreateCaps( const char* mimetype )
{
    GstCaps* caps = NULL;
    GST_INFO( "inside sCreateCaps");
    if ( !strcmp(mimetype, "audio/x-raw-int") ||
            !strcmp(mimetype, "audio/x-raw-float") )
    {
        caps = gst_caps_new_simple(mimetype,
                "width", G_TYPE_INT, 16,
                "depth", G_TYPE_INT, 16,
                "signed",G_TYPE_BOOLEAN, TRUE,
                "endianness",G_TYPE_INT, G_BYTE_ORDER,
                "rate", G_TYPE_INT, 16000,
                "channels", G_TYPE_INT, 1, 
                NULL);
    }
    else
    {
        // fallback to default 
        GST_INFO("fallback");
        caps = gst_caps_new_simple(mimetype, NULL);
        
    }
    return caps;
} 

#ifdef WEAK_REF
void sWeakNotify (gpointer  data, GObject  *where_the_object_was)
{
    GST_DEBUG("__release__ %s", (const char*) data );
}
#endif // WEAK_REF


//  Exported Functions

#ifndef EKA2 // for EKA1 only
EXPORT_C TInt E32Dll(TDllReason /*aReason*/)
    // Called when the DLL is loaded and unloaded. Note: have to define
    // epoccalldllentrypoints in MMP file to get this called in THUMB.
{
    return KErrNone;
}
#endif


