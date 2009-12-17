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


#ifndef __G711_ENCODER_INTERFACE__H
#define __G711_ENCODER_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GST_TYPE_G711_ENCODER               (gst_g711_encoder_get_type ())
#define GST_G711_ENCODER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_G711_ENCODER, GstG711Encoder))
#define GST_G711_ENCODER_CLASS(vtable)      (G_TYPE_CHECK_CLASS_CAST ((vtable), GST_TYPE_G711_ENCODER, GstG711EncoderIntfc))
#define GST_IS_G711_ENCODER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_G711_ENCODER))
#define GST_IS_G711_ENCODER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_TYPE ((vtable), GST_TYPE_G711_ENCODER))
#define GST_G711_ENCODER_GET_IFACE(obj)     (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_G711_ENCODER, GstG711EncoderIntfc))

typedef struct _GstG711Encoder     GstG711Encoder;
typedef struct _GstG711EncoderIntfc GstG711EncoderIntfc;


/**
*  Interface for G711 encoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*/
enum TG711EncodeMode
	{
	EEncALaw = 1,
	EEncULaw
	};
	
struct _GstG711EncoderIntfc
    {
    GTypeInterface parent;  
    gint (*SetEncoderMode)(enum TG711EncodeMode aEncodeMode);
    gint (*SetVadMode) (gboolean aVadMode);
    gint (*GetVadMode)(gboolean* aVadMode);
	
    };

IMPORT_C GType gst_g711_encoder_get_type(void);

G_END_DECLS

#endif /* __G711_ENCODER_INTERFACE__ */
