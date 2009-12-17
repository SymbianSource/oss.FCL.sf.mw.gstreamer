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
  {"GstAdapter", sizeof (GstAdapter), 88}
  ,
  {"GstAdapterClass", sizeof (GstAdapterClass), 168}
  ,
  {"GstBaseSink", sizeof (GstBaseSink), 600}
  ,
  {"GstBaseSinkClass", sizeof (GstBaseSinkClass), 728}
  ,
  {"GstBaseSrc", sizeof (GstBaseSrc), 600}
  ,
  {"GstBaseSrcClass", sizeof (GstBaseSrcClass), 744}
  ,
  {"GstBaseTransform", sizeof (GstBaseTransform), 576}
  ,
  {"GstBaseTransformClass", sizeof (GstBaseTransformClass), 744}
  ,
  {"GstCollectData", sizeof (GstCollectData), 168}
  ,
  {"GstCollectPads", sizeof (GstCollectPads), 168}
  ,
  {"GstCollectPadsClass", sizeof (GstCollectPadsClass), 272}
  ,
  {"GstPushSrc", sizeof (GstPushSrc), 632}
  ,
  {"GstPushSrcClass", sizeof (GstPushSrcClass), 784}
  ,
  {"GstTimedValue", sizeof (GstTimedValue), 32}
  ,
  {"GstValueArray", sizeof (GstValueArray), 32}
  ,
  {"GstController", sizeof (GstController), 80}
  ,
  {"GstControllerClass", sizeof (GstControllerClass), 168}
  ,
  {"GstNetClientClock", sizeof (GstNetClientClock), 368}
  ,
  {"GstNetClientClockClass", sizeof (GstNetClientClockClass), 384}
  ,
  {"GstNetTimePacket", sizeof (GstNetTimePacket), 16}
  ,
  {"GstNetTimeProvider", sizeof (GstNetTimeProvider), 152}
  ,
  {"GstNetTimeProviderClass", sizeof (GstNetTimeProviderClass), 240}
  ,
  {NULL, 0, 0}
};
