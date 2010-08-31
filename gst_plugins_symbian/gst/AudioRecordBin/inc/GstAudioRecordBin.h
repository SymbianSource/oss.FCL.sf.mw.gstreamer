/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/

// This file defines the API for GSTAUDIORECORDBIN.dll

#ifndef __GSTAUDIORECORDBIN_H__
#define __GSTAUDIORECORDBIN_H__

//  Include Files

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf
#include <gst/gst.h>


#ifdef __cplusplus
extern "C"
{
#endif     
       
    IMPORT_C GstElement* CreateAudioRecordBin( GstElement* Source, GstCaps* encCaps, GstCaps* muxCaps);
    
#ifdef __cplusplus
}
#endif


#endif  // __GSTAUDIORECORDBIN_H__

