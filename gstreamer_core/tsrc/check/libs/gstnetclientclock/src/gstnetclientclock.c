/* GStreamer
 * Copyright (C) 2005 Andy Wingo <wingo@pobox.com>
 *
 * gstnetclientclock.c: Unit test for the network client clock
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

#include "libgstreamer_wsd_solution.h" 

#define LOG_FILE "c:\\logs\\gstnetclientclock_logs.txt" 
#include "std_log_result.h" 
#include <gst/gst_global.h>
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/check/gstcheck.h>
#include <gst/net/gstnet.h>

#include <unistd.h>

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
IMPORT_C extern gboolean _gst_check_raised_critical ;
#endif
//gboolean _gst_check_raised_warning = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
IMPORT_C extern gboolean _gst_check_raised_warning ;
#endif
//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
IMPORT_C extern gboolean _gst_check_expecting_log ;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
IMPORT_C extern gboolean _gst_check_threads_running ;
#endif


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
IMPORT_C extern GList *buffers;
#endif


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(thread_list,gstcheck,GList*)
#define thread_list (*GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g)())
#else 
IMPORT_C extern GList *thread_list;
#endif

//GMutex *mutex;
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(mutex,gstcheck,GMutex*)
#define mutex (*GET_GSTREAMER_WSD_VAR_NAME(mutex,gstcheck,g)())
#else 
IMPORT_C extern GMutex *mutex;
#endif

//GCond *start_cond;              /* used to notify main thread of thread startups */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
IMPORT_C extern GCond *start_cond;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
IMPORT_C extern GCond *sync_cond;
#endif
//char* xmlfile = "gstnetclientclock";

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

void test_instantiation()
{
  GstClock *client, *local;
  
  xmlfile = "test_instantiation";
  std_log(LOG_FILENAME_LINE, "Test Started test_instantiation");
  
  local = gst_system_clock_obtain ();
  client = gst_net_client_clock_new (NULL, "127.0.0.1", 1234, GST_SECOND);
  fail_unless (local != NULL, "failed to get system clock");
  fail_unless (client != NULL, "failed to get network client clock");

  /* one for gstreamer, one for us */
  ASSERT_OBJECT_REFCOUNT (local, "system clock", 2);
  ASSERT_OBJECT_REFCOUNT (client, "network client clock", 1);

  gst_object_unref (client);

  ASSERT_OBJECT_REFCOUNT (local, "system clock", 2);

  gst_object_unref (local);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
}



void test_functioning()
{
  GstNetTimeProvider *ntp;
  GstClock *client, *server;
  GstClockTime basex, basey, rate_num, rate_denom;
  GstClockTime servtime, clienttime;
  gint port;

	xmlfile = "test_functioning";
  std_log(LOG_FILENAME_LINE, "Test Started test_functioning");
  
  server = gst_system_clock_obtain ();
  fail_unless (server != NULL, "failed to get system clock");

  /* move the clock ahead 100 seconds */
  gst_clock_get_calibration (server, &basex, &basey, &rate_num, &rate_denom);
  basey += 100 * GST_SECOND;
  gst_clock_set_calibration (server, basex, basey, rate_num, rate_denom);

  ntp = gst_net_time_provider_new (server, "127.0.0.1", 0);
  fail_unless (ntp != NULL, "failed to create network time provider");

  g_object_get (ntp, "port", &port, NULL);

  client = gst_net_client_clock_new (NULL, "127.0.0.1", port, GST_SECOND);
  fail_unless (client != NULL, "failed to get network client clock");

  g_object_get (client, "port", &port, NULL);

  /* let the clocks synchronize */
  g_usleep (G_USEC_PER_SEC * 10);
  
  servtime = gst_clock_get_time (server);
  clienttime = gst_clock_get_time (client);

  /* can't in general make a precise assertion here, because this depends on
   * system load and a lot of things. however within half a second they should
   * at least be within 1/10 of a second of each other... */
   if (servtime > clienttime)
    fail_unless (servtime - clienttime < 100 * GST_MSECOND,
        "clocks not in sync (%" GST_TIME_FORMAT ")",
        GST_TIME_ARGS (servtime - clienttime));
  else
    fail_unless (clienttime - servtime < 100 * GST_MSECOND,
        "clocks not in sync (%" GST_TIME_FORMAT ")",
        GST_TIME_ARGS (clienttime - servtime));
    
  /*
     g_print ("diff: %" GST_TIME_FORMAT,
     GST_TIME_ARGS (servtime > clienttime ? servtime - clienttime
     : clienttime - servtime));
   */

  /* one for gstreamer, one for ntp, one for us */
  ASSERT_OBJECT_REFCOUNT (server, "system clock", 3);
  ASSERT_OBJECT_REFCOUNT (client, "network client clock", 1);

  gst_object_unref (ntp);

  ASSERT_OBJECT_REFCOUNT (server, "system clock", 2);

  gst_object_unref (client);
  gst_object_unref (server);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



//void
//gst_net_client_clock_suite (void)
//{
//test_instantiation();
//test_functioning();
//}

void (*fn[]) (void) = {
test_instantiation,
test_functioning
};

char *args[] = {
"test_instantiation",
"test_functioning"
};

GST_CHECK_MAIN (gst_net_client_clock);
#if 0
int main(int argc,char** argv)
{
		int i;
		gst_check_init(&argc,&argv);
		for( i=0;i<2;i++)
			fn[i]();
			
} 
#endif