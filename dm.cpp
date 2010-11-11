/**
 * dm.cpp for galatea dialog studio
 * since 2010-06 
 * by Takuya Nishimoto
 * contributed by Hiroki Deguchi, Lu Di
 * http://en.nishimotz.com/project:dmcpp
 * http://ja.nishimotz.com/project:dmcpp
 * based on 
 *  julius-simple.c (julius 4.1.5) by Akinobu Lee
 *  facedetect.cpp (http://sourceforge.net/projects/opencvlibrary/)
 * 日本語文字コード UTF-8 
 */

#include "application.h"
#include "facerecog.h"
#include "sprecog.h"

static Application *app = new Application();
static FaceRecog *faceRecog = new FaceRecog();
static SpRecog *spRecog = new SpRecog();

void *julius_worker( void *my_workorderp )
{
  spRecog->openLogFile();
  spRecog->loadConfigFile(app);
  spRecog->addCallbacks(app);
  spRecog->start(app);
  spRecog->close();
  return NULL;
}

static string cvresult_msg = "hello";

void put_message(Mat &image, string msg)
{
  // void putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false)
  int fontFace = FONT_HERSHEY_PLAIN;
  double fontScale = 2.0;
  int thickness = 2;
  cv::Scalar color(255,0,0); // (b,g,r)
  cv::putText(image, 
	      msg, 
	      cvPoint(30, 30), 
	      fontFace,
	      fontScale,
	      color,
	      thickness
	      );
}

void *cv_worker( void *my_workorderp )
{
  // workorder_t *workorderp = (workorder_t *)my_workorderp;
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
#if 0
    if (faceRecog->faces.size() == 0) {
      app->send("to @FSM set AgentEnable = DISABLE");
    } else {
      app->send("to @FSM set AgentEnable = ENABLE");
    }
#endif
    put_message(frameCopy, app->getCvresultMsg());
    cv::imshow( "opencv result", frameCopy );    
    int k = cv::waitKey( 10 );
    if( k >= 0 ) {
      cerr << "key " << k << " pressed" << endl;
      // goto _cleanup_;
    }
  }
  cv::waitKey(0);
 _cleanup_:
  cvReleaseCapture( &capture );
}

int main(int argc, char *argv[])
{
  workorder_t *workorderp; // TODO: initialize
  create_and_detach_thread(workorderp, julius_worker);
  create_and_detach_thread(workorderp, cv_worker);
  sleep(3);

  app->openLogFile();
  app->send("to @AM-MCL set AutoMove = 1"); // enable face motion
  app->send("to @AM-MCL set Emotion = HAPPY");
  app->agentSpeak("こんにちは。これは長いメッセージです。");

  while (true) {
    app->iteration();
  }
  return(0);
}
