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


#ifndef __GST_AMRMUX_H__
#define __GST_AMRMUX_H__


#include <gst/gst.h>
#include <gst/gst_global.h>


G_BEGIN_DECLS

#define GST_TYPE_AMRMUX \
  (gst_amrmux_get_type())
#define GST_AMRMUX(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_AMRMUX,GstAmrMux))
#define GST_AMRMUX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_AMRMUX,GstAmrMuxClass))
#define GST_IS_AMRMUX(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_AMRMUX))
#define GST_IS_AMRMUX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_AMRMUX))


typedef struct _GstAmrMux GstAmrMux;
typedef struct _GstAmrMuxClass GstAmrMuxClass;

/**
 * GstAmrMux:
 *
 * 
 */
struct _GstAmrMux {
  GstElement parent;

  /* pads */
  GstPad *sinkpad,*srcpad;
    
  /* Flag to decide whether to write amr header or not */
  gboolean writeheader;  
};

struct _GstAmrMuxClass {
  GstElementClass parent_class;
};

GType gst_amrmux_get_type(void);

G_END_DECLS

#endif /* __GST_AMRMUX_H__ */
