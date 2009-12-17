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

#include "gstilbcdecoderinterface.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif

static guint ilbc_decoder_count = 0;
static void gst_ilbc_decoder_base_init (gpointer g_class);
static void gst_ilbc_decoder_base_finalize (gpointer g_class);
             
EXPORT_C GType gst_ilbc_decoder_get_type(void)
{
  static GType ilbc_decoder_type = 0;

  if (ilbc_decoder_type == 0) {
    static const GTypeInfo ilbc_decoder_info = {
      sizeof (GstIlbcDecoderIntfc), /* class_size */
      gst_ilbc_decoder_base_init,  /* base_init */
      gst_ilbc_decoder_base_finalize,  /* base_finalize */
      NULL,
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      0,
      0,
      NULL
    };

  
    ilbc_decoder_type = g_type_register_static (G_TYPE_INTERFACE, "GstIlbcDecoderInterface",
        &ilbc_decoder_info, 0);
  
  }

  return ilbc_decoder_type;
}


static void gst_ilbc_decoder_base_init (gpointer g_class)
    {
    static gboolean initialized = FALSE;
    ilbc_decoder_count++;
    
    if (G_UNLIKELY (!initialized)) 
        {
        initialized = TRUE;
        }
    }

static void gst_ilbc_decoder_base_finalize (gpointer g_class)
    {
    ilbc_decoder_count--;
     
    }


