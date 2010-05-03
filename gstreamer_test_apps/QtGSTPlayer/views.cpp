#include <hbmainwindow.h>
#include <hbview.h>
//#include <hbformview.h>

#include "views.h"

// Helper methods for retrieving the views used in this application
namespace Views {

    // Store the mainwindow ptr here because we cannot call HbWidget::mainWindow() from these static functions.
    HbMainWindow *win;
    
/*!
  Returns the pointer to folder view.
*/
    HbView *folderView()
    {
    	// 0 since folderView was added first to the main window
        return win->viewAt(0);
    }

/*!
  Returns the pointer to settings view.
*/
  /*  HbFormView *settingsView()
    {
	    // 1 since settingsView was the second view added
        return static_cast<HbFormView *>(win->viewAt(1));
    }
    */
}
