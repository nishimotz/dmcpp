#ifndef _facerecog_h_
#define _facerecog_h_

#include <iostream>

// opencv-local
#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace cv;

String cascadeName =
"/usr/local/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";
String nestedCascadeName =
"/usr/local/share/opencv/haarcascades/haarcascade_mcs_eyepair_small.xml";

class FaceRecog {
 public:
  CascadeClassifier cascade, nestedCascade;
  bool loadModels() {
    if( !cascade.load( cascadeName ) ) {
      cerr << "ERROR: Could not load classifier cascade" << endl;
      return false;
    }
    if( !nestedCascade.load( nestedCascadeName ) ) {
      cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
      return false;
    }
    return true;
  }
};

#endif
