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
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>

int
main (int argc, char *argv[])
{
  //This will update registry
  gst_init (&argc, &argv);  
  return 0;
}
