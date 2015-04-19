#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    sound.loadSound("sounds/koto.mp3");
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
void ofApp::draw(){
    ofEnableAlphaBlending();
    ofSetColor(255,255,255,100);
    ofCircle(wW, wH, radius * 100);
    ofSetColor(0, 255, 255, 100);
    ofCircle(wW, wH, radiusTwo * 100);
    cout << radius << endl;
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
