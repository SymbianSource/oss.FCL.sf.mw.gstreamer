#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <hbview.h>
//#include <hbformview.h>

#include "record_play.h"

QT_BEGIN_NAMESPACE
class QDirModel;
QT_END_NAMESPACE

//class HbFormItem;
class HbListView;

class FolderView : public HbView
{
    Q_OBJECT

public:
    explicit FolderView(QGraphicsItem *parent = 0);
    ~FolderView();
public slots:
    void folderViewTriggered();
    //void settingsViewTriggered();
    void item_pressed(const QModelIndex &index);
//
    void record_AMR();
    void record_WAV();
    void record_RAW();
    void record_G711();
    void record_G729();
    void record_ILBC();
    void record_AAC();

    void pause();
    void resume();
    void stop();
    void timertimeout();
    void showNote(const int err);

//void record_file( int type );


private:
    HbListView *mFileList;

//signals:
//    void pressed(const QModelIndex &index);
//    void released(const QModelIndex &index);
//    void activated(const QModelIndex &index);
//    void longPressed(HbAbstractViewItem *item, const QPointF &coords);

    QDirModel *mModel;
    QString mFolderPath;
    QTimer *mTimer;
    bool mIsRecording;
    
    
    //state st;

};

#endif // FOLDERVIEW_H
