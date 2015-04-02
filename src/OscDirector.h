//
//  OscDirector.h
//  BaseApp
//
//  Created by Mark van de Korput on 30/03/15.
//
//

#ifndef __BaseApp__OscDirector__
#define __BaseApp__OscDirector__

#include "ofxOsc.h"
#include "ofxAnimatableFloat.h"
#include "ofxHistoryPlot.h"
#include "sattribute.h"

class OscDirector{
public:
    OscDirector(string url="127.0.0.1", int port=9000);
    void setup(string url, int port);
    void setupConfigurables();
    void update(float dt);
    void draw();

public:
    void playAudio();
    void setAudioMasterVolume(float volume=0.0f);
    void playClip();
    void setClipSpeed(float speed);
    void setLayerAlpha(int layer, float alpha);
    void rampdownClipSpeed(float from=1.0f, float to=0.0f, float duration=5.0f);
    void rampdownLayerAlpha(int layer=3, float from=1.0, float to=0.0, float duration=2.0);
    void rampdownMasterVolume(float from=1.0, float to=0.0, float duration=5.0);

protected:
    string oscUrl;
    int oscPort;
    ofxOscSender oscSender;
    string masterAudioVolumeMsg, playAudioMsg, playClipMsg;
    ofxAnimatableFloat rampdownSpeedAnim, rampdownAlphaAnim,rampdownVolumeAnim;
    int rampdownAlphaLayer;
    ofxHistoryPlot *volumePlot;
    Sattribute<float> audioVolume;
};


#endif /* defined(__BaseApp__OscDirector__) */
