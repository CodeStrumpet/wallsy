#include "testApp.h"

void testApp::setup(){
	
	ofBackground(255, 255, 255);
    ofSetFrameRate(60);
	
    colorImage.allocate(CAM_WIDTH, CAM_HEIGHT);
    threImg.allocate(CAM_WIDTH, CAM_HEIGHT);
    bgImg.allocate(CAM_WIDTH, CAM_HEIGHT);
	
	vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);
	
	physics.init();
	physics.createBounds();
	physics.setGravity(0, 10);
	physics.checkBounds(true);
	
	threshold = 10;
	bLearnBackground = true;
}

void testApp::update(){
	
	vidGrabber.update();
	
	if (vidGrabber.isFrameNew())
	{
        colorImage.setFromPixels(vidGrabber.getPixels(), CAM_WIDTH, CAM_HEIGHT);
        colorImage.mirror(false, true);
		
        if(bLearnBackground)
        {
            bgImg = colorImage;
            bLearnBackground = false;
        }
		
        threImg = colorImage;
        threImg.absDiff(bgImg);
        threImg.blur(5);
        threImg.threshold(threshold);
		
        contourFinder.findContours(threImg, 200, (CAM_WIDTH*CAM_HEIGHT)/3, 5, false);
		
		int i;
		
		// update physics
		
		for (i=polys.size()-1; i>=0; i--) {
			physics.world->DestroyBody(polys[i].body);
		}
		
		polys.clear();
        
        for (i=0; i<contourFinder.nBlobs; i++) {
            ofxBox2dPolygon b2poly;
            
            for (int j=0; j<contourFinder.blobs[i].pts.size(); j++) {
                b2poly.addVertex(ofVec2f(contourFinder.blobs[i].pts[j]));
            }
            
            b2poly.triangulate();  
            b2poly.create(physics.world);
            polys.push_back(b2poly);
        }
	}
	
	physics.update();
}

void testApp::draw(){
	
    ofSetColor(255, 255, 255);
    colorImage.draw(CAM_WIDTH, 0);
	
    // Thresholded image + contour finder
	
    ofSetColor(0, 0, 255);
    threImg.draw(0, 0);
    contourFinder.draw(0, 0);
	
    // Triangulation
	
    //triangle.draw(CAM_WIDTH, 0);
	
	// Physics

	int i;
	ofSetColor(0, 255, 0);
	nbCircles = circles.size();
	for(i=0; i<nbCircles; i++) circles[i].draw();
	
    //ofNoFill();
    ofSetColor(255, 0, 0);
	for(i=polys.size()-1; i>=0; i--) {
		polys[i].draw();
	}
	
    // Infos
	
    ofSetColor(0, 0, 0);
    ofDrawBitmapString("FPS : " + ofToString(ofGetFrameRate()), 10, CAM_HEIGHT + 20);
    //ofDrawBitmapString("nb Triangles : " + ofToString(triangle.nTriangles), 10, CAM_HEIGHT + 40);
    ofDrawBitmapString("nb Circles : " + ofToString(nbCircles), 10, CAM_HEIGHT + 60);
}

void testApp::keyPressed(int key){
	
	switch (key) {
			
		case ' ':
			bLearnBackground = true;
			break;
			
		case 's':
			vidGrabber.videoSettings();
			break;
			
		case 'c':
			ofxBox2dCircle circle;
			circle.setPhysics(3.0, 0.53, 0.1);
			circle.setup(physics.getWorld(), mouseX, mouseY, 20);
			circles.push_back(circle);
			break;
	}
}
