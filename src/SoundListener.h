//
//  SoundListener.h
//  BaseApp
//
//  Created by Mark van de Korput on 30/03/15.
//
//

#ifndef __BaseApp__SoundListener__
#define __BaseApp__SoundListener__

#include "ofxFft.h"
#include "ofxHistoryPlot.h"
#include "sattribute.h"
#include "ofxAnimatableFloat.h"

class SoundListener{
public:
    SoundListener();
    ~SoundListener();
    void setup();
    void update(float dt);
    void draw();

public: // operations
    void audioIn( float * input, int bufferSize, int nChannels );

public: // events
    ofEvent<float> amplitudeTriggerEvent;

protected: // attributes
    enum InputMode{ MODE_MIC, MODE_NOISE, MODE_SINE };
    
    int nBufferSize;
    ofxFft* fft;
    float *audioInput, *fftOutput;
    InputMode mode;
    ofxHistoryPlot *amplitudePlot;
    ofxAnimatableFloat ampTriggerDelayAnim;

public:
    Sattribute<float> maxAmp;
    Sattribute<float> topAmp;
};

#endif /* defined(__BaseApp__SoundListener__) */
