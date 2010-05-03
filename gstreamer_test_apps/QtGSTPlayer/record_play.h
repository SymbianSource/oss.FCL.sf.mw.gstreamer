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
 
#ifndef __HELP_FILE_H__
#define __HELP_FILE_H__
#include <glib.h>
#include <stdio.h>

enum
{
    NONE,
    PLAYING,
    STOP,
    PAUSE,
    /*RESUME,
    RECORDING*/
};

enum
{
    RECORD_NONE,
    RECORD_AMR,
    RECORD_WAV,
    RECORD_RAW,
    RECORD_G711,
    RECORD_G729,
    RECORD_ILBC,
    RECORD_AAC
};


#ifdef __cplusplus
extern "C" 
{
#endif

int gst_play_file( const char* file );

int gst_record_file( int type );

int gst_record_wav();

int gst_pause();

int gst_resume();

int gst_record_stop();

int gst_seek();

int gst_unref();

int gst_get_events();

int gst_record_aac();

int gst_record_amr();

#ifdef __cplusplus
}
#endif
#endif //__HELP_FILE_H__

