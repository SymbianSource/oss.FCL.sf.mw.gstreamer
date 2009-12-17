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

static GstCheckABIStruct list[] = {
 {"GstAdapter", sizeof(GstAdapter), 52},
 {"GstAdapterClass", sizeof(GstAdapterClass), 84},
 {"GstBaseSink", sizeof(GstBaseSink), 384},
 {"GstBaseSinkClass", sizeof(GstBaseSinkClass), 368},
 {"GstBaseSrc", sizeof(GstBaseSrc), 376},
 {"GstBaseSrcClass", sizeof(GstBaseSrcClass), 376},
 {"GstBaseTransform", sizeof(GstBaseTransform), 352},
 {"GstBaseTransformClass", sizeof(GstBaseTransformClass), 376},
 {"GstCollectData", sizeof(GstCollectData), 120},
 {"GstCollectPads", sizeof(GstCollectPads), 92},
 {"GstCollectPadsClass", sizeof(GstCollectPadsClass), 136},
 {"GstPushSrc", sizeof(GstPushSrc), 392},
 {"GstPushSrcClass", sizeof(GstPushSrcClass), 396},

 {"GstTimedValue", sizeof(GstTimedValue), 28},
 {"GstValueArray", sizeof(GstValueArray), 20},
 {"GstController", sizeof(GstController), 40},
 {"GstControllerClass", sizeof(GstControllerClass), 84},

 {"GstNetClientClock", sizeof(GstNetClientClock), 248},
 {"GstNetClientClockClass", sizeof(GstNetClientClockClass), 192},
 {"GstNetTimePacket", sizeof(GstNetTimePacket), 16},
 {"GstNetTimeProvider", sizeof(GstNetTimeProvider), 84},
 {"GstNetTimeProviderClass", sizeof(GstNetTimeProviderClass), 120},

 {NULL, 0, 0}
};
