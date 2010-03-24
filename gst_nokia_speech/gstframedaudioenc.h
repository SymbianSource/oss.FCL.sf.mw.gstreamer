/* GStreamer Framed Audio Encoder
 * Copyright 2009 Collabora Ltd,
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

#ifndef __GST_FRAMED_AUDIO_ENC_H__
#define __GST_FRAMED_AUDIO_ENC_H__

#include <gst/gst.h>
#include <gst/base/gstadapter.h>

G_BEGIN_DECLS

typedef struct _GstNokiaFramedAudioEnc      GstNokiaFramedAudioEnc;
typedef struct _GstNokiaFramedAudioEncClass GstNokiaFramedAudioEncClass;

typedef GstNokiaFramedAudioEnc      GstFramedAudioEnc;
typedef GstNokiaFramedAudioEncClass GstFramedAudioEncClass;

struct _GstNokiaFramedAudioEnc
{
  /* helper for this element */
  GstElement *element;

  /* private stuff */

  /* pad with incoming data */
  GstPad *sinkpad;

  GstAdapter *adapter;
  GstClockTime next_ts;
};

typedef enum _GstDtxDecision {
  GST_DTX_DECISION_VOICE,
  GST_DTX_DECISION_SID_UPDATE,
  GST_DTX_DECISION_SID_NONE
} GstDtxDecision;

void      gst_framed_audio_enc_init (GstFramedAudioEnc * enc, GstElement * element,
          GstDebugCategory * cat);
void      gst_framed_audio_enc_reset (GstFramedAudioEnc * enc);
void      gst_framed_audio_enc_finalize (GstFramedAudioEnc * enc);
GstPad *  gst_framed_audio_enc_request_new_pad (GstFramedAudioEnc * enc,
          GstPadTemplate * templ, const gchar * req_name, GstPad ** pad_p);
void      gst_framed_audio_enc_release_pad (GstFramedAudioEnc * enc, GstPad * pad,
          GstPad ** pad_p, void (disable_cn) (GstElement *));
gboolean  gst_framed_audio_enc_sink_event (GstPad * pad, GstEvent * event);

G_END_DECLS

#endif /* __GST_FRAMED_AUDIO_ENC_H__ */
