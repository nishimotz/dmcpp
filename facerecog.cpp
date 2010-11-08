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

void FaceRecog::detectAndDraw(Mat& img, double scale) 
{
  CascadeClassifier cascade = this->cascade;
  CascadeClassifier nestedCascade = this->cascade;
    int i = 0;
    double t = 0;
    const static Scalar colors[] =  { CV_RGB(0,0,255),
        CV_RGB(0,128,255),
        CV_RGB(0,255,255),
        CV_RGB(0,255,0),
        CV_RGB(255,128,0),
        CV_RGB(255,255,0),
        CV_RGB(255,0,0),
        CV_RGB(255,0,255)} ;
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );

    cvtColor( img, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    t = (double)cvGetTickCount(); 
    cascade.detectMultiScale( smallImg, this->faces,
        1.1, 2, 0
        |CV_HAAR_FIND_BIGGEST_OBJECT
        //|CV_HAAR_DO_ROUGH_SEARCH
        |CV_HAAR_SCALE_IMAGE
        ,
        Size(30, 30) );

   t = (double)cvGetTickCount() - t;
    // fprintf(stderr, "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
    
    for( vector<Rect>::const_iterator r = this->faces.begin(); 
	 r != this->faces.end(); r++, i++ )
    {
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;
        center.x = cvRound((r->x + r->width*0.5)*scale);
        center.y = cvRound((r->y + r->height*0.5)*scale);
        radius = cvRound((r->width + r->height)*0.25*scale);
        circle( img, center, radius, color, 3, 8, 0 );
        if( nestedCascade.empty() )
            continue;
        smallImgROI = smallImg(*r);
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
   	    |CV_HAAR_FIND_BIGGEST_OBJECT
            //|CV_HAAR_DO_ROUGH_SEARCH
            //|CV_HAAR_DO_CANNY_PRUNING
            |CV_HAAR_SCALE_IMAGE
            ,
	Size( 3, 3 ));
        for( vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ )
        {
            center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
            center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
            radius = cvRound((nr->width + nr->height)*0.25*scale);
            //circle( img, center, radius, color, 3, 8, 0 );
	    Point p1, p2;
	    p1.x = (r->x + nr->x)*scale; p2.x = p1.x + nr->width * scale;
	    p1.y = (r->y + nr->y)*scale; p2.y = p1.y + nr->height * scale;
            rectangle( img, p1, p2, color, 3);
        }
    }  
}
