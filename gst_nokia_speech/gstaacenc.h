/* GStreamer AAC encoder
 * Copyright 2009 Collabora Multimedia,
 * Copyright 2009 Nokia Corporation
 *  @author: Mark Nauwelaerts <mark.nauwelaerts@collabora.co.uk>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* TODO non-GPL license */

#ifndef __GST_AAC_ENC_H__
#define __GST_AAC_ENC_H__

#include <gst/gst.h>

#include <eaacplus_api.h>

#include "gstframedaudioenc.h"

G_BEGIN_DECLS

/* #define's don't like whitespacey bits */
#define GST_TYPE_AAC_ENC			\
  (gst_aac_enc_get_type())
#define GST_AAC_ENC(obj)						\
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_AAC_ENC,GstAACEnc))
#define GST_AAC_ENC_CLASS(klass)					\
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_AAC_ENC,GstAACEncClass))
#define GST_IS_AAC_ENC(obj)					\
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_AAC_ENC))
#define GST_IS_AAC_ENC_CLASS(klass)				\
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_AAC_ENC))
#define GST_AAC_ENC_CAST(obj)          ((GstAACEnc*)(obj))


typedef struct _GstNokiaAACEnc      GstNokiaAACEnc;
typedef struct _GstNokiaAACEncClass GstNokiaAACEncClass;

typedef GstNokiaAACEnc      GstAACEnc;
typedef GstNokiaAACEncClass GstAACEncClass;

struct _GstNokiaAACEnc
{
  GstElement element;

  GstPad *sinkpad, *srcpad, *cnpad;

  GstFramedAudioEnc enc;
  HANDLE_AACPLUS_ENC encoder;
  AACPLUS_ENC_INFO info;

  /* mode selection */
  GstClockTime frame_duration;
  gint raw_frame_size;
  gint codec_frame_size;

  /* optional helper (history) buffer */
  guint8 *buffer;

  /* stream description */
  gint rate;
  gint channels;

  /* properties */
  gint bitrate;
  guint profile;
  guint format;
};

struct _GstNokiaAACEncClass
{
  GstElementClass parent_class;
};

GType gst_aac_enc_get_type (void);

G_END_DECLS

#endif /* __GST_AAC_ENC_H__ */
