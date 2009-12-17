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

#ifndef __gst_interfaces_marshal_MARSHAL_H__
#define __gst_interfaces_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,BOOLEAN (interfaces-marshal.list:1) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_interfaces_marshal_VOID__OBJECT_BOOLEAN (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);

/* VOID:OBJECT,POINTER (interfaces-marshal.list:2) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_interfaces_marshal_VOID__OBJECT_POINTER (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);

/* VOID:OBJECT,STRING (interfaces-marshal.list:3) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_interfaces_marshal_VOID__OBJECT_STRING (GClosure     *closure,
                                                        GValue       *return_value,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint,
                                                        gpointer      marshal_data);

/* VOID:OBJECT,ULONG (interfaces-marshal.list:4) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_interfaces_marshal_VOID__OBJECT_ULONG (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);

/* VOID:OBJECT,INT (interfaces-marshal.list:5) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_interfaces_marshal_VOID__OBJECT_INT (GClosure     *closure,
                                                     GValue       *return_value,
                                                     guint         n_param_values,
                                                     const GValue *param_values,
                                                     gpointer      invocation_hint,
                                                     gpointer      marshal_data);

G_END_DECLS

#endif /* __gst_interfaces_marshal_MARSHAL_H__ */

