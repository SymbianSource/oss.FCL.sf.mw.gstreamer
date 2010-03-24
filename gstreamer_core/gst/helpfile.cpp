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

#include "helpfile.h" 
#include <f32file.h>
#include <e32std.h> 
#include <pls.h> // For emulator WSD API 
#include <glib.h>
#include <config.h>
#include <stdlib.h> 

extern "C" char* libgstreamer_get_dll_path(char* dllName)
{
    char* dllFullPath = NULL;
    RFs fs;
    CDir* directory=NULL;
    if( !(fs.Connect()) )
    {
        TFindFile findFile(fs);
        TFileName file;
        TPtrC8 filename;
        filename.Set( reinterpret_cast<TUint8*>( dllName ) );
        file.Copy( filename );
        if( ! findFile.FindWildByDir(file,KResFileNamePath,directory) )
        {
            TBuf8<KMaxFileName> fileName;
            fileName.Copy( findFile.File() );
            TInt length = fileName.Size() + 1;
            dllFullPath = (char*)malloc( length );
            memcpy( dllFullPath, fileName.PtrZ(), length );
            delete directory;
        }
        fs.Close();
    }
    return dllFullPath;
}

