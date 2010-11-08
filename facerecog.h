#ifndef _facerecog_h_
#define _facerecog_h_

#include <iostream>

// opencv-local
#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace cv;

class FaceRecog {
 public:
  CascadeClassifier cascade, nestedCascade;
  bool loadModels(void);
  vector<Rect> faces;
};

#endif
