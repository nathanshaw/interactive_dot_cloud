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
    ofxIntSlider dotLinesSlider;
    
    ofxFloatSlider colorDeltaSlider;
    ofxFloatSlider lineMinSlider;
    ofxFloatSlider lineRangeSlider;
    
    ofxFloatSlider cloudSizeSlider;
    
private:
    
    ofSoundPlayer soundPlayer;  // Sample Player
		
    // constants
    const bool DEBUG = false;
    const int MAX_PARTICLES = 2000;
    const int NUM_BANDS=256;    // Number of bands in our spectrum
    
    // want to be FFT
    float spectrum[256];        // Smoothed spectrum
    float adjustedSpectrum[256];// Smoothed spectrum
    float rad;                  // Cloud radius param
    float vel;                  // Cloud velocity param
    int radBand;                // Band index in spectrum that affects rad value
    int velBand;                // Band index in spectrum that affects vel value
    
    // point cloud stuff
    int numPoints;   // Number of points in the cloud
    ofPoint p[2000];             // Cloud's point positions
    float tx[2000], ty[2000];     // Offsets for perlin noise calculation
    
    float time0;                // previous frame time
    
    // color stuff
    ofColor webColor;
    int redDir, greenDir, blueDir;
    
    // the rest
    ofVec2f mousePos;
    bool displayHelp;
    
};
