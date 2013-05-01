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
	
    void loadEnviroPolys();
    void saveEnviroPolys();
    
    // Input
    void keyPressed(int key);
    void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
    

private:
    
    // Imaging
	ofVideoGrabber          vidGrabber;
	ofxCvGrayscaleImage         colorImage;  // It is so ugly that "colorImage" is actually a grayscale image right now. sorry...
	ofxCvGrayscaleImage     threImg;
	ofxCvGrayscaleImage     bgImg;
	ofxCvContourFinder      contourFinder;
	
    // Box2D
	ofxBox2d				 physics;
	vector <ofxBox2dCircle>  circles;
	vector <ofxBox2dPolygon> polys;
    vector <ofxBox2dPolygon> enviroPolys;
    vector <ofPolyline>			lines;   // temporarily hold points before creating polyline?
	int						 nbCircles;
    
    bool                     isAutoCirclePopulating;
    
    ofxOpenNI               openNI;
	
	int                      threshold;
	bool                     bLearnBackground;
};

#endif
