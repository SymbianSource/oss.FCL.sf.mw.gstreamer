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

#include "gstg729decoderinterface.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif

static guint g729_decoder_count = 0;
static void gst_g729_decoder_base_init (gpointer g_class);
static void gst_g729_decoder_base_finalize (gpointer g_class);
             
EXPORT_C GType gst_g729_decoder_get_type(void)
{
  static GType g729_decoder_type = 0;

  if (g729_decoder_type == 0) {
    static const GTypeInfo g729_decoder_info = {
      sizeof (GstG729DecoderIntfc), /* class_size */
      gst_g729_decoder_base_init,  /* base_init */
      gst_g729_decoder_base_finalize,  /* base_finalize */
      NULL,
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      0,
      0,
      NULL
    };

  
    g729_decoder_type = g_type_register_static (G_TYPE_INTERFACE, "GstG729DecoderInterface",
        &g729_decoder_info, 0);
  
  }

  return g729_decoder_type;
}


static void gst_g729_decoder_base_init (gpointer g_class)
    {
    static gboolean initialized = FALSE;
    g729_decoder_count++;
    
    if (G_UNLIKELY (!initialized)) 
        {
        initialized = TRUE;
        }
    }

static void gst_g729_decoder_base_finalize (gpointer g_class)
    {
    g729_decoder_count--;
     
    }


