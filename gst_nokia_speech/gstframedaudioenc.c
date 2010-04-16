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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <string.h>

#include "gstframedaudioenc.h"

/* generic part */
#ifndef RAW_FRAME_SIZE

/* this will reference caller's debug category;
 * there is a copy of this per plugin lib (= debug category) */
GST_DEBUG_CATEGORY_STATIC (framedaudioenc_debug);
#define GST_CAT_DEFAULT framedaudioenc_debug

void
gst_framed_audio_enc_reset (GstFramedAudioEnc * enc)
{
  gst_adapter_clear (enc->adapter);
  enc->next_ts = GST_CLOCK_TIME_NONE;
}

/* installs @enc as element private for @element's pad,
 * and possibly some event and query handler.
 * if these need overriding, chain up to them
 * chain and setcaps still need to be set by @element */
void
gst_framed_audio_enc_init (GstFramedAudioEnc * enc, GstElement * element,
    GstDebugCategory * cat)
{
  enc->element = element;
#ifndef GST_DISABLE_GST_DEBUG
  framedaudioenc_debug = cat;
#endif

  enc->adapter = gst_adapter_new ();

  /* hook some */
  enc->sinkpad = gst_element_get_pad (enc->element, "sink");
  g_assert (enc->sinkpad);
  gst_pad_set_element_private (enc->sinkpad, enc);

  /* watch downstream events */
  gst_pad_set_event_function (enc->sinkpad,
      GST_DEBUG_FUNCPTR (gst_framed_audio_enc_sink_event));

  gst_framed_audio_enc_reset (enc);
}

void
gst_framed_audio_enc_finalize (GstFramedAudioEnc * enc)
{
  gst_object_unref (enc->adapter);

  gst_pad_set_element_private (enc->sinkpad, NULL);
  gst_object_unref (enc->sinkpad);
}

GstPad *
gst_framed_audio_enc_request_new_pad (GstFramedAudioEnc * enc,
    GstPadTemplate * templ, const gchar * req_name, GstPad ** pad_p)
{
  GstElement *element;
  GstPad *newpad;
  GstElementClass *klass;
  GstCaps *caps;

  g_return_val_if_fail (templ != NULL, NULL);

  element = enc->element;
  klass = GST_ELEMENT_GET_CLASS (element);

  if (templ != gst_element_class_get_pad_template (klass, "cn"))
    goto wrong_template;

  GST_DEBUG_OBJECT (enc->element, "adding cn pad");
  newpad = gst_pad_new_from_template (templ, "cn");
  /* set template caps */
  caps = gst_caps_copy (gst_pad_get_pad_template_caps (newpad));
  gst_pad_set_caps (newpad, caps);
  gst_caps_unref (caps);
  gst_pad_use_fixed_caps (newpad);
  gst_pad_set_active (newpad, TRUE);
  /* only 1 pad by name can be added */
  if (gst_element_add_pad (element, newpad)) {
    GST_OBJECT_LOCK (element);
    gst_object_replace ((GstObject **) pad_p, GST_OBJECT_CAST (newpad));
    GST_OBJECT_UNLOCK (element);
    GST_DEBUG_OBJECT (enc->element, "cn pad added");
  } else {
    gst_object_unref (newpad);
    goto already_requested;
  }

  return newpad;

  /* ERRORS */
wrong_template:
  {
    GST_ERROR_OBJECT (element, "not our template!");
    return NULL;
  }
already_requested:
  {
    GST_ERROR_OBJECT (element, "only 1 instance of a pad can be requested");
    return NULL;
  }
}

void
gst_framed_audio_enc_release_pad (GstFramedAudioEnc * enc, GstPad * pad,
    GstPad ** pad_p, void (disable_cn) (GstElement *))
{
  GstElement *element = enc->element;

  GST_DEBUG_OBJECT (enc->element, "releasing cn pad");

  GST_OBJECT_LOCK (element);
  if (pad != *pad_p)
    goto wrong_pad;
  GST_OBJECT_UNLOCK (element);

  /* reconfigure encoder */
  disable_cn (element);

  if (gst_element_remove_pad (element, pad)) {
    GST_OBJECT_LOCK (element);
    gst_object_replace ((GstObject **) pad_p, NULL);
    GST_OBJECT_UNLOCK (element);
    GST_DEBUG_OBJECT (enc->element, "cn pad released");
  }

  /* ERRORS */
wrong_pad:
  {
    GST_OBJECT_UNLOCK (element);
    GST_ERROR_OBJECT (element, "pad not requested; can not be released!");
    return;
  }
}

