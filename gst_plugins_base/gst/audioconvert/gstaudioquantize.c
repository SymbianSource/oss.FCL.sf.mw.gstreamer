/* GStreamer
 * Copyright (C) 2007 Sebastian Dröge <slomo@circular-chaos.org>
 *
 * gstaudioquantize.c: quantizes audio to the target format and optionally
 *                     applies dithering and noise shaping.
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

/*
 * FIXME: When doing dithering with int as intermediate format
 *        one gets audible harmonics while the noise floor is
 *        constant for double as intermediate format!
 */

/* TODO: - Maybe drop 5-pole noise shaping and use coefficients
 *         generated by dmaker
 *         http://shibatch.sf.net
 */

#include <gst/gst.h>
#include <string.h>
#include <math.h>
#include "audioconvert.h"
#include "gstaudioquantize.h"

#define MAKE_QUANTIZE_FUNC_NAME(name)                                   \
gst_audio_quantize_quantize_##name

/* Quantize functions for gint32 as intermediate format */

#define MAKE_QUANTIZE_FUNC_I(name, DITHER_INIT_FUNC, ADD_DITHER_FUNC,   \
                             ROUND_FUNC)                                \
static void                                                             \
MAKE_QUANTIZE_FUNC_NAME (name) (AudioConvertCtx *ctx, gint32 *src,      \
                                gint32 *dst, gint count)                \
{                                                                       \
  gint scale = ctx->out_scale;                                          \
  gint channels = ctx->out.channels;                                    \
  gint chan_pos;                                                        \
                                                                        \
  if (scale > 0) {                                                      \
    gint32 tmp;                                                         \
    guint32 mask = 0xffffffff & (0xffffffff << scale);                  \
    guint32 bias = 1U << (scale - 1);                                   \
    DITHER_INIT_FUNC()                                                  \
                                                                        \
    for (;count;count--) {                                              \
      for (chan_pos = 0; chan_pos < channels; chan_pos++) {             \
        tmp = *src++;                                                   \
        ADD_DITHER_FUNC()                                               \
        ROUND_FUNC()                                                    \
        *dst = tmp & mask;                                              \
        dst++;                                                          \
      }                                                                 \
    }                                                                   \
  } else {                                                              \
    for (;count;count--) {                                              \
      for (chan_pos = 0; chan_pos < channels; chan_pos++) {             \
        *dst = *src++;                                                  \
        dst++;                                                          \
      }                                                                 \
    }                                                                   \
  }                                                                     \
}


/* Quantize functions for gdouble as intermediate format with
 * int as target */

#define MAKE_QUANTIZE_FUNC_F(name, DITHER_INIT_FUNC, NS_INIT_FUNC,      \
                             ADD_NS_FUNC, ADD_DITHER_FUNC,              \
                             UPDATE_ERROR_FUNC)                         \
static void                                                             \
MAKE_QUANTIZE_FUNC_NAME (name) (AudioConvertCtx *ctx, gdouble *src,     \
                                gdouble *dst, gint count)               \
{                                                                       \
  gint scale = ctx->out_scale;                                          \
  gint channels = ctx->out.channels;                                    \
  gint chan_pos;                                                        \
  gdouble factor = (1U<<(32-scale-1)) - 1;                              \
                                                                        \
  if (scale > 0) {                                                      \
    gdouble tmp;                                                        \
    DITHER_INIT_FUNC()                                                  \
    NS_INIT_FUNC()                                                      \
                                                                        \
    for (;count;count--) {                                              \
      for (chan_pos = 0; chan_pos < channels; chan_pos++) {             \
        tmp = *src++;                                                   \
        ADD_NS_FUNC()                                                   \
        ADD_DITHER_FUNC()                                               \
        tmp = floor(tmp * factor + 0.5);                                \
        *dst = CLAMP (tmp, -factor - 1, factor);                        \
        UPDATE_ERROR_FUNC()                                             \
        dst++;                                                          \
      }                                                                 \
    }                                                                   \
  } else {                                                              \
    for (;count;count--) {                                              \
      for (chan_pos = 0; chan_pos < channels; chan_pos++) {             \
        *dst = *src++ * 2147483647.0;                                   \
        dst++;                                                          \
      }                                                                 \
    }                                                                   \
  }                                                                     \
}

/* Rounding functions for int as intermediate format, only used when
 * not using dithering. With dithering we include this offset in our
 * dither noise instead. */

