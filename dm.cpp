/**
 * dm.cpp for galatea dialog studio
 * since 2010-06 
 * by Takuya Nishimoto
 * contributed by Hiroki Deguchi, Lu Di
 * http://ja.nishimotz.com/project:dmcpp
 * based on 
 *  julius-simple.c (julius 4.1.5) by Akinobu Lee
 *  facedetect.cpp (http://sourceforge.net/projects/opencvlibrary/)
 * 日本語文字コード UTF-8 
 */

#include "application.h"
#include "facerecog.h"
#include "sprecog.h"
// #include "util.cpp"

// -----------------------------------------------------------
// thread management
// -----------------------------------------------------------

// static thread_info_t m_thread_info;
static Application *app = new Application();
static FaceRecog *faceRecog = new FaceRecog();

void *julius_worker( void *my_workorderp )
{
  workorder_t *workorderp = (workorder_t *)my_workorderp;

  Jconf *jconf; // Julius configuration parameter holder
  Recog *recog; // Julius recognition instance

  FILE *srm_log_fp; 
  srm_log_fp = fopen("_srm_log.txt", "w"); 
  jlog_set_output(srm_log_fp);

  char conf_file_name[] = "julius.conf";
  jconf = j_config_load_file_new(conf_file_name);
  if (jconf == NULL) {
    fprintf(stderr, "error in j_config_load_file_new\n");
    app->tell("error in j_config_load_file_new");
    return NULL;
  }
  
  recog = j_create_instance_from_jconf(jconf);
  if (recog == NULL) {
    app->tell("error in j_create_instance_from_jconf");
    return NULL;
  }

#if 0
  callback_add(recog, CALLBACK_EVENT_SPEECH_READY, status_recready, (void *)app);
  callback_add(recog, CALLBACK_EVENT_SPEECH_START, status_recstart, (void *)app);
  callback_add(recog, CALLBACK_RESULT, output_result, (void *)app);
  callback_add(recog, CALLBACK_RESULT_PASS1_INTERIM, result_pass1_current, (void *)app);
#else
  add_callbacks(recog, app);
#endif
  if (j_adin_init(recog) == FALSE) {    
    app->tell("error in j_adin_init");
    return NULL;
  }

  j_recog_info(recog); /* output system information to log */

  switch(j_open_stream(recog, NULL)) {
  case 0:			
    app->tell("ok j_open_stream");
    break;
  case -1:      		
    app->tell("error in input stream");
    return NULL;
  case -2:
    app->tell("error in beginning input");
    return NULL;
  }
  fflush(srm_log_fp);

  int ret = j_recognize_stream(recog);
  if (ret == -1) {
    app->tell("error j_recognize_stream");
    return NULL;
  }

  j_close_stream(recog);
  j_recog_free(recog);

  free(workorderp);
  return NULL;
}


void *cv_worker( void *my_workorderp )
{
  workorder_t *workorderp = (workorder_t *)my_workorderp;
  faceRecog->loadModels();
  double scale = 4;
  CvCapture* capture = cvCaptureFromCAM(0);
  cvNamedWindow( "opencv result", 1 );
  for(;;) {
    IplImage* iplImg = cvQueryFrame( capture );
    Mat frame, frameCopy;
    frame = iplImg;
    if( frame.empty() )
      break;
    if( iplImg->origin == IPL_ORIGIN_TL )
      frame.copyTo( frameCopy );
    else
      flip( frame, frameCopy, 0 );
    faceRecog->detectAndDraw( frameCopy, scale );
    if (faceRecog->faces.size() == 0) {
      app->send("to @FSM set AgentEnable = DISABLE");
    } else {
      app->send("to @FSM set AgentEnable = ENABLE");
    }
    cv::imshow( "opencv result", frameCopy );    
    int k = waitKey( 10 );
    if( k >= 0 ) {
      cerr << "key " << k << " pressed" << endl;
      // goto _cleanup_;
    }
  }
  waitKey(0);
 _cleanup_:
  cvReleaseCapture( &capture );
}

// -----------------------------------------------------------

int
main(int argc, char *argv[])
{
  sleep(3);

  workorder_t *workorderp; // TODO: initialize

  create_and_detach_thread(workorderp, julius_worker);
  create_and_detach_thread(workorderp, cv_worker);

  FILE *dm_log_fp = fopen("_dm_log.txt", "w"); 

  app->send("to @AM-MCL set AutoMove = 1"); // enable face motion
  app->send("to @AM-MCL set Emotion = HAPPY");
  app->send("to @AM-MCL set Speak = こんにちは。これは長いメッセージです。");

  for (int n = 0 ; ; n++) {
    const int bufsize = 100000;
    char ibuf[bufsize], obuf[bufsize];
    char *ret = fgets( ibuf, bufsize, stdin );
    chomp(ibuf);
    sprintf(obuf, "%d %s", n, ibuf);
    // app->tell(buf);
    if (starts_with(ibuf, "From @SSM rep Speak.stat = IDLE")) { 
      fprintf(stderr, "%s\n", ibuf); fflush(stderr);
    }
    fprintf(dm_log_fp, "%s\n", obuf); 
    fflush(dm_log_fp);
  }
  return(0);
}
