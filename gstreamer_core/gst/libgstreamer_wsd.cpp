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



#include "libgstreamer_wsd_solution.h" 

#ifdef EMULATOR

#include <e32std.h> 
#include <pls.h> // For emulator WSD API 
#include <glib.h>

const TUid KLibGStreamerUid3 = {0x0AB229CB};     // This is the UID of the library

extern "C" EXPORT_C	struct libgstreamer_global_struct *libgstreamer_ImpurePtr()
{
	  
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct libgstreamer_global_struct* p  = Pls<struct libgstreamer_global_struct>(KLibGStreamerUid3, &libgstreamer_Init);
  	return p;

#else

	return NULL;	

#endif

	  
	  
}

int libgstreamer_Init(libgstreamer_global_struct *g)
{
#if defined(__WINSCW__) || defined(__WINS__)
	if(g)
	{
//#if GST_TESTCASE_COMPILATION 
    	g->GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g) = NULL;
    	g->GET_GSTREAMER_WSD_VAR_NAME(check_mutex,gstcheck,g) = NULL;
    	g->GET_GSTREAMER_WSD_VAR_NAME(check_cond,gstcheck,g) = NULL;    	
 	
    	g->GET_GSTREAMER_WSD_VAR_NAME(fp_std_log,std_log_result,g) = NULL;
    	g->GET_GSTREAMER_WSD_VAR_NAME(assert_failed,std_log_result,g) = 0;
    	g->GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g) = FALSE;
			g->GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g) = FALSE;
			g->GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g) = FALSE;
			
		g->GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g) = NULL;
//#endif  /* GST_TESTCASE_COMPILATION */		
	

		return 0;
	  }
	  return 1;
#else

	return 1;

#endif		  
}

extern "C" EXPORT_C void _gstreamer_wsd_reset()
{
	struct libgstreamer_global_struct* p  = Pls<struct libgstreamer_global_struct>(KLibGStreamerUid3, &libgstreamer_Init);	
	if(p)
	{
		memset(p,0,sizeof(struct libgstreamer_global_struct));
		libgstreamer_Init(p);
	}
}

#endif // EMULATOR


