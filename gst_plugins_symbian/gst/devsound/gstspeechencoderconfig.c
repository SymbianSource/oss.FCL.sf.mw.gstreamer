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

#include "gstregistry.h"

#include "gstspeechencoderconfig.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif

static void gst_speech_encoder_config_base_init (gpointer g_class);

EXPORT_C GType gst_speech_encoder_config_get_type (void)
{
  static GType speech_encoder_config_type = 0;

  if (G_UNLIKELY (speech_encoder_config_type == 0)) {
    static const GTypeInfo speech_encoder_config_info = {
      sizeof (GstSpeechEncoderConfigIntfc),  /* class_size */
      gst_speech_encoder_config_base_init,  /* base_init */
      NULL,                     /* base_finalize */
      NULL,
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      0,
      0,
      NULL
    };

  
    speech_encoder_config_type = g_type_register_static (G_TYPE_INTERFACE, "GstSpeechEncoderConfig",
        &speech_encoder_config_info, 0);

  }

  return speech_encoder_config_type;
}

static void gst_speech_encoder_config_base_init (gpointer g_class)
    {
    static gboolean initialized = FALSE;
    
    if (G_UNLIKELY (!initialized)) 
        {
        initialized = TRUE;
        }
    }