gboolean
gst_framed_audio_enc_sink_event (GstPad * pad, GstEvent * event)
{
  GstFramedAudioEnc *enc;

  enc = gst_pad_get_element_private (pad);
  g_return_val_if_fail (enc, FALSE);

  GST_LOG_OBJECT (enc->element, "received %s", GST_EVENT_TYPE_NAME (event));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_FLUSH_STOP:
      /* fall-through */
    case GST_EVENT_EOS:
      gst_adapter_clear (enc->adapter);
      enc->next_ts = GST_CLOCK_TIME_NONE;
      break;
    default:
      break;
  }

  return gst_pad_event_default (pad, event);
}

#else
/* included part */

/* parameters:
     RAW_FRAME_SIZE
     CODEC_FRAME_SIZE
     FRAME_DURATION
     AUDIO_SAMPLE_RATE (optional)
   callback:
     codec_get_data(enc, in, out, dtx)

   If one does not mind a few cycles, include'ing can also be replaced by
   a regular include & call, at the expense of some additional parameters
   passed some way or another.
*/

#ifndef AUDIO_SAMPLE_RATE
#define AUDIO_SAMPLE_RATE   (8000)
#endif

/* quite some conditional stuff;
 * the (ugly?) cost of trying to stay inner loop optimal */

static GstFlowReturn
gst_framed_audio_enc_chain (GstFramedAudioEnc * enc, GstBuffer * buf,
    GstPad * srcpad, GstPad ** _cnpad)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GstBuffer *obuf = NULL;
#ifdef CN_PAD
  GstBuffer *cnbuf = NULL;
  GstPad *cnpad = NULL;
#endif
  gboolean discont = FALSE;
  const guint8 *data;
  guint8 *odata;
  gint av, flush, osize;

#ifdef CN_PAD
  GST_OBJECT_LOCK (enc->element);
  if (_cnpad)
    cnpad = *_cnpad;
  if (cnpad)
    gst_object_ref (cnpad);
  GST_OBJECT_UNLOCK (enc->element);
#endif

  if (G_LIKELY (buf)) {
    GST_LOG_OBJECT (enc->element, "input buffer of size %d with ts: %"
        GST_TIME_FORMAT, GST_BUFFER_SIZE (buf),
        GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (buf)));
    discont = GST_BUFFER_FLAG_IS_SET (buf, GST_BUFFER_FLAG_DISCONT);

    /* reposition to the new buffer's timestamp,
     * while correcting for some minor left-over */
    if (GST_BUFFER_TIMESTAMP_IS_VALID (buf)) {
      if (GST_CLOCK_TIME_IS_VALID (enc->next_ts)) {
        GstClockTimeDiff diff, limit;
        GstClockTime tleft;

        tleft = GST_FRAMES_TO_CLOCK_TIME
            (gst_adapter_available (enc->adapter) / 2, AUDIO_SAMPLE_RATE);
        diff =
            GST_CLOCK_DIFF (enc->next_ts + tleft, GST_BUFFER_TIMESTAMP (buf));
        limit = GST_SECOND / AUDIO_SAMPLE_RATE / 2;
        /* try for a perfect stream if possible, do not act on rounding errors */
        if (diff > limit || diff < -limit) {
          enc->next_ts = GST_BUFFER_TIMESTAMP (buf);
          if (enc->next_ts > tleft)
            enc->next_ts -= tleft;
          GST_LOG_OBJECT (enc->element, "marking discont based on timestamps");
          discont = TRUE;
        }
      } else
        enc->next_ts = GST_BUFFER_TIMESTAMP (buf);
    }

    gst_adapter_push (enc->adapter, buf);
    buf = NULL;
  }

  av = gst_adapter_available (enc->adapter);
  if (G_UNLIKELY (av < RAW_FRAME_SIZE))
    goto done;

  data = gst_adapter_peek (enc->adapter, av);
  obuf = gst_buffer_new_and_alloc (av / RAW_FRAME_SIZE * CODEC_FRAME_SIZE);
  odata = GST_BUFFER_DATA (obuf);
  osize = 0;
  flush = 0;

  while (TRUE) {
    gint esize;
#ifdef CN_PAD
    GstDtxDecision dtx;

    /* safe default to start with, should get set */
    dtx = GST_DTX_DECISION_VOICE;
    esize = codec_get_data (enc->element, data + flush, odata, &dtx);
#else
    esize = codec_get_data (enc->element, data + flush, odata, NULL);
#endif

    if (G_UNLIKELY (esize < 0))
      goto encode_failed;

#ifdef CN_PAD
    /* cn in a separate stream */
    switch (dtx) {
      case GST_DTX_DECISION_VOICE:
#endif
        flush += RAW_FRAME_SIZE;
        av -= RAW_FRAME_SIZE;

        odata += esize;
        osize += esize;
#ifdef CN_PAD
        break;
      case GST_DTX_DECISION_SID_UPDATE:
        GST_LOG_OBJECT (enc->element, "dtx: SID_UPDATE %d", esize);
        /* if already data before, need to put SID data separately */
        if (G_UNLIKELY (osize)) {
          cnbuf = gst_buffer_new_and_alloc (esize);
          memcpy (GST_BUFFER_DATA (cnbuf), data + osize, esize);
        } else {
          cnbuf = obuf;
          obuf = NULL;
        }
        /* and send one or both */
        goto send;
        break;
      case GST_DTX_DECISION_SID_NONE:
        GST_LOG_OBJECT (enc->element, "dtx: SID_NONE %d", esize);
        /* maybe send preceding voice, if any */
        goto send;
        break;
    }
#endif

#ifdef CODEC_FRAME_VARIABLE
    /* flush output after insufficient data */
    if (av >= RAW_FRAME_SIZE)
      continue;
#else
    /* ... or some reduced (e.g. silence) frame */
    if (esize >= CODEC_FRAME_SIZE && av >= RAW_FRAME_SIZE)
      continue;
#endif

#ifdef CN_PAD
  send:
#endif
    /* maybe a silent discarded frame */
    if (G_LIKELY (osize)) {
      GST_BUFFER_SIZE (obuf) = osize;
      GST_BUFFER_DURATION (obuf)
          = FRAME_DURATION * (flush / RAW_FRAME_SIZE);
      GST_BUFFER_TIMESTAMP (obuf) = enc->next_ts;
      if (G_UNLIKELY (discont)) {
        GST_BUFFER_FLAG_SET (obuf, GST_BUFFER_FLAG_DISCONT);
        discont = FALSE;
      }

      GST_LOG_OBJECT (enc->element,
          "pushing buffer of size %d with ts: %" GST_TIME_FORMAT,
          GST_BUFFER_SIZE (obuf), GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (obuf)));
      gst_buffer_set_caps (obuf, GST_PAD_CAPS (srcpad));
      ret = gst_pad_push (srcpad, obuf);
      obuf = NULL;
    } else {
      ret = GST_FLOW_OK;
    }

