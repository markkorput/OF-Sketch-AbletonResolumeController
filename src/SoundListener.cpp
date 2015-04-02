//
//  SoundListener.cpp
//  BaseApp
//
//  Created by Mark van de Korput on 30/03/15.
//
//

#include "SoundListener.h"
#include "ofxSuperLog.h"
#include "ofxRemoteUIVars.h"

SoundListener::SoundListener(){
    // setup(); // <-- this is causing problems
    nBufferSize = 256;
    mode = MODE_MIC;
    maxAmp.set(1.5);
}

SoundListener::~SoundListener(){
    delete fftOutput;
    delete fft;
}

void SoundListener::setup(){
    ofSoundStreamSetup(0 /* outputs */,2 /* inputs */, ofGetAppPtr(), 44100, nBufferSize, 2);
    fft = ofxFft::create(nBufferSize, OF_FFT_WINDOW_BARTLETT);
    audioInput = new float[fft->getBinSize()];
    fftOutput = new float[fft->getBinSize()];
    amplitudePlot = new ofxHistoryPlot( NULL, "amplitude", 400, false); //NULL cos we don't want it to auto-update. confirmed by "true"
    amplitudePlot->setAutoRangeShrinksBack(true);
}

void SoundListener::update(float dt){
    ampTriggerDelayAnim.update(dt);
}

void SoundListener::draw(){
    amplitudePlot->draw(0,0);
    
    if(topAmp.get() > RUI_VAR(float, "trigger-amplitude")){
        ofSetColor(255, 0, 0);
    } else {
        ofSetColor(0, 255, 0);
    }
    ofRect(0, amplitudePlot->getHeight(), amplitudePlot->getWidth(), 10);

}

void SoundListener::audioIn( float * input, int bufferSize, int nChannels ){
    // LOG << "audio IN: " << *input;
    if (mode == MODE_MIC) {
        // store input in audioInput buffer
        memcpy(audioInput, input, sizeof(float) * bufferSize);
    } else if (mode == MODE_NOISE) {
        for (int i = 0; i < bufferSize; i++)
            audioInput[i] = ofRandom(-1, 1);
    } else if (mode == MODE_SINE) {
        for (int i = 0; i < bufferSize; i++)
            audioInput[i] = sinf(PI * i * 200 / 800);
    }

    fft->setSignal(audioInput);
    memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * fft->getBinSize());
    
    float avg = 0.0;
    for(int i=0; i<fft->getBinSize(); i++){
//        avg += fftOutput[i];
        if(fftOutput[i] > avg)
            avg = fftOutput[i];
    }

    amplitudePlot->update(avg);
    if(avg > maxAmp.get())
        maxAmp.set(avg);
    
    if(avg < topAmp.get() && topAmp.get() > RUI_VAR(float, "trigger-amplitude")){
        if(!ampTriggerDelayAnim.isAnimating()){
            ampTriggerDelayAnim.setDuration(RUI_VAR(float, "trigger-delay"));
            ampTriggerDelayAnim.animateFromTo(0.0, 1.0);

            float value = ofMap(avg, RUI_VAR(float, "trigger-amplitude"), maxAmp.get(), 0.0, 1.0);
            ofNotifyEvent(amplitudeTriggerEvent, value, this);
        }
    }

    topAmp.set(avg);
    
    // LOG << "FFT - avg: " << avg / fft->getBinSize() << ", ttl: " << fft->getBinSize();
}