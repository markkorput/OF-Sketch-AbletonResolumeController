#include "ofApp.h"

#include "ofxSuperLog.h"
#include "ofxTimeMeasurements.h"
#include "ofxRemoteUIVars.h"

//--------------------------------------------------------------
// callback function for ofxRemoteUI actions
bool bRecalcNow = true;

void ruiServerCallback(RemoteUIServerCallBackArg arg){
    switch (arg.action) {
        case CLIENT_DID_SET_PRESET:
        case CLIENT_UPDATED_PARAM:
            bRecalcNow = true;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofLogToFile("log.txt", true);
    //ofSetLogLevel(OF_LOG_VERBOSE);

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofSetWindowShape(600, 400);

    TIME_SAMPLE_SET_FRAMERATE(60);
    // TIME_SAMPLE_ENABLE();

    // setup ofxRemoteUI
    RUI_SETUP();
    RUI_SET_CALLBACK(ruiServerCallback);
    // TODO: setup params here
    RUI_NEW_GROUP("App");
    RUI_DEFINE_VAR_WV(bool, "app-fullscreen", false);
    RUI_DEFINE_VAR_WV(string, "OSC_IP", "127.0.01");
    RUI_DEFINE_VAR_WV(int, "OSC_PORT", 9000, 0, 10000);
    RUI_DEFINE_VAR_WV(string, "OSC_address", "/live/play/clip");
    RUI_DEFINE_VAR_WV(string, "OSC_audio_master_volume_msg", "/live/master/volume");
    RUI_DEFINE_VAR_WV(string, "OSC_VIDEO_IP", "127.0.0.1");
    RUI_DEFINE_VAR_WV(int, "OSC_VIDEO_PORT", 7000, 0, 10000);
    RUI_DEFINE_VAR_WV(string, "OSC_video_clip_msg", "/layer1/clip1/connect");
    RUI_DEFINE_VAR_WV(float, "trigger-amplitude", 0.0, 0.0000001, 5.0);
    RUI_DEFINE_VAR_WV(float, "trigger-delay", 0.1, 0.01, 5.0);
    RUI_DEFINE_VAR_WV(int, "alpha-rampdown-layer", 3, 1, 10);
    RUI_DEFINE_VAR_WV(float, "alpha-rampdown-duration", 4.0, 0.1, 20);
    RUI_DEFINE_VAR_WV(float, "alpha-rampdown-start", 1.0, 0.0, 1.0);
    RUI_DEFINE_VAR_WV(float, "alpha-rampdown-end", 0.1, 0.0, 1.0);
    RUI_DEFINE_VAR_WV(float, "volume-rampdown-duration", 2.0, 0.1, 20);
    RUI_DEFINE_VAR_WV(float, "volume-rampdown-start", 0.8, 0.0, 1.0);
    RUI_DEFINE_VAR_WV(float, "volume-rampdown-end", 0.4, 0.0, 1.0);
    RUI_LOAD_FROM_XML();

    //ofSoundStreamListDevices();
    soundListener.setup();
    ofAddListener(soundListener.amplitudeTriggerEvent, this, &ofApp::onAmplitudeTrigger);
}

//--------------------------------------------------------------
void ofApp::update(){
    float dt = (1.f/60.f); // elapsed time (60 fps)

    if(bRecalcNow){
        bRecalcNow = false;
        // TODO: perform post-param-change updates
        ofSetFullscreen(RUI_VAR(bool, "app-fullscreen"));
        oscDirector.setup(RUI_VAR(string, "OSC_IP"), RUI_VAR(int, "OSC_PORT"));
        oscVideoDirector.setup(RUI_VAR(string, "OSC_VIDEO_IP"), RUI_VAR(int, "OSC_VIDEO_PORT"));
    }
    
    soundListener.update(dt);
    oscDirector.update(dt);
    oscVideoDirector.update(dt);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(120), ofColor(170));
    soundListener.draw();
    ofTranslate(0, 130);
    oscDirector.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'f':
            OFX_REMOTEUI_SERVER_SET_VAR(bool, "app-fullscreen", !RUI_VAR(bool, "app-fullscreen"));
            bRecalcNow = true;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::exit(){

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
    if(y > ((float)ofGetWindowHeight()) / 2){
        // oscDirector.playAudio();
        float amp = (float)x / (float)ofGetWindowWidth();
        onAmplitudeTrigger(amp);
    }
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

void ofApp::audioIn(float * output, int bufferSize, int nChannels){
    //LOG << "audio: " << *output;
    soundListener.audioIn(output, bufferSize, nChannels);
}

void ofApp::onAmplitudeTrigger(float &amp){
    LOG << "audio trigger (value: " << amp << ")";
    float volume = ofMap(amp, 0.0, 1.0, RUI_VAR(float, "volume-rampdown-start"), 1.0);
    // oscDirector.setAudioMasterVolume(volume);
    oscDirector.rampdownMasterVolume(volume, RUI_VAR(float, "volume-rampdown-end"), RUI_VAR(float, "volume-rampdown-duration"));
    oscDirector.playAudio();
    //oscVideoDirector.playClip();
    oscVideoDirector.rampdownClipSpeed(0.078f, 0.013f, 8.0f);
    oscVideoDirector.rampdownLayerAlpha(RUI_VAR(int, "alpha-rampdown-layer") /* layer */, RUI_VAR(float, "alpha-rampdown-start") /* start alpha; opaque */, RUI_VAR(float, "alpha-rampdown-end") /* end alpha; transparent */, RUI_VAR(float, "alpha-rampdown-duration") /* duration */);
}