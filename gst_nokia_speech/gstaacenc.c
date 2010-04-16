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

/**
 * SECTION:element-nokiaaacenc
 * @seealso: nokiaaacdec
 *
 * nokiaaacenc encodes raw audio to AAC streams.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <string.h>

#include "gstaacenc.h"

GST_DEBUG_CATEGORY_STATIC (aac_enc);
#define GST_CAT_DEFAULT aac_enc

enum
{
  AAC_PROFILE_AUTO = 0,
  AAC_PROFILE_LC = 2,
  AAC_PROFILE_HE = 5
};

#define GST_TYPE_AAC_ENC_PROFILE (gst_aac_enc_profile_get_type ())
static GType
gst_aac_enc_profile_get_type (void)
{
  static GType gst_aac_enc_profile_type = 0;

  if (!gst_aac_enc_profile_type) {
    static GEnumValue gst_aac_enc_profile[] = {
      {AAC_PROFILE_AUTO, "Codec selects LC or HE", "AUTO"},
      {AAC_PROFILE_LC, "Low complexity profile", "LC"},
      {AAC_PROFILE_HE, "High Efficiency", "HE"},
      {0, NULL, NULL},
    };

    gst_aac_enc_profile_type = g_enum_register_static ("GstNokiaAacEncProfile",
        gst_aac_enc_profile);
  }

  return gst_aac_enc_profile_type;
}

#define GST_TYPE_AAC_ENC_OUTPUTFORMAT (gst_aac_enc_outputformat_get_type ())
static GType
gst_aac_enc_outputformat_get_type (void)
{
  static GType gst_aac_enc_outputformat_type = 0;

  if (!gst_aac_enc_outputformat_type) {
    static GEnumValue gst_aac_enc_outputformat[] = {
      {RAW, "AAC Raw format", "RAW"},
      {USE_ADTS, "Audio Data Transport Stream format", "ADTS"},
      {USE_ADIF, "Audio Data Interchange Format", "ADIF"},
      {0, NULL, NULL},
    };

    gst_aac_enc_outputformat_type =
        g_enum_register_static ("GstNokiaAacEncOutputFormat",
        gst_aac_enc_outputformat);
  }

  return gst_aac_enc_outputformat_type;
}

enum
{
  PROP_0,
  PROP_BITRATE,
  PROP_PROFILE,
  PROP_FORMAT
};

static GstStaticPadTemplate gst_aac_enc_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw-int, "
        "endianness = (int) BYTE_ORDER, "
        "signed = (bool) TRUE, "
        "width = (int) 16, "
        "depth = (int) 16, "
        "rate = (int) [ 8000, 96000 ],  channels = (int) [ 1, 2 ] ")
    );

static GstStaticPadTemplate gst_aac_enc_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/mpeg, " "mpegversion = (int) 4, "
        "rate = (int) [ 8000, 96000 ],  channels = (int) [ 1, 2 ] ")
    );

static void gst_aac_enc_base_init (gpointer g_class);
static void gst_aac_enc_class_init (GstAACEncClass * klass);
static void gst_aac_enc_init (GstAACEnc * filter, GstAACEncClass * klass);

static void gst_aac_enc_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_aac_enc_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static void gst_aac_enc_finalize (GObject * object);
static void gst_aac_enc_reset (GstAACEnc * enc);
static GstStateChangeReturn gst_aac_enc_change_state (GstElement * element,
    GstStateChange transition);
static gboolean gst_aac_enc_sink_setcaps (GstPad * pad, GstCaps * caps);
static GstFlowReturn gst_aac_enc_chain (GstPad * pad, GstBuffer * buffer);

GST_BOILERPLATE (GstNokiaAACEnc, gst_aac_enc, GstElement, GST_TYPE_ELEMENT);

static void
gst_aac_enc_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_set_details_simple (element_class,
      "Nokia AAC encoder", "Codec/Encoder/Audio",
      "Nokia AAC encoder",
      "MCC, Mark Nauwelaerts <mark.nauwelaerts@collabora.co.uk>");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_aac_enc_src_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_aac_enc_sink_template));
}

/* initialize the plugin's class */
static void
gst_aac_enc_class_init (GstAACEncClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  GST_DEBUG_CATEGORY_INIT (aac_enc, "nokiaaacenc", 0, "Nokia AAC encoder");

  gobject_class->set_property = gst_aac_enc_set_property;
  gobject_class->get_property = gst_aac_enc_get_property;
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_aac_enc_finalize);

  /* properties */
  g_object_class_install_property (gobject_class, PROP_BITRATE,
      g_param_spec_int ("bitrate", "Bitrate (bps)", "Bitrate in bits/sec",
          8 * 1000, 320 * 1000, 128 * 1000,
          (GParamFlags) (G_PARAM_READWRITE | G_PARAM_CONSTRUCT)));
  g_object_class_install_property (gobject_class, PROP_PROFILE,
      g_param_spec_enum ("profile", "Profile",
          "MPEG/AAC encoding profile",
          GST_TYPE_AAC_ENC_PROFILE, AAC_PROFILE_LC,
          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  g_object_class_install_property (gobject_class, PROP_FORMAT,
      g_param_spec_enum ("output-format", "Output format",
          "Format of output frames",
          GST_TYPE_AAC_ENC_OUTPUTFORMAT, RAW,
          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_aac_enc_change_state);
}

static void
gst_aac_enc_init (GstAACEnc * enc, GstAACEncClass * klass)
{
  enc->sinkpad =
      gst_pad_new_from_static_template (&gst_aac_enc_sink_template, "sink");
  gst_pad_set_setcaps_function (enc->sinkpad,
      GST_DEBUG_FUNCPTR (gst_aac_enc_sink_setcaps));
  gst_pad_set_chain_function (enc->sinkpad,
      GST_DEBUG_FUNCPTR (gst_aac_enc_chain));
  gst_element_add_pad (GST_ELEMENT (enc), enc->sinkpad);

  enc->srcpad =
      gst_pad_new_from_static_template (&gst_aac_enc_src_template, "src");
  gst_pad_use_fixed_caps (enc->srcpad);
  gst_element_add_pad (GST_ELEMENT (enc), enc->srcpad);

#ifndef GST_DISABLE_GST_DEBUG
  gst_framed_audio_enc_init (&enc->enc, GST_ELEMENT (enc), aac_enc);
#else
  gst_framed_audio_enc_init (&enc->enc, GST_ELEMENT (enc), NULL);
#endif

  gst_aac_enc_reset (enc);
}

static void
gst_aac_enc_reset (GstAACEnc * enc)
{
  gst_framed_audio_enc_reset (&enc->enc);
  if (enc->encoder)
    EnAACPlus_Enc_Delete (enc->encoder);
  enc->encoder = NULL;
  g_free (enc->buffer);
  enc->buffer = NULL;
}

static void
gst_aac_enc_finalize (GObject * object)
{
  GstAACEnc *enc = (GstAACEnc *) object;

  gst_framed_audio_enc_finalize (&enc->enc);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
gst_aac_enc_setup_encoder (GstAACEnc * enc)
{
  AACPLUS_ENC_CONFIG enc_params;
  AACPLUS_ENC_MODE mode;
  gint rate, channels;
  guint maxbitrate;

  rate = enc->rate;
  channels = enc->channels;

  /* only up to 2 channels supported */
  enc_params.sampleRate = rate;
  enc_params.bitRate = enc->bitrate;
  enc_params.nChannels = channels;
  enc_params.aac_tools = USE_ALL;
  enc_params.pcm_mode = 16;
  enc_params.format = enc->format;

  /* check, warn and correct if the max bitrate for the given samplerate is
   * exceeded. Maximum of 6144 bit for a channel */
  maxbitrate =
      (guint) (6144.0 * (gdouble) rate / (gdouble) 1024.0 + .5) * channels;
  if (enc_params.bitRate > maxbitrate) {
    GST_ELEMENT_INFO (enc, RESOURCE, SETTINGS, (NULL),
        ("bitrate %d exceeds maximum allowed bitrate of %d for samplerate %d "
            "and %d channels.  Setting bitrate to %d",
            enc_params.bitRate, maxbitrate, rate, channels, maxbitrate));
    enc_params.bitRate = maxbitrate;
  }

  /* set up encoder */
  if (enc->encoder)
    EnAACPlus_Enc_Delete (enc->encoder);

  /* only these profiles are really known to and supported by codec */
  switch (enc->profile) {
    case AAC_PROFILE_LC:
      mode = MODE_AACLC;
      break;
    case AAC_PROFILE_HE:
      mode = MODE_EAACPLUS;
      break;
    case AAC_PROFILE_AUTO:
      mode = MODE_AUTO;
      break;
    default:
      mode = MODE_AACLC;
      g_assert_not_reached ();
      break;
  }
  enc->encoder = EnAACPlus_Enc_Create (&enc_params, mode);

  if (!enc->encoder)
    goto setup_failed;

  /* query and setup params,
   * also set up some buffers for fancy HE */
  EnAACPlus_Enc_GetSetParam (enc->encoder, &enc->info);

#define DUMP_FIELD(f)  \
  GST_DEBUG_OBJECT (enc, "encoder info: " G_STRINGIFY (f) " = %d", enc->info.f);

  DUMP_FIELD (InBufSize);
  DUMP_FIELD (OutBufSize);
  DUMP_FIELD (Frame_Size);
  DUMP_FIELD (writeOffset);
  DUMP_FIELD (InBufSize);

  enc->raw_frame_size = enc->info.Frame_Size;
  enc->codec_frame_size = enc->info.OutBufSize;
  enc->frame_duration =
      GST_FRAMES_TO_CLOCK_TIME (enc->raw_frame_size / enc->channels / 2,
      enc->rate);

  g_free (enc->buffer);
  /* safety margin */
  enc->buffer = g_malloc (enc->info.InBufSize * 2);

  return TRUE;

  /* ERRORS */
setup_failed:
  {
    GST_ELEMENT_ERROR (enc, LIBRARY, SETTINGS, (NULL), (NULL));
    return FALSE;
  }
}

static gint
gst_aac_enc_rate_idx (gint rate)
{
  static int rates[] = {
    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025,
    8000, 7350
  };
  guint i;

  for (i = 0; i < G_N_ELEMENTS (rates); ++i)
    if (rates[i] == rate)
      return i;

  return 0xF;
}

static gboolean
gst_aac_enc_sink_setcaps (GstPad * pad, GstCaps * caps)
{
  GstAACEnc *enc;
  gboolean ret = TRUE;
  GstStructure *s;
  GstBuffer *buf = NULL;
  gint rate, channels;

  enc = GST_AAC_ENC (GST_PAD_PARENT (pad));

  /* extract stream properties */
  s = gst_caps_get_structure (caps, 0);

  if (!s)
    goto refuse_caps;

  ret = gst_structure_get_int (s, "rate", &rate);
  ret &= gst_structure_get_int (s, "channels", &channels);

  if (!ret)
    goto refuse_caps;

  enc->rate = rate;
  enc->channels = channels;

  /* NOTE:
   * - codec only supports LC or HE (= LC + SBR etc)
   * - HE has (more) restrictive samplerate/channels/bitrate combination
   * - AUTO makes codec select between LC or HE (depending on settings)
   */

  gst_aac_enc_setup_encoder (enc);
  if (!enc->encoder)
    return FALSE;

  /* HE iff writeOffset <> 0 iff Frame_Size <> 1024 * 2 * channels */
  if (enc->info.writeOffset)
    rate /= 2;

  /* create codec_data if raw output */
  if (enc->format == RAW) {
    gint rate_idx;
    guint8 *data;

    buf = gst_buffer_new_and_alloc (5);
    data = GST_BUFFER_DATA (buf);
    rate_idx = gst_aac_enc_rate_idx (rate);

    GST_DEBUG_OBJECT (enc, "codec_data: profile=%d, sri=%d, channels=%d",
        enc->profile, rate_idx, enc->channels);

    /* always write LC profile, and use implicit signaling for HE SBR */
    data[0] = ((2 & 0x1F) << 3) | ((rate_idx & 0xE) >> 1);
    data[1] = ((rate_idx & 0x1) << 7);
    if (rate_idx != 0x0F) {
      data[1] |= ((channels & 0xF) << 3);
      GST_BUFFER_SIZE (buf) = 2;
    } else {
      gint srate;

      srate = rate << 7;
      data[1] |= ((srate >> 24) & 0xFF);
      data[2] = ((srate >> 16) & 0xFF);
      data[3] = ((srate >> 8) & 0xFF);
      data[4] = (srate & 0xFF);
      data[4] |= ((channels & 0xF) << 3);
      GST_BUFFER_SIZE (buf) = 5;
    }
  }

  /* fix some in src template */
  caps = gst_caps_copy (gst_pad_get_pad_template_caps (enc->srcpad));
  gst_caps_set_simple (caps, "rate", G_TYPE_INT, rate,
      "channels", G_TYPE_INT, channels, NULL);
  if (buf) {
    gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, buf, NULL);
    gst_buffer_unref (buf);
  }
  ret = gst_pad_set_caps (enc->srcpad, caps);
  gst_caps_unref (caps);

  return ret;

  /* ERRORS */
refuse_caps:
  {
    GST_WARNING_OBJECT (enc, "refused caps %" GST_PTR_FORMAT, caps);
    return FALSE;
  }
}

static gint
gst_aac_enc_get_data (GstElement * element, const guint8 * in, guint8 * out,
    GstDtxDecision * dtx)
{
  GstAACEnc *enc;
  gint res;
  gint offset;
  UWord32 used, encoded;
  Word8 *inbuffer;

  enc = GST_AAC_ENC_CAST (element);

  offset = enc->info.writeOffset;
  if (offset) {
    memcpy (enc->buffer + offset, in, enc->raw_frame_size);
    inbuffer = (Word8 *) enc->buffer;
  } else {
    inbuffer = (Word8 *) in;
  }

  res = EnAACPlus_Enc_Encode (enc->encoder, &enc->info, inbuffer, &used,
      (UWord8 *) out, &encoded);

  if (offset) {
    memcpy (enc->buffer, enc->buffer + used, offset);
  }

  return res == 0 ? encoded : -1;
}

/* set parameters */
#define AUDIO_SAMPLE_RATE   ((GST_AAC_ENC (enc->element))->rate)
#define RAW_FRAME_SIZE      ((GST_AAC_ENC (enc->element))->raw_frame_size)
/* safe maximum frame size */
#define CODEC_FRAME_SIZE    ((GST_AAC_ENC (enc->element))->codec_frame_size)
/* do not set variable frame;
 * this will make every frame act as a silence frame and force output */
/* #define CODEC_FRAME_VARIABLE 1 */
#define FRAME_DURATION      ((GST_AAC_ENC (enc->element))->frame_duration)
#define codec_get_data(enc, in, out, dtx)  \
    gst_aac_enc_get_data (enc, in, out, dtx)

/* and include code */
#include "gstframedaudioenc.c"

static GstFlowReturn
gst_aac_enc_chain (GstPad * pad, GstBuffer * buf)
{
  GstAACEnc *enc;

  enc = GST_AAC_ENC (GST_PAD_PARENT (pad));

  if (G_UNLIKELY (enc->encoder == NULL))
    goto not_negotiated;

  return gst_framed_audio_enc_chain (&enc->enc, buf, enc->srcpad, &enc->cnpad);

  /* ERRORS */
not_negotiated:
  {
    GST_ELEMENT_ERROR (enc, CORE, NEGOTIATION, (NULL),
        ("format wasn't negotiated before chain function"));
    gst_buffer_unref (buf);
    return GST_FLOW_NOT_NEGOTIATED;
  }
}

static void
gst_aac_enc_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstAACEnc *enc;

  enc = GST_AAC_ENC (object);

  switch (prop_id) {
    case PROP_BITRATE:
      enc->bitrate = g_value_get_int (value);
      break;
    case PROP_PROFILE:
      enc->profile = g_value_get_enum (value);
      break;
    case PROP_FORMAT:
      enc->format = g_value_get_enum (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_aac_enc_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstAACEnc *enc;

  enc = GST_AAC_ENC (object);

  switch (prop_id) {
    case PROP_BITRATE:
      g_value_set_int (value, enc->bitrate);
      break;
    case PROP_PROFILE:
      g_value_set_enum (value, enc->profile);
      break;
    case PROP_FORMAT:
      g_value_set_enum (value, enc->format);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstStateChangeReturn
gst_aac_enc_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstAACEnc *enc = GST_AAC_ENC (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      gst_aac_enc_reset (enc);
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }

  return ret;
}

static gboolean
plugin_init (GstPlugin * plugin)
{

  if (!gst_element_register (plugin, "nokiaaacenc", GST_RANK_SECONDARY,
          GST_TYPE_AAC_ENC))
    return FALSE;

  return TRUE;
}

/* this is the structure that gst-register looks for
 * so keep the name plugin_desc, or you cannot get your plug-in registered */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "nokiaaacenc",
    "Nokia AAC MCC codec",
    plugin_init, VERSION, "Proprietary", "gst-nokia-speech", "")

EXPORT_C GstPluginDesc* _GST_PLUGIN_DESC()
   {
      return &gst_plugin_desc;
   }

