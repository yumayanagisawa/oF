#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    sound.loadSound("sounds/koto.mp3");
    sound.setVolume(0.4);
    sound.play();
    
    nBandsToGet = 128;
    fftSmoothed = new float[128];
    
    for (int i = 0; i < 128; i++){
        fftSmoothed[i] = 0;
    }
    
    wH = ofGetWindowHeight();
    wH = wH / 2;
    wW = ofGetWindowWidth();
    wW = wW / 2;
    
    // serial
    //serial.setup("dev/cu.usbmodem1451", 8080);
    radius = 150;
    float radians;
    float x;
    float y;
    centerX = ofGetWindowWidth();
    centerX = centerX / 2;
    centerY = ofGetWindowHeight();
    centerY = centerY / 2;
    cout << centerX;
    for (int degree=0; degree<15; degree++) {
        radians = degree * 24 * M_PI/180;
        x = centerX + radius * cos(radians);
        y = centerY + radius * sin(radians);
        xPoints[degree] = x;
        yPoints[degree] = y;
    }
    cout << xPoints[0];
    
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(0,0,0);
    // update
    //ofSoundUpdate();
	fftSmoothed = ofSoundGetSpectrum(nBandsToGet);
    // assign value
    radius = fftSmoothed[0];
    radiusTwo = fftSmoothed[10];
    //cout << fftSmoothed[0] << endl;
}

//--------------------------------------------------------------
float * calc(float x1, float y1, float x2, float y2, float centerX, float radius){
    // calc the mid point first
    float midX = (x1 + x2) / 2;
    float midY = (y1 + y2) / 2;
    // get slope & the counterpart
    float slope = (y2 - y1) / (x2 - x1);
    float anotherSlope = -1 / slope;
    float b = midY - anotherSlope * midX;
    // a2 + b2 = c2 >> val2 + (anotherSlope * val)2 = c2
    float val = (700 + radius * 1000) / (1 + pow(anotherSlope, 2));
    //cout << val << endl;
    float sqrtVal = sqrt(val);
    //cout << sqrtVal << endl;

    float xPos;
    float yPos;
    if(centerX < midX) {
        xPos = midX + sqrtVal;
        yPos = midY + (sqrtVal * anotherSlope);
    } else {
        xPos = midX - sqrtVal;
        yPos = midY - (sqrtVal * anotherSlope);
    }
    float posArr[6];
    posArr[0] = x1;
    posArr[1] = y1;
    posArr[2] = xPos;
    posArr[3] = yPos;
    posArr[4] = x2;
    posArr[5] = y2;
    return posArr;
}
void ofApp::draw(){
    ofEnableAlphaBlending();
    ofSetColor(255,255,255,100);
    ofCircle(wW, wH, radius * 100);
    ofSetColor(0, 255, 255, 100);
    ofCircle(wW, wH, radiusTwo * 100);
    
    // poly line
    //cout << radius << endl;
    //cout << xPoints[0] << endl;
    //cout << xPoints[1] << endl;
    for (int i=0; i < 15; i++) {
        int target = i + 1;
        if (target == 15) {
            target = 0;
        }
        ofLine(xPoints[i], yPoints[i], xPoints[target], yPoints[target]);
        float * pointArr = calc(xPoints[i], yPoints[i], xPoints[target], yPoints[target], centerX, radius);
        //cout << pointArr[0] << endl;
        //cout << pointArr[1] << endl;
        //cout << pointArr[2] << endl;
        //cout << pointArr[3] << endl;
        //cout << pointArr[4] << endl;
        //cout << pointArr[5] << endl;
        //cout << "what is wrong" << endl;
        polyline.quadBezierTo(pointArr[0], pointArr[1], pointArr[2], pointArr[3], pointArr[4], pointArr[5]);
    }
    polyline.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