#define ROUND()                                                         \
        if (tmp > 0 && G_MAXINT32 - tmp <= bias)                        \
          tmp = G_MAXINT32;                                             \
        else                                                            \
          tmp += bias;


#define NONE_FUNC()

/* Dithering definitions
 * See http://en.wikipedia.org/wiki/Dithering or
 * http://www.cadenzarecording.com/Dither.html for explainations.
 *
 * We already add the rounding offset to the dither noise here
 * to have only one overflow check instead of two. */

#define INIT_DITHER_RPDF_I()                                            \
  gint32 rand;                                                          \
  gint32 dither = (1<<(scale));

#define ADD_DITHER_RPDF_I()                                             \
        rand = g_rand_int_range (ctx->dither_random, bias - dither,     \
               bias + dither);                                          \
        if (rand > 0 && tmp > 0 && G_MAXINT32 - tmp <= rand)            \
                tmp = G_MAXINT32;                                       \
        else if (rand < 0 && tmp < 0 && G_MININT32 - tmp >= rand)       \
                tmp = G_MININT32;                                       \
        else                                                            \
                tmp += rand;

#define INIT_DITHER_RPDF_F()                                            \
  gdouble dither = 1.0/(1U<<(32 - scale - 1));

#define ADD_DITHER_RPDF_F()                                             \
        tmp += g_rand_double_range (ctx->dither_random, - dither,       \
               dither);

#define INIT_DITHER_TPDF_I()                                            \
  gint32 rand;                                                          \
  gint32 dither = (1<<(scale - 1));                                     \
  bias = bias >> 1;

#define ADD_DITHER_TPDF_I()                                             \
        rand = g_rand_int_range (ctx->dither_random, bias - dither,     \
               bias + dither - 1)                                           \
               + g_rand_int_range (ctx->dither_random, bias - dither,   \
               bias + dither - 1);                                          \
        if (rand > 0 && tmp > 0 && G_MAXINT32 - tmp <= rand)            \
                tmp = G_MAXINT32;                                       \
        else if (rand < 0 && tmp < 0 && G_MININT32 - tmp >= rand)       \
                tmp = G_MININT32;                                       \
        else                                                            \
                tmp += rand;

#define INIT_DITHER_TPDF_F()                                            \
  gdouble dither = 1.0/(1U<<(32 - scale));

#define ADD_DITHER_TPDF_F()                                             \
        tmp += g_rand_double_range (ctx->dither_random, - dither,       \
               dither)                                                  \
               + g_rand_double_range (ctx->dither_random, - dither,     \
               dither);

#define INIT_DITHER_TPDF_HF_I()                                         \
  gint32 rand;                                                          \
  gint32 dither = (1<<(scale-1));                                       \
  gint32 *last_random = (gint32 *) ctx->last_random, tmp_rand;          \
  bias = bias >> 1;

#define ADD_DITHER_TPDF_HF_I()                                          \
        tmp_rand = g_rand_int_range (ctx->dither_random, bias - dither, \
                   bias + dither); \
        rand = tmp_rand - last_random[chan_pos];                        \
        last_random[chan_pos] = tmp_rand;                               \
        if (rand > 0 && tmp > 0 && G_MAXINT32 - tmp <= rand)            \
                tmp = G_MAXINT32;                                       \
        else if (rand < 0 && tmp < 0 && G_MININT32 - tmp >= rand)       \
                tmp = G_MININT32;                                       \
        else                                                            \
                tmp += rand;

/* Like TPDF dither but the dither noise is oriented more to the
 * higher frequencies */

#define INIT_DITHER_TPDF_HF_F()                                         \
  gdouble rand;                                                         \
  gdouble dither = 1.0/(1U<<(32 - scale));                              \
  gdouble *last_random = (gdouble *) ctx->last_random, tmp_rand;

#define ADD_DITHER_TPDF_HF_F()                                          \
        tmp_rand = g_rand_double_range (ctx->dither_random, - dither,   \
                   dither);                                             \
        rand = tmp_rand - last_random[chan_pos];                        \
        last_random[chan_pos] = tmp_rand;                               \
        tmp += rand;

/* Noise shaping definitions.
 * See http://en.wikipedia.org/wiki/Noise_shaping for explanations. */


/* Simple error feedback: Just accumulate the dithering and quantization
 * error and remove it from each sample. */

#define INIT_NS_ERROR_FEEDBACK()                                        \
  gdouble orig;                                                         \
  gdouble *errors = ctx->error_buf;

