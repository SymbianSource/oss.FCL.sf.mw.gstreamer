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



#ifndef __ILBC_ENCODER_INTERFACE__H
#define __ILBC_ENCODER_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GST_TYPE_ILBC_ENCODER               (gst_ilbc_encoder_get_type ())
#define GST_ILBC_ENCODER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_ILBC_ENCODER, GstIlbcEncoder))
#define GST_ILBC_ENCODER_CLASS(vtable)      (G_TYPE_CHECK_CLASS_CAST ((vtable), GST_TYPE_ILBC_ENCODER, GstIlbcEncoderIntfc))
#define GST_IS_ILBC_ENCODER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_ILBC_ENCODER))
#define GST_IS_ILBC_ENCODER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_TYPE ((vtable), GST_TYPE_ILBC_ENCODER))
#define GST_ILBC_ENCODER_GET_IFACE(obj)     (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_ILBC_ENCODER, GstIlbcEncoderIntfc))

typedef struct _GstIlbcEncoder     GstIlbcEncoder;
typedef struct _GstIlbcEncoderIntfc GstIlbcEncoderIntfc;


/**
*  Interface for Ilbc encoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*/
enum TIlbcEncodeMode
	{
	EIlbc20msFrame = 1,
	EIlbc30msFrame
	};
	
struct _GstIlbcEncoderIntfc
    {
    GTypeInterface parent;  
    gint (*SetEncoderMode)(enum TIlbcEncodeMode aEncodeMode);
    gint (*SetVadMode) (gboolean aVadMode);
    gint (*GetVadMode)(gboolean* aVadMode);
	
    };

IMPORT_C GType gst_ilbc_encoder_get_type(void);

G_END_DECLS

#endif /* __ILBC_ENCODER_INTERFACE__ */
