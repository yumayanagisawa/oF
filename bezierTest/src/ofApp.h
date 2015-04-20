#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofSoundPlayer sound;
        float 				* fftSmoothed;
        // int bands to get
        int nBandsToGet;
    
        float radius;
        float radiusTwo;
    
        int wH;
        int wW;
    
        // serial
        ofSerial serial;
    
    float xPoints[15];
    float yPoints[15];
    float * pointArr[6];
    int centerX;
    int centerY;
    
    ofPolyline polyline;
    
		
};
