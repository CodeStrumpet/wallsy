#include "testApp.h"

#define USE_OPENNI 1

void testApp::setup(){
	
	ofBackground(255, 255, 255);
    ofSetFrameRate(60);
	
    colorImage.allocate(CAM_WIDTH, CAM_HEIGHT);
    threImg.allocate(CAM_WIDTH, CAM_HEIGHT);
    bgImg.allocate(CAM_WIDTH, CAM_HEIGHT);
	
    if (USE_OPENNI) {
        // setup openni
        openNI.setup();
        openNI.addDepthGenerator();
        openNI.addImageGenerator();
        openNI.setRegister(true);
        openNI.setMirror(true);
        openNI.addUserGenerator();
        openNI.setMaxNumUsers(2);
        openNI.setDepthColoring(COLORING_GREY);

        openNI.start();
        
    } else {
        vidGrabber.setDeviceID(0);
        vidGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);        
    }
	
    // setup box2d
	physics.init();
	physics.createBounds();
	physics.setGravity(0, 10);
	physics.checkBounds(true);
    
    isAutoCirclePopulating = true;
    
    // load polygons from string file
    loadEnviroPolys();
    
    
	
	threshold = 10;
	bLearnBackground = true;
}

void testApp::update(){
    if (USE_OPENNI) {
        openNI.update();
    } else {
        vidGrabber.update();
    }
	
	if (vidGrabber.isFrameNew() || openNI.isNewFrame()) 
	{
        
        if (USE_OPENNI /*&& (openNI.isInfraOn() || openNI.isPlaying())*/) {

            colorImage.setFromPixels(openNI.getDepthPixels().getChannel(0));
            
        } else if (vidGrabber.isInitialized()) {
            colorImage.setFromPixels(vidGrabber.getPixels(), CAM_WIDTH, CAM_HEIGHT);
        }
        
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
    
    // add some circles every so often if enabled
	if(isAutoCirclePopulating   &&  (int)ofRandom(0, 10) == 0) {
		ofxBox2dCircle c;
		c.setPhysics(1, 0.5, 0.1);
		c.setup(physics.getWorld(), mouseX, mouseY, ofRandom(3, 10));
		circles.push_back(c);
	}

	
	physics.update();
}

void testApp::draw(){

	
    ofSetColor(255, 255, 255);
    ofSetLineWidth(1);
    
    colorImage.draw(CAM_WIDTH, 0);
	
    // Thresholded image + contour finder
	
    ofSetHexColor(0x1826B0);
    threImg.draw(0, 0);
    //contourFinder.draw(0, 0);
	
    // Triangulation
	
    //triangle.draw(CAM_WIDTH, 0);
	
	// Physics

	int i;
    ofFill();
	nbCircles = circles.size();
	for(i=0; i<nbCircles; i++) {
        ofSetHexColor(0xFFE200);
        circles[i].draw();
        //9303A7
    }
    ofNoFill();
    for(i=0; i<nbCircles; i++) {
        ofSetHexColor(0x9303A7);
        circles[i].draw();
    }
	
    ofNoFill();
    ofSetHexColor(0x2E3784);
	for(i=polys.size()-1; i>=0; i--) {
		polys[i].draw();
	}
    
    // Draw enviroPolys and lines
	ofSetHexColor(0x4C59D8);
    ofSetLineWidth(4);
	ofNoFill();
	for (int i=0; i<lines.size(); i++) {
		lines[i].draw();
	}
	for (int i=0; i<enviroPolys.size(); i++) {
		enviroPolys[i].draw();
	}

    
    

    // Infos
	
    ofSetColor(0, 0, 0);
    ofDrawBitmapString("FPS : " + ofToString(ofGetFrameRate()), 10, CAM_HEIGHT + 20);
    //ofDrawBitmapString("nb Triangles : " + ofToString(triangle.nTriangles), 10, CAM_HEIGHT + 40);
    ofDrawBitmapString("nb Circles : " + ofToString(nbCircles), 10, CAM_HEIGHT + 60);
    
}

void testApp::loadEnviroPolys() {
    // load the lines we saved...
	ifstream f;
	f.open(ofToDataPath("lines.txt").c_str());
	vector <string> strLines;
	while (!f.eof()) {
		string ptStr;
		getline(f, ptStr);
		strLines.push_back(ptStr);
	}
	f.close();
	
	for (int i=0; i<strLines.size(); i++) {
		vector <string> pts = ofSplitString(strLines[i], ",");
		if(pts.size() > 0) {
			ofxBox2dPolygon poly;
			for (int j=0; j<pts.size(); j+=2) {
				if(pts[j].size() > 0) {
					float x = ofToFloat(pts[j]);
					float y = ofToFloat(pts[j+1]);
					poly.addVertex(x, y);
				}
			}
			poly.create(physics.getWorld());
			enviroPolys.push_back(poly);
		}
	}
}

void testApp::saveEnviroPolys() {
    ofstream f;
    f.clear();
    f.open(ofToDataPath("lines.txt").c_str());
    for (int i=0; i<lines.size(); i++) {
        for (int j=0; j<lines[i].size(); j++) {
            float x = lines[i][j].x;
            float y = lines[i][j].y;
            f << x << "," << y << ",";
        }
        f << "\n";
    }
    f.close();lines.clear();
}


void testApp::keyPressed(int key){
	
	switch (key) {
            
        case 'p':
            openNI.startPlayer("test.oni");
            break;
            
        case 'r':  //reset enviroPolys            
            lines.clear();
            for (int i=0; i<enviroPolys.size(); i++) {
                enviroPolys[i].destroy();
            }
            break;
            
		case ' ':
			bLearnBackground = true;
			break;
			
		case 's': // save enviroPolys
            saveEnviroPolys();
			break;
			
		case 'c':
            isAutoCirclePopulating = !isAutoCirclePopulating;            
            break;
            
        }
    
}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	lines.back().addVertex(x, y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	lines.push_back(ofPolyline());
	lines.back().addVertex(x, y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
	
	ofxBox2dPolygon poly;
	lines.back().simplify();
	
	for (int i=0; i<lines.back().size(); i++) {
		poly.addVertex(lines.back()[i]);
	}
	
	//poly.setPhysics(1, .2, 1);  // uncomment this to see it fall!
	poly.create(physics.getWorld());
	enviroPolys.push_back(poly);
	
	//lines.clear();
}

