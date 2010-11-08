#include "facerecog.h"

bool FaceRecog::loadModels(void) 
{
  const string cascadeName =
    "/usr/local/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";
  const string nestedCascadeName =
    "/usr/local/share/opencv/haarcascades/haarcascade_mcs_eyepair_small.xml";
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

