/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "gstg711decoderinterface.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif

static guint g711_decoder_count = 0;
static void gst_g711_decoder_base_init (gpointer g_class);
static void gst_g711_decoder_base_finalize (gpointer g_class);
             
EXPORT_C GType gst_g711_decoder_get_type(void)
{
  static GType g711_decoder_type = 0;

  if (g711_decoder_type == 0) {
    static const GTypeInfo g711_decoder_info = {
      sizeof (GstG711DecoderIntfc), /* class_size */
      gst_g711_decoder_base_init,  /* base_init */
      gst_g711_decoder_base_finalize,  /* base_finalize */
      NULL,
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      0,
      0,
      NULL
    };


    g711_decoder_type = g_type_register_static (G_TYPE_INTERFACE, "GstG711DecoderInterface",
        &g711_decoder_info, 0);

  }

  return g711_decoder_type;
}


static void gst_g711_decoder_base_init (gpointer g_class)
    {
    static gboolean initialized = FALSE;
    g711_decoder_count++;

    if (G_UNLIKELY (!initialized))
        {
        initialized = TRUE;
        }
    }

static void gst_g711_decoder_base_finalize (gpointer g_class)
    {
    g711_decoder_count--;

    }


