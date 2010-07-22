#include <QModelIndex>
#include <QDirModel>
#include <QApplication>

//#include <hbformitem.h>
//#include <hbformview.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <hbmainwindow.h>

#include <QDebug>
#include <QAction>
#include <HbAction>
#include "folderview.h"
#include "views.h"
#include <HbAbstractViewItem>
#include <QTimer>
#include <hbnotificationdialog.h>
/*! Constructor of FolderView. 
 */
#ifdef __cplusplus
extern "C" 
{
#endif

int mCurrentState = 0;

#ifdef __cplusplus
}
#endif
//extern int mCurrentState;

FolderView::FolderView(QGraphicsItem *parent)
        : HbView(parent),mModel( NULL ), mFolderPath("c:\\data\\Sounds\\Digital\\"),mTimer(NULL),mIsRecording( FALSE )
{
    mCurrentState = NONE;
   
    // Create a main widget for this view 
    // In this case, list view is appropriate
    // Note: HbListView is not derived from HbView,
    // this is just like any other Hb widget but with a feature of 
    // model-view separation
    mFileList = new HbListView(this);

    // Create a model to set for the list view
    mModel = new QDirModel(this);
    mFileList->setModel(mModel);
    
    // Setting initial path for folder list
    mFileList->setRootIndex(mModel->index(mFolderPath));
    //mFileList->
    // Setting the main widget for this view
    setWidget(mFileList);
    //int x = mFileList->indexCount();    
    QObject::connect(mFileList, SIGNAL(pressed(const QModelIndex)), this, SLOT(item_pressed(const QModelIndex)));

    // add menu
    HbMenu* menu1 = menu()->addMenu(tr("Record"));

    HbAction* menu1_Action = new HbAction(tr("RAW"), this);
    connect(menu1_Action, SIGNAL(triggered()), this, SLOT(record_RAW()));
    menu1->addAction( menu1_Action );

    menu1_Action = new HbAction(tr("WAV"), this);
    connect(menu1_Action, SIGNAL(triggered()), this, SLOT(record_WAV()));
    menu1->addAction( menu1_Action );

    //create action Nokia India sub menu item.
    menu1_Action = new HbAction(tr("AMR"), this);
    connect(menu1_Action, SIGNAL(triggered()), this, SLOT(record_AMR()));
    menu1->addAction( menu1_Action );

    menu1_Action = new HbAction(tr("G711"), this);
    connect(menu1_Action, SIGNAL(triggered()), this, SLOT(record_G711()));
    menu1->addAction( menu1_Action );
    
    menu1_Action = new HbAction(tr("AAC"), this);
    connect(menu1_Action, SIGNAL(triggered()), this, SLOT(record_AAC()));
    menu1->addAction( menu1_Action );
    menu1_Action = new HbAction(tr("G729"), this);
    connect(menu1_Action, SIGNAL(triggered()), this, SLOT(record_G729()));
    menu1->addAction( menu1_Action );

    menu1_Action = new HbAction(tr("ILBC"), this);
    connect(menu1_Action, SIGNAL(triggered()), this, SLOT(record_ILBC()));
    menu1->addAction( menu1_Action );
    
   

    //menu()->addAction(tr("Play"), this, SLOT(play()));
    menu()->addAction(tr("Stop"), this, SLOT(stop()));
    menu()->addAction(tr("Pause"), this, SLOT(pause()));
    menu()->addAction(tr("Resume"), this, SLOT(resume()));
    menu()->addAction(tr("Exit"), qApp, SLOT(quit()));
    
    
    /// timer required to get the glib events
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(timertimeout()));
    mTimer->start(10);

}

FolderView::~FolderView()
{
    if(mCurrentState != NONE)
        gst_unref();
delete mFileList;
delete mModel;
mTimer->stop();
delete mTimer;
}


void FolderView::folderViewTriggered()
{
    mainWindow()->setCurrentView(Views::folderView());
}

