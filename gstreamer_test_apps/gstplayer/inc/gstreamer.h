#include <e32def.h>
#include <e32cmn.h>
#include <gst/gst.h>
#include <string.h>
#include "gstplayerappview.h"

extern GstElement *pipeline, *source, *wavparse,*sink,*conv,*resample,*decoder;
extern GstBus *bus;

extern CGSTPlayerAppView *iGstView;


int GstreamerNew(TFileName filename);
int GstreamerPlay(TFileName filename);
int gst_play_mp3();
int gst_play_wave();
int gst_play_raw();
int gst_play_amr();
int gst_play_g711();
int gst_play_g729();
int gst_play_ilbc();

int gst_record_raw();
int gst_record_wav();
int gst_record_amr();
int gst_record_g711();
int gst_record_aac();
int gst_record_g729();
int gst_record_ilbc();
int gst_current_volume();
int gst_max_volume();
int gst_volume_up();
int gst_volume_down();

void ShownoteL();

int gst_current_gain();
int gst_max_gain();
int gst_gain_up();
int gst_gain_down();

int gst_balance();
void samplesplayed();
void samplesrecorded();
void samplesrecorded();
void getsinkpadcaps();
void negotiatedsinkcaps();
void getsrcpadcaps();
void negotiatedsrccaps();