#ifdef CN_PAD
    /* check for stuff to send on cn pad */
    if (cnbuf && cnpad) {
      /* only at most 1 SID update per buffer */
      GST_BUFFER_SIZE (cnbuf) = esize;
      GST_BUFFER_DURATION (cnbuf) = FRAME_DURATION;
      GST_BUFFER_TIMESTAMP (cnbuf) = enc->next_ts;

      GST_LOG_OBJECT (enc->element,
          "pushing cn buffer of size %d with ts: %" GST_TIME_FORMAT,
          GST_BUFFER_SIZE (cnbuf),
          GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (cnbuf)));
      gst_buffer_set_caps (cnbuf, GST_PAD_CAPS (cnpad));
      if (G_LIKELY (ret == GST_FLOW_OK)) {
        ret = gst_pad_push (cnpad, cnbuf);
        /* cn pad may not be linked */
        if (G_UNLIKELY (ret == GST_FLOW_NOT_LINKED))
          ret = GST_FLOW_OK;
      } else
        gst_pad_push (cnpad, cnbuf);
      cnbuf = NULL;
    } else if (G_UNLIKELY (cnbuf)) {
      /* should not occur */
      gst_buffer_unref (cnbuf);
      cnbuf = NULL;
    }

    if (dtx != GST_DTX_DECISION_VOICE) {
      /* still need to count non-voice encoded frame */
      flush += RAW_FRAME_SIZE;
      av -= RAW_FRAME_SIZE;
    }
#endif /* CN_PAD */

    /* remove used part */
    gst_adapter_flush (enc->adapter, flush);
    if (GST_CLOCK_TIME_IS_VALID (enc->next_ts))
      enc->next_ts += FRAME_DURATION * (flush / RAW_FRAME_SIZE);

    /* end if insufficient left or error */
    if (av < RAW_FRAME_SIZE || ret != GST_FLOW_OK)
      break;

    /* allocate new buffer */
    if (!obuf) {
      obuf = gst_buffer_new_and_alloc (av / RAW_FRAME_SIZE * CODEC_FRAME_SIZE);
      odata = GST_BUFFER_DATA (obuf);
      osize = 0;
    }
    /* and prepare to consume again */
    data = gst_adapter_peek (enc->adapter, av);
    flush = 0;
  }

  if (!av) {
    enc->next_ts = GST_CLOCK_TIME_NONE;
  }

done:
#ifdef CN_PAD
  GST_OBJECT_LOCK (enc->element);
  if (cnpad)
    gst_object_unref (cnpad);
  GST_OBJECT_UNLOCK (enc->element);
#endif

  return ret;

  /* ERRORS */
encode_failed:
  {
    GST_ELEMENT_ERROR (enc, STREAM, ENCODE, (NULL), (NULL));
    ret = GST_FLOW_ERROR;
    gst_buffer_unref (obuf);
    goto done;
  }
}
#endif
