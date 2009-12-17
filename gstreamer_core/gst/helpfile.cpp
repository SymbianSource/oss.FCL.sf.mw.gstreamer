/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 

#include "helpfile.h" 
#include <f32file.h>
#include <e32std.h> 
#include <pls.h> // For emulator WSD API 
#include <glib.h>

extern "C" char* libgstreamer_get_dll_path(char* dllName)
{
    TUint ch = TUint('A');
    char* fullPath = dllName;
    #ifdef __WINSCW__ 
    char* dllPath = "z:\\sys\\bin\\plugins";
    fullPath = g_strjoin ("\\", dllPath,dllName, NULL);
    #else
    char* dllPath = "sys\\bin";
    
    char* filename;
    TInt i; 
    RFs fs;
    TDriveList driveList;
    //err = fs.Connect();
    if( !(fs.Connect()) )
    {
        //err = fs.DriveList(driveList);
        if( !(fs.DriveList(driveList)) )
        {
            for( i = 0; i< KMaxDrives; i++ )
            {
            
                if( driveList[i] != 0 )
                {
                    char temp[3];
                    temp[0]= char(ch);
                    temp[1]= ':';
                    temp[2]= '\0';
                    
                    filename = g_strjoin ("\\", temp,dllPath,dllName, NULL);
                    if (g_file_test (filename, G_FILE_TEST_EXISTS))
                    {
                        fullPath = filename;
                        break;
                    }
                }
                ch++;
            }
        }
        fs.Close();
    }
    #endif
    return fullPath;
}

