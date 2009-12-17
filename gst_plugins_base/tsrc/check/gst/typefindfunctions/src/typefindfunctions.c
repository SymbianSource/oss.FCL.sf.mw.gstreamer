/* GStreamer unit tests for the -base typefind functions
 *
 * Copyright (C) 2007 Tim-Philipp Müller <tim centricular net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gst/gst_global.h>
#include <gst/check/gstcheck.h>
#include <gst/base/gsttypefindhelper.h>


#define LOG_FILE "c:\\logs\\typefindfunctions_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log;
#endif

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

void test_quicktime_mpeg4video()
{
  /* quicktime redirect file which starts with what could also be interpreted
   * as an MPEG-4 video object layer start code */
  const guint8 qt_redirect_396042[] =
      { 0x00, 0x00, 0x01, 0x22, 0x6d, 0x6f, 0x6f, 0x76, 0x00, 0x00, 0x01, 0x1a,
    0x72, 0x6d, 0x72, 0x61, 0x00, 0x00, 0x00, 0x86, 0x72, 0x6d, 0x64, 0x61,
    0x00, 0x00, 0x00, 0x54, 0x72, 0x64, 0x72, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x75, 0x72, 0x6c, 0x20, 0x00, 0x00, 0x00, 0x40, 0x68, 0x74, 0x74, 0x70,
    0x3a, 0x2f, 0x2f, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0x2e, 0x71, 0x74,
    0x76, 0x2e, 0x61, 0x70, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x2f,
    0x65, 0x76, 0x65, 0x6e, 0x74, 0x73, 0x2f, 0x6a, 0x61, 0x6e, 0x2f, 0x6a,
    0x34, 0x37, 0x64, 0x35, 0x32, 0x6f, 0x6f, 0x2f, 0x71, 0x74, 0x37, 0x72,
    0x65, 0x71, 0x75, 0x69, 0x72, 0x65, 0x64, 0x2e, 0x6d, 0x6f, 0x76, 0x00,
    0x00, 0x00, 0x00, 0x10, 0x72, 0x6d, 0x64, 0x72, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0a, 0xf0, 0x00, 0x00, 0x00, 0x1a, 0x72, 0x6d, 0x76, 0x63,
    0x00, 0x00, 0x00, 0x00, 0x71, 0x74, 0x69, 0x6d, 0x06, 0x50, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x72, 0x6d,
    0x64, 0x61, 0x00, 0x00, 0x00, 0x5a, 0x72, 0x64, 0x72, 0x66, 0x00, 0x00,
    0x00, 0x00, 0x75, 0x72, 0x6c, 0x20, 0x00, 0x00, 0x00, 0x46, 0x68, 0x74,
    0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0x2e,
    0x71, 0x74, 0x76, 0x2e, 0x61, 0x70, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f,
    0x6d, 0x2f, 0x65, 0x76, 0x65, 0x6e, 0x74, 0x73, 0x2f, 0x6a, 0x61, 0x6e,
    0x2f, 0x6a, 0x34, 0x37, 0x64, 0x35, 0x32, 0x6f, 0x6f, 0x2f, 0x38, 0x38,
    0x34, 0x38, 0x31, 0x32, 0x35, 0x5f, 0x32, 0x5f, 0x33, 0x35, 0x30, 0x5f,
    0x72, 0x65, 0x66, 0x2e, 0x6d, 0x6f, 0x76, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x72, 0x6d, 0x64, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf0,
    0x00, 0x00, 0x00, 0x1a, 0x72, 0x6d, 0x76, 0x63, 0x00, 0x00, 0x00, 0x00,
    0x71, 0x74, 0x69, 0x6d, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
  };
  GstTypeFindProbability prob;
  const gchar *type;
  GstBuffer *buf;
  GstCaps *caps = NULL;
  
  //xmlfile = "test_quicktime_mpeg4video";
  std_log(LOG_FILENAME_LINE, "Test Started test_quicktime_mpeg4video");
  
  buf = gst_buffer_new ();
  GST_BUFFER_DATA (buf) = (guint8 *) qt_redirect_396042;
  GST_BUFFER_SIZE (buf) = sizeof (qt_redirect_396042);
  GST_BUFFER_OFFSET (buf) = 0;

  caps = gst_type_find_helper_for_buffer (NULL, buf, &prob);
  fail_unless (caps != NULL);
  GST_LOG ("Found type: %" GST_PTR_FORMAT, caps);

  type = gst_structure_get_name (gst_caps_get_structure (caps, 0));
  fail_unless_equals_string (type, "video/quicktime");
  fail_unless (prob > GST_TYPE_FIND_MINIMUM && prob <= GST_TYPE_FIND_MAXIMUM);

  gst_buffer_unref (buf);
  gst_caps_unref (caps);
  
  std_log(LOG_FILENAME_LINE, "test_quicktime_mpeg4video Test Successful");
  create_xml(0);
}

void (*fn[]) (void) = {
      test_quicktime_mpeg4video
};

char *args[] = {
"test_quicktime_mpeg4video"
};

GST_CHECK_MAIN (typefindfunctions);


//int main()
//    {
//    
//    gst_check_init(NULL,NULL);
//    test_quicktime_mpeg4video();
//    return 0;
//    }
