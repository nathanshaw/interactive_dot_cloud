#pragma once

#include "ofMain.h"
#include <string.h>
#include "ofxGui.h"

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
    
    ofxPanel gui;
    ofxToggle guiToggle;
    ofxToggle colorToggle;
    ofxToggle lineToggle;
    ofxToggle spectrumToggle;
    
    ofxIntSlider numParticleSlider;
    ofxIntSlider alphaFadeSlider;
    ofxIntSlider redSlider;
    ofxIntSlider greenSlider;
    ofxIntSlider blueSlider;
    ofxIntSlider circleRadSlider;
    
    ofxFloatSlider lineMinSlider;
    ofxFloatSlider lineRangeSlider;
    
    ofxFloatSlider cloudSizeSlider;
    
private:
    ofSoundPlayer soundPlayer;  // Sample Player
		
    const int NUM_BANDS=256;    // Number of bands in our spectrum
    float spectrum[256];        // Smoothed spectrum
    float adjustedSpectrum[256];// Smoothed spectrum
   
    float rad;                  // Cloud radius param
    float vel;                  // Cloud velocity param
    int radBand;                // Band index in spectrum that affects rad value
    int velBand;                // Band index in spectrum that affects vel value
    
    int numPoints;   // Number of points in the cloud
    ofPoint p[1500];             // Cloud's point positions
    float tx[1500], ty[1500];     // Offsets for perlin noise calculation
    
    float time0;                // previous frame time
    
    bool displayGui;            // if the GUI currently being displayed?
    bool displayHelp;           // display keyboard shorcuts
    
    ofColor webColor;
    int red;
    int green;
    int blue;
    int redDir;
    int greenDir;
    int blueDir;
    
    ofVec2f mousePos;
    
    const bool DEBUG = true;
};
