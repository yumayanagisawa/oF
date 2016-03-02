#include "ofApp.h"

ofFloatImage spectrumImage;
const int N = 512;
float spectrum[N];
float originalVals[N];

//--------------------------------------------------------------
void ofApp::setup(){
    // initially shader type is 0
    shaderType = 3;
    ofHideCursor();
    ofDisableArbTex();    // load image
    ofLoadImage(texture, "texturetastic_gray.png");
    ofLoadImage(noiseTex, "noise_texture_0001.png");
    noiseTex.setTextureWrap(GL_REPEAT, GL_REPEAT);
    
    isFFTShader = true;
    ofSetFrameRate(60);
    receiver.setup(PORT);
    //Set spectrum values to 0
    for (int i=0; i<N; i++) {
        spectrum[i] = 0.0f;
    }
    
    shader.load("", "shaders/raymarch.frag");
    shaderB.load("", "shaders/water.frag");
    shaderC.load("", "shaders/cube.frag");
    //shaderD.load("", "shaders/volumetric_lines.frag");
    shaderD.load("", "shaders/sinus.frag");
    shaderE.load("", "shaders/weird_warp.frag");
    shaderF.load("", "shaders/isolines.frag");
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        //cout << "got" << endl;
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // check for mouse moved message
        if(m.getAddress() == "/fftAddress"){
            for (int i = 0; i < N; i++) {
                originalVals[i] = m.getArgAsFloat(i);
            }
        }

        
    }
    float *val = originalVals;
    //cout << val[10] << endl;
    //cout << originalVals[10] << endl;
    for ( int i=0; i<N; i++ ) {
        //spectrum[i] *= 0.95; //0.97;	//Slow decreasing
        spectrum[i] *= 0.98;
        spectrum[i] = max( spectrum[i], val[i]*50.0f ); // c++ thing "max()" val[i]*"SOMEMULTIPLY" the value might change based on input audio
    }
    
    //Set spectrum to spectrumImage
    //spectrumImage.setFromPixels( spectrum, N, 1, OF_IMAGE_GRAYSCALE );
    spectrumImage.setFromPixels( spectrum, N, 1, OF_IMAGE_GRAYSCALE );
    //spectrumImage.setFromPixels( spectrum, N, 1, OF_IMAGE_COLOR );

}

//--------------------------------------------------------------
void ofApp::draw(){
    if (shaderType == 0) {
        // shader!
        shader.begin();
        shader.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        shader.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
        shader.setUniformTexture("iChannel0", spectrumImage.getTexture(), 0);
        //shader.setUniformTexture("iChannel1", spectrumImage.getTexture(), 1);
        shader.setUniformTexture("iChannel1", texture, 1);
        shader.setUniform4f("iMouse", mouseX, mouseY, 0, 0);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        shader.end();
    } else if(shaderType == 1){
        shaderB.begin();
        shaderB.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        shaderB.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        shaderB.end();
    } else if(shaderType == 2) {
        shaderC.begin();
        shaderC.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        shaderC.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
        shaderC.setUniformTexture("iChannel1", spectrumImage.getTexture(), 1);
        //shader.setUniformTexture("iChannel1", spectrumImage.getTexture(), 1);
        shaderC.setUniformTexture("iChannel0", noiseTex, 0);
        shaderC.setUniformTexture("iChannel2", noiseTex, 2);
        //shaderC.setUniform4f("iMouse", mouseX, mouseY, 0, 0);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        shaderC.end();
    } else if(shaderType == 3){
        shaderD.begin();
        shaderD.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        //iChannelTime[0]
        //shaderD.setUniform1f("iChannelTime[0]", ofGetElapsedTimef());
        shaderD.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
        shaderD.setUniformTexture("iChannel0", spectrumImage.getTexture(), 0);
        //shader.setUniformTexture("iChannel1", spectrumImage.getTexture(), 1);
        //shaderD.setUniformTexture("iChannel1", texture, 1);
        //shaderD.setUniform4f("iMouse", mouseX, mouseY, 0, 0);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        shaderD.end();
    } else if(shaderType == 4){
        shaderE.begin();
        shaderE.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        shaderE.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
        shaderE.setUniformTexture("iChannel0", spectrumImage.getTexture(), 0);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        shaderE.end();
    } else if(shaderType == 5){
        shaderF.begin();
        shaderF.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        shaderF.setUniform1f("iChannelTime[0]", ofGetElapsedTimef());
        shaderF.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
        shaderF.setUniformTexture("iChannel0", spectrumImage.getTexture(), 0);
        shaderF.setUniformTexture("iChannel1", spectrumImage.getTexture(), 1);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        shaderF.end();
    }
    // draw spectrum?
    //spectrumImage.draw(0, 0, 256, 256);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == '0') {
        //isFFTShader = false;
        shaderType = 0;
    } else if(key == '1') {
        //isFFTShader = true;
        shaderType = 1;
    } else if(key == '2'){
        shaderType = 2;
    } else if(key == '3'){
        shaderType = 3;
    } else if(key == '4'){
        shaderType = 4;
    } else if(key == '5'){
        shaderType = 5;
    }

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
