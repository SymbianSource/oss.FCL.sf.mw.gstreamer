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


#ifndef __G711_DECODER_INTERFACE__H
#define __G711_DECODER_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GST_TYPE_G711_DECODER               (gst_g711_decoder_get_type ())
#define GST_G711_DECODER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_G711_DECODER, GstG711Decoder))
#define GST_G711_DECODER_CLASS(vtable)      (G_TYPE_CHECK_CLASS_CAST ((vtable), GST_TYPE_G711_DECODER, GstG711DecoderIntfc))
#define GST_IS_G711_DECODER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_G711_DECODER))
#define GST_IS_G711_DECODER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_TYPE ((vtable), GST_TYPE_G711_DECODER))
#define GST_G711_DECODER_GET_IFACE(obj)     (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_G711_DECODER, GstG711DecoderIntfc))

typedef struct _GstG711Decoder     GstG711Decoder;
typedef struct _GstG711DecoderIntfc GstG711DecoderIntfc;


/**
*  Interface for G711 decoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*/
enum TG711DecodeMode
    {
    EDecALaw,
    EDecULaw
     };
struct _GstG711DecoderIntfc
    {
    GTypeInterface parent;   
    gint (*SetDecoderMode)(enum TG711DecodeMode aDecodeMode);
    gint (*SetCng)(gboolean aCng);
	gint (*GetCng)(gboolean* aCng);
	gint (*SetPlc)(gboolean aPlc);
	
    };

IMPORT_C GType gst_g711_decoder_get_type(void);

G_END_DECLS

#endif /* __G711_DECODER_INTERFACE__ */
