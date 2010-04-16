#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>

int
main (int argc, char *argv[])
{
  // To Update registry set GST_REGISTRY_UPDATE to yes.
  // by default it will not update the registry..
  // if the registry is not present it will create registry.
  setenv("GST_REGISTRY_UPDATE", "yes", 0);
  //This will update registry
  gst_init (&argc, &argv);  
  gst_deinit();
  return 0;
}