#define ADD_NS_ERROR_FEEDBACK()                                         \
        orig = tmp;                                                     \
        tmp -= errors[chan_pos];

#define UPDATE_ERROR_ERROR_FEEDBACK()                                   \
        errors[chan_pos] += (*dst)/factor - orig;

/* Same as error feedback but also add 1/2 of the previous error value.
 * This moves the noise a bit more into the higher frequencies. */

#define INIT_NS_SIMPLE()                                                \
  gdouble orig;                                                         \
  gdouble *errors = ctx->error_buf, cur_error;

#define ADD_NS_SIMPLE()                                                 \
        cur_error = errors[chan_pos*2] - 0.5 * errors[chan_pos*2 + 1];  \
        tmp -= cur_error;                                               \
        orig = tmp;

#define UPDATE_ERROR_SIMPLE()                                           \
        errors[chan_pos*2 + 1] = errors[chan_pos*2];                    \
        errors[chan_pos*2] = (*dst)/factor - orig;


/* Noise shaping coefficients from[1], moves most power of the
 * error noise into inaudible frequency ranges.
 *
 * [1]
 * "Minimally Audible Noise Shaping", Stanley P. Lipshitz,
 * John Vanderkooy, and Robert A. Wannamaker,
 * J. Audio Eng. Soc., Vol. 39, No. 11, November 1991. */

static const gdouble ns_medium_coeffs[] = {
  2.033, -2.165, 1.959, -1.590, 0.6149
};

#define INIT_NS_MEDIUM()                                                \
  gdouble orig;                                                         \
  gdouble *errors = ctx->error_buf, cur_error;                          \
  int j;

#define ADD_NS_MEDIUM()                                                 \
        cur_error = 0.0;                                                \
        for (j = 0; j < 5; j++)                                         \
          cur_error += errors[chan_pos*5 + j] * ns_medium_coeffs[j];    \
        tmp -= cur_error;                                               \
        orig = tmp;

#define UPDATE_ERROR_MEDIUM()                                           \
        for (j = 4; j > 0; j--)                                         \
          errors[chan_pos*5 + j] = errors[chan_pos*5 + j-1];            \
        errors[chan_pos*5] = (*dst)/factor - orig;

/* Noise shaping coefficients by David Schleef, moves most power of the
 * error noise into inaudible frequency ranges */

static const gdouble ns_high_coeffs[] = {
  2.08484, -2.92975, 3.27918, -3.31399, 2.61339, -1.72008, 0.876066, -0.340122
};

#define INIT_NS_HIGH()                                                  \
  gdouble orig;                                                         \
  gdouble *errors = ctx->error_buf, cur_error;                          \
  int j;

#define ADD_NS_HIGH()                                                   \
        cur_error = 0.0;                                                \
        for (j = 0; j < 8; j++)                                         \
          cur_error += errors[chan_pos + j] * ns_high_coeffs[j];          \
        tmp -= cur_error;                                               \
        orig = tmp;

#define UPDATE_ERROR_HIGH()                                             \
        for (j = 7; j > 0; j--)                                         \
          errors[chan_pos + j] = errors[chan_pos + j-1];                \
        errors[chan_pos] = (*dst)/factor - orig;


MAKE_QUANTIZE_FUNC_I (signed_none_none, NONE_FUNC, NONE_FUNC, ROUND);
MAKE_QUANTIZE_FUNC_I (signed_rpdf_none, INIT_DITHER_RPDF_I, ADD_DITHER_RPDF_I,
    NONE_FUNC);
MAKE_QUANTIZE_FUNC_I (signed_tpdf_none, INIT_DITHER_TPDF_I, ADD_DITHER_TPDF_I,
    NONE_FUNC);
MAKE_QUANTIZE_FUNC_I (signed_tpdf_hf_none, INIT_DITHER_TPDF_HF_I,
    ADD_DITHER_TPDF_HF_I, NONE_FUNC);

MAKE_QUANTIZE_FUNC_I (unsigned_none_none, NONE_FUNC, NONE_FUNC, ROUND);
MAKE_QUANTIZE_FUNC_I (unsigned_rpdf_none, INIT_DITHER_RPDF_I, ADD_DITHER_RPDF_I,
    NONE_FUNC);
MAKE_QUANTIZE_FUNC_I (unsigned_tpdf_none, INIT_DITHER_TPDF_I, ADD_DITHER_TPDF_I,
    NONE_FUNC);