void FolderView::showNote(const int err)
{
    if(err)
    {
        HbNotificationDialog* notifyDialog = new HbNotificationDialog;
        notifyDialog->setTimeout(HbPopup::StandardTimeout);
        QString strong;
        QString sprint= strong.sprintf("Format Not supported(%d)",err);
        notifyDialog->setTitleTextWrapping(Hb::TextWordWrap);
        notifyDialog->setTitle(QString("Error"));
        notifyDialog->setWrapMode(Hb::TextWordWrap);
        notifyDialog->setText(sprint);
        
        notifyDialog->exec();
        delete notifyDialog;
    }
}
void FolderView::item_pressed(const QModelIndex &index)
{
    if( mIsRecording )
    {
        return;
    }
    QVariant variant=index.data();
    QString str = variant.toString();
    
    
    QString fullpath = mFolderPath;
    fullpath.append( str );
    
    TBuf16<1024> buf16;
    buf16.Copy( (TUint16*)fullpath.data_ptr()->data ,fullpath.data_ptr()->size );
    TBuf8<1024> buf8;
    buf8.Copy( buf16 );

    if( NONE != mCurrentState )
    {
    gst_unref();
    }
   
    int err = gst_play_file( (char*)buf8.PtrZ() );
    if(err)
    {
        showNote(err);
        mCurrentState = NONE;
    }
    else
    {
        mCurrentState = PLAYING;
    }   

}
void FolderView::pause()
{
    if( PLAYING == mCurrentState  ){
    gst_pause();
    mCurrentState = PAUSE;
    }
}

void FolderView::resume()
{
    if( PAUSE == mCurrentState  ){
    gst_resume();
    mCurrentState = PLAYING;
    }
}

void FolderView::stop()
{
    if( mIsRecording == TRUE )
    {
        gst_record_stop();
    
        if( mModel )
            delete mModel;
        
        mModel = new QDirModel(this);
        mFileList->setModel(mModel);

        mFileList->reset();
        mFileList->setRootIndex(mModel->index(mFolderPath));
//        mFileList->show();
//      int x = mFileList->indexCount();
        //setWidget(mFileList);
        mIsRecording = FALSE;
        return;
    }
    if( PLAYING == mCurrentState || PAUSE == mCurrentState ){
    gst_unref();
    mCurrentState = NONE;
    }
}

void FolderView::record_AMR()
{
    if( mCurrentState == NONE ){
        int err = gst_record_file( RECORD_AMR );
        if(err)
        {
            showNote(err);
            mCurrentState = NONE;
            mIsRecording = FALSE;
        }
        else
        {
            mCurrentState = PLAYING;
            mIsRecording = TRUE;
        }
    }
}

void FolderView::record_WAV()
{
    if( mCurrentState == NONE ){
        int err = gst_record_file( RECORD_WAV );
        if(err)
        {
            showNote(err);
            mCurrentState = NONE;
            mIsRecording = FALSE;
        }
        else
        {
            mCurrentState = PLAYING;
            mIsRecording = TRUE;
        }
        //gst_record_wav();
    }    
}

void FolderView::record_RAW()
{
    if( mCurrentState == NONE ){
        int err = gst_record_file( RECORD_RAW );
        if(err)
        {
            showNote(err);
            mCurrentState = NONE;
            mIsRecording = FALSE;
        }
        else
        {
            mCurrentState = PLAYING;
            mIsRecording = TRUE;
        }
    }
}

void FolderView::record_G711()
{
    if( mCurrentState == NONE ){
        int err = gst_record_file( RECORD_G711 );
        if(err)
        {
            showNote(err);
            mCurrentState = NONE;
            mIsRecording = FALSE;
        }
        else
        {
            mCurrentState = PLAYING;
            mIsRecording = TRUE;
        }
    }
}

void FolderView::record_G729()
{
    if( mCurrentState == NONE ){
        int err = gst_record_file( RECORD_G729 );
        if(err)
        {
            showNote(err);
            mCurrentState = NONE;
            mIsRecording = FALSE;
        }
        else
        {
            mCurrentState = PLAYING;
            mIsRecording = TRUE;
        }
    }
}

void FolderView::record_ILBC()
{
    if( mCurrentState == NONE ){
        int err = gst_record_file( RECORD_ILBC );
        if(err)
        {
            showNote(err);
            mCurrentState = NONE;
            mIsRecording = FALSE;
        }
        else
        {
            mCurrentState = PLAYING;
            mIsRecording = TRUE;
        }
    }
}

void FolderView::record_AAC()
{
    if( mCurrentState == NONE ){
        int err = gst_record_file( RECORD_AAC );
        if(err)
        {
            showNote(err);
            mCurrentState = NONE;
            mIsRecording = FALSE;
        }
        else
        {
            mCurrentState = PLAYING;
            mIsRecording = TRUE;
        }
    }
}
void FolderView::timertimeout()
{
    if( PLAYING == mCurrentState ){
    gst_get_events();
    }
    
    if( STOP == mCurrentState ){
        gst_unref();
        mCurrentState = NONE;
        mIsRecording = FALSE;
    }
}
