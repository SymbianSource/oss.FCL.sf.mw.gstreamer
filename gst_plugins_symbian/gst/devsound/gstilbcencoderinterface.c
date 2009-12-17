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

#include "gstilbcencoderinterface.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif

static guint ilbc_encoder_count = 0;
static void gst_ilbc_encoder_base_init (gpointer g_class);
static void gst_ilbc_encoder_base_finalize (gpointer g_class);
             
EXPORT_C GType gst_ilbc_encoder_get_type(void)
{
  static GType ilbc_encoder_type = 0;

  if (ilbc_encoder_type == 0) {
    static const GTypeInfo ilbc_encoder_info = {
      sizeof (GstIlbcEncoderIntfc), /* class_size */
      gst_ilbc_encoder_base_init,  /* base_init */
      gst_ilbc_encoder_base_finalize,  /* base_finalize */
      NULL,
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      0,
      0,
      NULL
    };

  
    ilbc_encoder_type = g_type_register_static (G_TYPE_INTERFACE, "GstIlbcEncoderInterface",
        &ilbc_encoder_info, 0);
  
  }

  return ilbc_encoder_type;
}


static void gst_ilbc_encoder_base_init (gpointer g_class)
    {
    static gboolean initialized = FALSE;
    ilbc_encoder_count++;
    
    if (G_UNLIKELY (!initialized)) 
        {
        initialized = TRUE;
        }
    }

static void gst_ilbc_encoder_base_finalize (gpointer g_class)
    {
    ilbc_encoder_count--;
     
    }


