//
//  OscDirector.cpp
//  BaseApp
//
//  Created by Mark van de Korput on 30/03/15.
//
//

#include "OscDirector.h"
#include "ofxSuperLog.h"
#include "ofxRemoteUIVars.h"

OscDirector::OscDirector(string url, int port){
    setup(url, port);
    audioVolume.set(0.2);
}

void OscDirector::setup(string url, int port){
    oscUrl = url;
    oscPort = port;
    oscSender.setup(url, port);
    setupConfigurables();
    volumePlot = new ofxHistoryPlot( NULL, "volume", 400, false); //NULL cos we don't want it to auto-update. confirmed by "true"
    //volumePlot->setAutoRangeShrinksBack(true);
    volumePlot->setRange(0.0, 1.2);
}

void OscDirector::setupConfigurables(){
    masterAudioVolumeMsg = RUI_VAR(string, "OSC_audio_master_volume_msg");
    playAudioMsg = RUI_VAR(string, "OSC_address");
    playClipMsg = RUI_VAR(string, "OSC_video_clip_msg");
}

void OscDirector::update(float dt){
    if(rampdownSpeedAnim.isAnimating()){
        rampdownSpeedAnim.update(dt);
        setClipSpeed(rampdownSpeedAnim.val());
        if(!rampdownSpeedAnim.isAnimating()){
            LOG << "video speed rampdown ended";
        }
    }

    if(rampdownAlphaAnim.isAnimating()){
        rampdownAlphaAnim.update(dt);
        setLayerAlpha(rampdownAlphaLayer, rampdownAlphaAnim.val());
        if(!rampdownAlphaAnim.isAnimating()){
            LOG << "video alpha rampdown complete";
        }
    }
    
    if(rampdownVolumeAnim.isAnimating()){
        rampdownVolumeAnim.update(dt);
        setAudioMasterVolume(rampdownVolumeAnim.val());
        if(!rampdownVolumeAnim.isAnimating()){
            LOG << "volume rampdown complete";
        }
    }
    volumePlot->update(audioVolume.get());
}

void OscDirector::draw(){
    ofSetColor(255);
    volumePlot->draw(0, 0);
}

void OscDirector::playAudio(){
    ofxOscMessage msg;
    msg.setAddress(playAudioMsg);
    msg.addIntArg(1);
    msg.addIntArg(1);
//    try{
        oscSender.sendMessage(msg);
//    }catch(exception e){
//        LOG << "exception: " << e.what();
//    }
    LOG << "playAudio message sent";
}

void OscDirector::setAudioMasterVolume(float volume){
    audioVolume.set(volume);

    ofxOscMessage msg;
    msg.setAddress(masterAudioVolumeMsg);
    // msg.addIntArg(1);
    msg.addFloatArg(audioVolume.get());
//    try{
        oscSender.sendMessage(msg);
//    }catch(exception e){
//        LOG << "exception: " << e.what();
//    }
    LOG << "Audio master volume message sent: " << volume;
}

void OscDirector::playClip(){
    ofxOscMessage msg;
    msg.setAddress(playClipMsg);
    msg.addIntArg(1);
    oscSender.sendMessage(msg);
    LOG << "Send video clip message";
}

void OscDirector::setClipSpeed(float speed){
    ofxOscMessage msg;
    msg.setAddress("/layer1/clip1/video/position/speed");
    speed = ofClamp(speed, 0.0f, 1.0f);
    msg.addFloatArg(speed);
    oscSender.sendMessage(msg);
    // LOG << "Updated clip speed: " << speed;
}

void OscDirector::setLayerAlpha(int layer, float alpha){
    ofxOscMessage msg;
    msg.setAddress("/layer"+ofToString(layer)+"/video/opacity/values");
    msg.addFloatArg(ofClamp(alpha, 0.0f, 1.0f));
    oscSender.sendMessage(msg);
}

void OscDirector::rampdownClipSpeed(float from, float to, float duration){
    rampdownSpeedAnim.setDuration(duration);
    rampdownSpeedAnim.animateFromTo(from, to);
    LOG << "started video speed rampdown from " << from << " to " << to;
}

void OscDirector::rampdownLayerAlpha(int layer, float from, float to, float duration){
    rampdownAlphaAnim.setDuration(duration);
    rampdownAlphaAnim.animateFromTo(from, to);
    rampdownAlphaLayer = layer;
    LOG << "started video layer rampdown from " << from << " to " << to;
}

void OscDirector::rampdownMasterVolume(float from, float to, float duration){
    rampdownVolumeAnim.setDuration(duration);
    rampdownVolumeAnim.animateFromTo(from, to);
    LOG << "started volume rampdown from " << from << " to " << to;
}





