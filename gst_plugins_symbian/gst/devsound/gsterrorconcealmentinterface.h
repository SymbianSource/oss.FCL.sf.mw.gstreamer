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


#ifndef __ERROR_CONCEALMENT_INTERFACE__H
#define __ERROR_CONCEALMENT_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_ERROR_CONCEALMENT     (gst_error_concealment_get_type ())
#define GST_ERROR_CONCEALMENT(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_ERROR_CONCEALMENT, GstErrorConcealment))
#define GST_IS_ERROR_CONCEALMENT(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_ERROR_CONCEALMENT))
#define GST_ERROR_CONCEALMENT_GET_IFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_ERROR_CONCEALMENT, GstErrorConcealmentIntfc))

typedef struct _GstErrorConcealment      GstErrorConcealment;
typedef struct _GstErrorConcealmentIntfc GstErrorConcealmentIntfc;

struct _GstErrorConcealmentIntfc
    {
    GTypeInterface parent;
    /**
     * Indicates that an frame of encoded audio is lost and that error
     * concealment should be performed for a lost frame.
     * By definition, the next buffer sent to the decoder will contain no
     * valid audio data.
     * @return status
     */
    gint (*ConcealErrorForNextBuffer) (void);

    /**
     * Puts the hwdevice into a frame-based interface.
     * The alternate mode of interface would be buffer-based.
     * @param aFrameMode
     * @return status
     */
    gint (*SetFrameMode)(gboolean aFrameMode);

    /**
     * Query to find out if frame-based interface is required by hwdevice
     * for supporting error concealment.
     * @param aFrameModeRqrd indicates if frame mode is required.
     * @return status
     */
    gint (*FrameModeRqrdForEC)(gboolean* aFrameModeRqrd);

    };

IMPORT_C GType gst_error_concealment_get_type(void);

G_END_DECLS

#endif /* __ERROR_CONCEALMENT_INTERFACE */