MAKE_QUANTIZE_FUNC_I (unsigned_tpdf_hf_none, INIT_DITHER_TPDF_HF_I,
    ADD_DITHER_TPDF_HF_I, NONE_FUNC);

MAKE_QUANTIZE_FUNC_F (float_none_error_feedback, NONE_FUNC,
    INIT_NS_ERROR_FEEDBACK, ADD_NS_ERROR_FEEDBACK, NONE_FUNC,
    UPDATE_ERROR_ERROR_FEEDBACK);
MAKE_QUANTIZE_FUNC_F (float_none_simple, NONE_FUNC, INIT_NS_SIMPLE,
    ADD_NS_SIMPLE, NONE_FUNC, UPDATE_ERROR_SIMPLE);
MAKE_QUANTIZE_FUNC_F (float_none_medium, NONE_FUNC, INIT_NS_MEDIUM,
    ADD_NS_MEDIUM, NONE_FUNC, UPDATE_ERROR_MEDIUM);
MAKE_QUANTIZE_FUNC_F (float_none_high, NONE_FUNC, INIT_NS_HIGH, ADD_NS_HIGH,
    NONE_FUNC, UPDATE_ERROR_HIGH);

MAKE_QUANTIZE_FUNC_F (float_rpdf_error_feedback, INIT_DITHER_RPDF_F,
    INIT_NS_ERROR_FEEDBACK, ADD_NS_ERROR_FEEDBACK, ADD_DITHER_RPDF_F,
    UPDATE_ERROR_ERROR_FEEDBACK);
MAKE_QUANTIZE_FUNC_F (float_rpdf_simple, INIT_DITHER_RPDF_F, INIT_NS_SIMPLE,
    ADD_NS_SIMPLE, ADD_DITHER_RPDF_F, UPDATE_ERROR_SIMPLE);
MAKE_QUANTIZE_FUNC_F (float_rpdf_medium, INIT_DITHER_RPDF_F, INIT_NS_MEDIUM,
    ADD_NS_MEDIUM, ADD_DITHER_RPDF_F, UPDATE_ERROR_MEDIUM);
MAKE_QUANTIZE_FUNC_F (float_rpdf_high, INIT_DITHER_RPDF_F, INIT_NS_HIGH,
    ADD_NS_HIGH, ADD_DITHER_RPDF_F, UPDATE_ERROR_HIGH);

MAKE_QUANTIZE_FUNC_F (float_tpdf_error_feedback, INIT_DITHER_TPDF_F,
    INIT_NS_ERROR_FEEDBACK, ADD_NS_ERROR_FEEDBACK, ADD_DITHER_TPDF_F,
    UPDATE_ERROR_ERROR_FEEDBACK);
MAKE_QUANTIZE_FUNC_F (float_tpdf_simple, INIT_DITHER_TPDF_F, INIT_NS_SIMPLE,
    ADD_NS_SIMPLE, ADD_DITHER_TPDF_F, UPDATE_ERROR_SIMPLE);
MAKE_QUANTIZE_FUNC_F (float_tpdf_medium, INIT_DITHER_TPDF_F, INIT_NS_MEDIUM,
    ADD_NS_MEDIUM, ADD_DITHER_TPDF_F, UPDATE_ERROR_MEDIUM);
MAKE_QUANTIZE_FUNC_F (float_tpdf_high, INIT_DITHER_TPDF_F, INIT_NS_HIGH,
    ADD_NS_HIGH, ADD_DITHER_TPDF_F, UPDATE_ERROR_HIGH);

MAKE_QUANTIZE_FUNC_F (float_tpdf_hf_error_feedback, INIT_DITHER_TPDF_HF_F,
    INIT_NS_ERROR_FEEDBACK, ADD_NS_ERROR_FEEDBACK, ADD_DITHER_TPDF_HF_F,
    UPDATE_ERROR_ERROR_FEEDBACK);
MAKE_QUANTIZE_FUNC_F (float_tpdf_hf_simple, INIT_DITHER_TPDF_HF_F,
    INIT_NS_SIMPLE, ADD_NS_SIMPLE, ADD_DITHER_TPDF_HF_F, UPDATE_ERROR_SIMPLE);
MAKE_QUANTIZE_FUNC_F (float_tpdf_hf_medium, INIT_DITHER_TPDF_HF_F,
    INIT_NS_MEDIUM, ADD_NS_MEDIUM, ADD_DITHER_TPDF_HF_F, UPDATE_ERROR_MEDIUM);
