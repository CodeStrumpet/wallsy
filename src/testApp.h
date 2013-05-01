#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxBox2d.h"
#include "ofxOpenNI.h"

#define CAM_WIDTH      640
#define CAM_HEIGHT     480

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);

private:
	ofVideoGrabber          vidGrabber;
	ofxCvGrayscaleImage         colorImage;
	ofxCvGrayscaleImage     threImg;
	ofxCvGrayscaleImage     bgImg;
	ofxCvContourFinder      contourFinder;
	
	ofxBox2d				 physics;
	vector <ofxBox2dCircle>  circles;
	vector <ofxBox2dPolygon> polys;
	int						 nbCircles;
    
    ofxOpenNI               openNI;
	
	int                      threshold;
	bool                     bLearnBackground;
};

#endif
