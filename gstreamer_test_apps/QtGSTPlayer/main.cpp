/*! This is an example application demonstrating how a simple Hb
  application is created. This application implements a folder
  browser.  The application creates two views. The main view (Folder
  view) shows the folder contents. The second view (Settings view) can
  be used to set the path which is shown in the Folder view.  Settings
  view uses the HbFormView. Folder view inherits HbView and implements
  a couple of methods to populate the Folder view.  */

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtoolbar.h>


#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>

#include "folderview.h"
#include "views.h"

int main(int argc, char *argv[])
{
    // Create HbApplication
    gst_init (&argc, &argv);
    HbApplication a(argc, argv);
    a.setApplicationName(QObject::tr("Folder Browser"));
    //LOG(_L("Entering main.cpp"));

    // Create main window
    HbMainWindow mainWindow;

    // Create View#1 : Folder view
    HbView *folderView = new FolderView;
    // Title pane text
    folderView->setTitle(QObject::tr("QtGSTPlayer"));
 
    // Add two views to main window, 
    // the adding order determines which one is shown first
  
    mainWindow.addView(folderView);
    /*mainWindow.addView(settingsView);*/

    // Store the mainwindow ptr.
    Views::win = &mainWindow;

    // Show main window
    mainWindow.show();

    return a.exec();
}