MAKE_QUANTIZE_FUNC_F (float_tpdf_hf_high, INIT_DITHER_TPDF_HF_F, INIT_NS_HIGH,
    ADD_NS_HIGH, ADD_DITHER_TPDF_HF_F, UPDATE_ERROR_HIGH);

static AudioConvertQuantize quantize_funcs[] = {
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (signed_none_none),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (signed_rpdf_none),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (signed_tpdf_none),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (signed_tpdf_hf_none),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (unsigned_none_none),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (unsigned_rpdf_none),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (unsigned_tpdf_none),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (unsigned_tpdf_hf_none),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_none_error_feedback),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_none_simple),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_none_medium),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_none_high),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_rpdf_error_feedback),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_rpdf_simple),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_rpdf_medium),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_rpdf_high),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_tpdf_error_feedback),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_tpdf_simple),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_tpdf_medium),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_tpdf_high),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_tpdf_hf_error_feedback),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_tpdf_hf_simple),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_tpdf_hf_medium),
  (AudioConvertQuantize) MAKE_QUANTIZE_FUNC_NAME (float_tpdf_hf_high)
};

static void
gst_audio_quantize_setup_noise_shaping (AudioConvertCtx * ctx)
{
  switch (ctx->ns) {
    case NOISE_SHAPING_HIGH:{
      ctx->error_buf = g_new0 (gdouble, ctx->out.channels * 8);
      break;
    }
    case NOISE_SHAPING_MEDIUM:{
      ctx->error_buf = g_new0 (gdouble, ctx->out.channels * 5);
      break;
    }
    case NOISE_SHAPING_SIMPLE:{
      ctx->error_buf = g_new0 (gdouble, ctx->out.channels * 2);
      break;
    }
    case NOISE_SHAPING_ERROR_FEEDBACK:
      ctx->error_buf = g_new0 (gdouble, ctx->out.channels);
      break;
    case NOISE_SHAPING_NONE:
    default:
      ctx->error_buf = NULL;
      break;
  }
  return;
}

static void
gst_audio_quantize_free_noise_shaping (AudioConvertCtx * ctx)
{
  switch (ctx->ns) {
    case NOISE_SHAPING_HIGH:
    case NOISE_SHAPING_MEDIUM:
    case NOISE_SHAPING_SIMPLE:
    case NOISE_SHAPING_ERROR_FEEDBACK:
    case NOISE_SHAPING_NONE:
    default:
      break;
  }
  return;

  g_free (ctx->error_buf);
  ctx->error_buf = NULL;
  return;
}

static void
gst_audio_quantize_setup_dither (AudioConvertCtx * ctx)
{
  switch (ctx->dither) {
    case DITHER_TPDF_HF:
      if (ctx->out.is_int)
        ctx->last_random = g_new0 (gint32, ctx->out.channels);
      else
        ctx->last_random = g_new0 (gdouble, ctx->out.channels);
      ctx->dither_random = g_rand_new ();
      break;
    case DITHER_RPDF:
    case DITHER_TPDF:
      ctx->dither_random = g_rand_new ();
      ctx->last_random = NULL;
      break;
    case DITHER_NONE:
    default:
      ctx->dither_random = NULL;
      ctx->last_random = NULL;
      break;
  }
  return;
}

static void
gst_audio_quantize_free_dither (AudioConvertCtx * ctx)
{
  g_free (ctx->last_random);
  if (ctx->dither_random)
    g_rand_free (ctx->dither_random);

  return;
}

static void
gst_audio_quantize_setup_quantize_func (AudioConvertCtx * ctx)
{
  gint index = 0;

  if (!ctx->out.is_int) {
    ctx->quantize = NULL;
    return;
  }

  if (ctx->ns == NOISE_SHAPING_NONE) {
    index += ctx->dither;
    index += (ctx->out.sign) ? 0 : 4;
  } else {
    index += 8 + (4 * ctx->dither);
    index += ctx->ns - 1;
  }

  ctx->quantize = quantize_funcs[index];
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


gboolean
gst_audio_quantize_setup (AudioConvertCtx * ctx)
{
  gst_audio_quantize_setup_dither (ctx);
  gst_audio_quantize_setup_noise_shaping (ctx);
  gst_audio_quantize_setup_quantize_func (ctx);

  return TRUE;
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
gst_audio_quantize_free (AudioConvertCtx * ctx)
{
  gst_audio_quantize_free_dither (ctx);
  gst_audio_quantize_free_noise_shaping (ctx);
}
