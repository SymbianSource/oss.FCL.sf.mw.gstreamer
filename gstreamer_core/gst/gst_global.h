/*
 *  Copyright © 2005-06 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ============================================================================
 */

#ifndef __GST_GLOBAL_H__
#define __GST_GLOBAL_H__

#define GST_CAT_QOS (*_GST_CAT_QOS())
#define	GST_CAT_DEFAULT	(*_GST_CAT_DEFAULT())
#define GST_CAT_EVENT (*_GST_CAT_EVENT())
#define	__gst_debug_enabled	(*gst_debug_enabled())
#define __gst_debug_min (*gst_debug_min())
#endif /* __GST_GLOBAL_H__ */
