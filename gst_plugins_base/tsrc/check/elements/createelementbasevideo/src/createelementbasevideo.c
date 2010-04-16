//createelementbase.c
/*
 *  Copyright © 2005 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer progrs, is protected by copyright controlled by 
 *  Nokia Corporation. All rights are reserved. Copying, 
 *  including reproducing, storing, adapting or translating, any 
 *  or all of this material requires the prior written consent of 
 *  Nokia Corporation. This material also contains confidential 
 *  information which may not be disclosed to others without the 
 *  prior written consent of Nokia Corporation.
 * ============================================================================
 */


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include <gst/gst_global.h>
#include <unistd.h>

#define LOG_FILE "c:\\logs\\createelementbasevideo_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{

    if(result)
    {
        assert_failed = 1;
    } 

    testResultXml(xmlfile);
    close_log_file();

    if(result)
    {
        exit (-1);
    }    

}
#include <gst/check/gstcheck.h>
#include "libgstreamer_wsd_solution.h" 

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
IMPORT_C extern GList *buffers;
#endif


#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical ;
#endif
//gboolean _gst_check_raised_warning = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_warning ;
#endif
//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log ;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running ;
#endif

void test_createelementvideo_base()
{
  GstElement *videorate,*videoscale,*videotestsrc,*ffmpegcolorspace;
  
  // xmlfile = "create_test_element";
    std_log(LOG_FILENAME_LINE, "Test Started element create base video");
   
    videorate       = gst_element_factory_make ("videorate", "videorate");
    fail_unless (videorate != NULL, "Could not create videorate");
 
    videoscale  = gst_element_factory_make ("videoscale", "videoscale");
    fail_unless (videoscale != NULL, "Could not create videoscale");
    
    videotestsrc = gst_element_factory_make ("videotestsrc", "videotestsrc");
    fail_unless (videotestsrc  != NULL, "Could not create videotestsrc ");
    
    ffmpegcolorspace    = gst_element_factory_make ("ffmpegcolorspace", "ffmpegcolorspace");
    fail_unless (ffmpegcolorspace != NULL, "Could not create ffmpegcolorspace");

  gst_object_unref (videorate);
  gst_object_unref (videoscale);
  gst_object_unref (videotestsrc);
  gst_object_unref (ffmpegcolorspace);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
   create_xml(0); 
}

void (*fn[]) (void) = {
        test_createelementvideo_base
};

char *args[] = {
        "test_createelementvideo_base"
};



GST_CHECK_MAIN (createelementbasevideo);
