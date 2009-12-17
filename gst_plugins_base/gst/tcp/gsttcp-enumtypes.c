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

/* Generated data (by glib-mkenums) */

#include "gsttcp-enumtypes.h"

#include "gsttcp.h"

/* enumerations from "gsttcp.h" */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_tcp_protocol_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { GST_TCP_PROTOCOL_NONE, "GST_TCP_PROTOCOL_NONE", "none" },
      { GST_TCP_PROTOCOL_GDP, "GST_TCP_PROTOCOL_GDP", "gdp" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("GstTCPProtocol", values);
  }
  return etype;
}

/* Generated data ends here */

