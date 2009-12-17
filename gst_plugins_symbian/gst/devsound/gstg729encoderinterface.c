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

#include "gstg729encoderinterface.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif

static guint g729_encoder_count = 0;
static void gst_g729_encoder_base_init (gpointer g_class);
static void gst_g729_encoder_base_finalize (gpointer g_class);
             
EXPORT_C GType gst_g729_encoder_get_type(void)
{
  static GType g729_encoder_type = 0;

  if (g729_encoder_type == 0) {
    static const GTypeInfo g729_encoder_info = {
      sizeof (GstG729EncoderIntfc), /* class_size */
      gst_g729_encoder_base_init,  /* base_init */
      gst_g729_encoder_base_finalize,  /* base_finalize */
      NULL,
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      0,
      0,
      NULL
    };

  
    g729_encoder_type = g_type_register_static (G_TYPE_INTERFACE, "GstG729EncoderInterface",
        &g729_encoder_info, 0);
  
  }

  return g729_encoder_type;
}


static void gst_g729_encoder_base_init (gpointer g_class)
    {
    static gboolean initialized = FALSE;
    g729_encoder_count++;
    
    if (G_UNLIKELY (!initialized)) 
        {
        initialized = TRUE;
        }
    }

static void gst_g729_encoder_base_finalize (gpointer g_class)
    {
    g729_encoder_count--;
     
    }


