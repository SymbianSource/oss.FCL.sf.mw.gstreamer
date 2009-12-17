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
  {"GstBin", sizeof (GstBin), 192},
  {"GstBinClass", sizeof (GstBinClass), 288},
  {"GstBuffer", sizeof (GstBuffer), 88},
  {"GstBufferClass", sizeof (GstBufferClass), 16},
  {"GstBus", sizeof (GstBus), 80},
  {"GstBusClass", sizeof (GstBusClass), 144},
  {"GstCaps", sizeof (GstCaps), 32},
  {"GstStaticCaps", sizeof (GstStaticCaps), 52},
  {"GstChildProxyInterface", sizeof (GstChildProxyInterface), 40},
  {"GstClockEntry", sizeof (GstClockEntry), 48},
  {"GstClock", sizeof (GstClock), 176},
  {"GstClockClass", sizeof (GstClockClass), 160},
  {"GstElement", sizeof (GstElement), 136},
  {"GstElementClass", sizeof (GstElementClass), 248},
  {"GstElementFactory", sizeof (GstElementFactory), 144},
  {"GstElementFactoryClass", sizeof (GstElementFactoryClass), 152},
  {"GstElementDetails", sizeof (GstElementDetails), 32},
  {"GstEvent", sizeof (GstEvent), 48},
  {"GstEventClass", sizeof (GstEventClass), 32},
  {"GstFormatDefinition", sizeof (GstFormatDefinition), 16},
  {"GstIndexEntry", sizeof (GstIndexEntry), 20},
  {"GstIndexGroup", sizeof (GstIndexGroup), 16},
  {"GstIndex", sizeof (GstIndex), 100},
  {"GstIndexClass", sizeof (GstIndexClass), 156},
  {"GstIndexAssociation", sizeof (GstIndexAssociation), 16},
  {"GstIndexFactory", sizeof (GstIndexFactory), 96},
  {"GstIndexFactoryClass", sizeof (GstIndexFactoryClass), 152},
  {"GstDebugCategory", sizeof (GstDebugCategory), 16},
  {"GstImplementsInterfaceClass", sizeof (GstImplementsInterfaceClass), 28},
  {"GstIterator", sizeof (GstIterator), 52},
  {"GstMessage", sizeof (GstMessage), 64},
  {"GstMessageClass", sizeof (GstMessageClass), 32},
  {"GstMiniObject", sizeof (GstMiniObject), 16},
  {"GstMiniObjectClass", sizeof (GstMiniObjectClass), 16},
  {"GstObject", sizeof (GstObject), 40},
  {"GstObjectClass", sizeof (GstObjectClass), 120},
  {"GstPad", sizeof (GstPad), 188},
  {"GstPadClass", sizeof (GstPadClass), 152},
  {"GstPadTemplate", sizeof (GstPadTemplate), 72},
  {"GstPadTemplateClass", sizeof (GstPadTemplateClass), 140},
  {"GstStaticPadTemplate", sizeof (GstStaticPadTemplate), 64},
  {"GstPipeline", sizeof (GstPipeline), 232},
  {"GstPipelineClass", sizeof (GstPipelineClass), 304},
  {"GstPlugin", sizeof (GstPlugin), 152},
  {"GstPluginClass", sizeof (GstPluginClass), 136},
  {"GstPluginDesc", sizeof (GstPluginDesc), 56},
  {"GstPluginFeature", sizeof (GstPluginFeature), 72},
  {"GstPluginFeatureClass", sizeof (GstPluginFeatureClass), 136},
  {"GstQueryTypeDefinition", sizeof (GstQueryTypeDefinition), 16},
  {"GstQuery", sizeof (GstQuery), 28},
  {"GstQueryClass", sizeof (GstQueryClass), 32},
  {"GstRegistry", sizeof (GstRegistry), 72},
  {"GstRegistryClass", sizeof (GstRegistryClass), 144},
  {"GstSegment", sizeof (GstSegment), 88},
  {"GstStructure", sizeof (GstStructure), 20},
  {"GstSystemClock", sizeof (GstSystemClock), 200},
  {"GstSystemClockClass", sizeof (GstSystemClockClass), 176},
  {"GstTagSetterIFace", sizeof (GstTagSetterIFace), 8},
  {"GstTask", sizeof (GstTask), 80},
  {"GstTaskClass", sizeof (GstTaskClass), 140},
  {"GstTrace", sizeof (GstTrace), 20},
  {"GstTraceEntry", sizeof (GstTraceEntry), 128},
  {"GstAllocTrace", sizeof (GstAllocTrace), 16},
  {"GstTypeFind", sizeof (GstTypeFind), 32},
  {"GstTypeFindFactory", sizeof (GstTypeFindFactory), 108},
  {"GstTypeFindFactoryClass", sizeof (GstTypeFindFactoryClass), 152},
  {"GstURIHandlerInterface", sizeof (GstURIHandlerInterface), 44},
  {"GstValueTable", sizeof (GstValueTable), 32},
  {"GstXML", sizeof (GstXML), 64},
  {"GstXMLClass", sizeof (GstXMLClass), 144},
  {NULL, 0, 0}
};
