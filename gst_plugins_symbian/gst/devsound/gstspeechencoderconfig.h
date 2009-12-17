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


#ifndef __SPEECH_ENCODER_CONFIG_INTERFACE__H
#define __SPEECH_ENCODER_CONFIG_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_SPEECH_ENCODER_CONFIG     (gst_speech_encoder_config_get_type ())
#define GST_SPEECH_ENCODER_CONFIG(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_SPEECH_ENCODER_CONFIG, GstSpeechEncoderConfig))
#define GST_IS_SPEECH_ENCODER_CONFIG(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_SPEECH_ENCODER_CONFIG))
#define GST_SPEECH_ENCODER_CONFIG_GET_IFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_SPEECH_ENCODER_CONFIG, GstSpeechEncoderConfigIntfc))

typedef struct _GstSpeechEncoderConfig      GstSpeechEncoderConfig;
typedef struct _GstSpeechEncoderConfigIntfc GstSpeechEncoderConfigIntfc;

struct _GstSpeechEncoderConfigIntfc
    {
    GTypeInterface parent;
 	gint (*GetSupportedBitrates)(GList** aSupportedBitrates);
	gint (*SetBitrate)(guint aBitrate);
	gint (*GetBitrate)(guint* aBitrate);
	gint (*SetVadMode)(gboolean aVadMode);
	gint (*GetVadMode)(gboolean* aVadMode);

    };

IMPORT_C GType gst_speech_encoder_config_get_type(void);

G_END_DECLS

#endif /* __SPEECH_ENCODER_CONFIG_INTERFACE */
