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
  {"GstBin", sizeof (GstBin), 336},
  {"GstBinClass", sizeof (GstBinClass), 568},
  {"GstBuffer", sizeof (GstBuffer), 120} ,
  {"GstBufferClass", sizeof (GstBufferClass), 32} ,
  {"GstBus", sizeof (GstBus), 152} ,
  {"GstBusClass", sizeof (GstBusClass), 288} ,
  {"GstCaps", sizeof (GstCaps), 56} ,
  {"GstStaticCaps", sizeof (GstStaticCaps), 96} ,
  {"GstChildProxyInterface", sizeof (GstChildProxyInterface), 80} ,
  {"GstClockEntry", sizeof (GstClockEntry), 64} ,
  {"GstClock", sizeof (GstClock), 240} ,
  {"GstClockClass", sizeof (GstClockClass), 320} ,
  {"GstElement", sizeof (GstElement), 232} ,
  {"GstElementClass", sizeof (GstElementClass), 488} ,
  {"GstElementFactory", sizeof (GstElementFactory), 280} ,
  {"GstElementFactoryClass", sizeof (GstElementFactoryClass), 304} ,
  {"GstElementDetails", sizeof (GstElementDetails), 64} ,
  {"GstEvent", sizeof (GstEvent), 64} ,
  {"GstEventClass", sizeof (GstEventClass), 64} ,
  {"GstFormatDefinition", sizeof (GstFormatDefinition), 32} ,
  {"GstIndexEntry", sizeof (GstIndexEntry), 32} ,
  {"GstIndexGroup", sizeof (GstIndexGroup), 24} ,
  {"GstIndex", sizeof (GstIndex), 192} ,
  {"GstIndexClass", sizeof (GstIndexClass), 312} ,
  {"GstIndexAssociation", sizeof (GstIndexAssociation), 16} ,
  {"GstIndexFactory", sizeof (GstIndexFactory), 192} ,
  {"GstIndexFactoryClass", sizeof (GstIndexFactoryClass), 304} ,
  {"GstDebugCategory", sizeof (GstDebugCategory), 24} ,
  {"GstImplementsInterfaceClass", sizeof (GstImplementsInterfaceClass), 56} ,
  {"GstIterator", sizeof (GstIterator), 104} ,
  {"GstMessage", sizeof (GstMessage), 104} ,
  {"GstMessageClass", sizeof (GstMessageClass), 64} ,
  {"GstMiniObject", sizeof (GstMiniObject), 24} ,
  {"GstMiniObjectClass", sizeof (GstMiniObjectClass), 32} ,
  {"GstObject", sizeof (GstObject), 80} ,
  {"GstObjectClass", sizeof (GstObjectClass), 240} ,
  {"GstPad", sizeof (GstPad), 368} ,
  {"GstPadClass", sizeof (GstPadClass), 304} ,
  {"GstPadTemplate", sizeof (GstPadTemplate), 136} ,
  {"GstPadTemplateClass", sizeof (GstPadTemplateClass), 280} ,
  {"GstStaticPadTemplate", sizeof (GstStaticPadTemplate), 112} ,
  {"GstPipeline", sizeof (GstPipeline), 392} ,
  {"GstPipelineClass", sizeof (GstPipelineClass), 600} ,
  {"GstPlugin", sizeof (GstPlugin), 280} ,
  {"GstPluginClass", sizeof (GstPluginClass), 272} ,
  {"GstPluginDesc", sizeof (GstPluginDesc), 104} ,
  {"GstPluginFeature", sizeof (GstPluginFeature), 144} ,
  {"GstPluginFeatureClass", sizeof (GstPluginFeatureClass), 272} ,
  {"GstQueryTypeDefinition", sizeof (GstQueryTypeDefinition), 32} ,
  {"GstQuery", sizeof (GstQuery), 48} ,
  {"GstQueryClass", sizeof (GstQueryClass), 64} ,
  {"GstRegistry", sizeof (GstRegistry), 144} ,
  {"GstRegistryClass", sizeof (GstRegistryClass), 288} ,
  {"GstSegment", sizeof (GstSegment), 104} ,
  {"GstStructure", sizeof (GstStructure), 40} ,
  {"GstSystemClock", sizeof (GstSystemClock), 288} ,
  {"GstSystemClockClass", sizeof (GstSystemClockClass), 352} ,
  {"GstTagSetterIFace", sizeof (GstTagSetterIFace), 16} ,
  {"GstTask", sizeof (GstTask), 160} ,
  {"GstTaskClass", sizeof (GstTaskClass), 280} ,
  {"GstTrace", sizeof (GstTrace), 32} ,
  {"GstTraceEntry", sizeof (GstTraceEntry), 128} ,
  {"GstAllocTrace", sizeof (GstAllocTrace), 24} ,
  {"GstTypeFind", sizeof (GstTypeFind), 64} ,
  {"GstTypeFindFactory", sizeof (GstTypeFindFactory), 216} ,
  {"GstTypeFindFactoryClass", sizeof (GstTypeFindFactoryClass), 304} ,
  {"GstURIHandlerInterface", sizeof (GstURIHandlerInterface), 88} ,
  {"GstValueTable", sizeof (GstValueTable), 64} ,
#ifndef GST_DISABLE_LOADSAVE
  {"GstXML", sizeof (GstXML), 128} ,
  {"GstXMLClass", sizeof (GstXMLClass), 288} ,
#endif
  {NULL, 0, 0}
};
