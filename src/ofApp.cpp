#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    // Setup our sample player
    soundPlayer.loadSound("NightTrain.mp3");
    soundPlayer.setLoop(true);
    soundPlayer.setSpeed(1.0);
    soundPlayer.play();
    
    // Init spectrum to 0
    for (int i=0; i<NUM_BANDS; i++) {
        spectrum[i] = 0.0;
    }
    
    for (int j=0; j<1500; j++) {
        tx[j] = ofRandom(0, 1000);
        ty[j] = ofRandom(0, 1000);
    }
    
    // setup some initial values
    rad = 2000;
    vel = 0.1;
    radBand = 2;
    velBand = 125;
    ofSetBackgroundAuto(false);
    ofEnableAlphaBlending();
    numPoints = 1000;
    redDir = -1;
    greenDir= -1;
    blueDir = -1;
    displayHelp = false;
    
    // gui setup
    gui.setup();
    gui.setBackgroundColor(ofColor(0,0,0,50));
    gui.setPosition(30,10);
    
    gui.add(guiToggle.setup("Display GUI", false));
    gui.add(colorToggle.setup("Line Color", true));
    gui.add(lineToggle.setup("Line Draw", true));
    gui.add(spectrumToggle.setup("Show Spectrum", false));
    gui.add(numParticleSlider.setup("Particle Number", numPoints, 10, MAX_PARTICLES));
    gui.add(alphaFadeSlider.setup("Alpha Fade", 10, 10, 150));
    gui.add(redSlider.setup("Red", 200, 75, 255));
    gui.add(greenSlider.setup("Green", 200, 75, 255));
    gui.add(blueSlider.setup("Blue", 200, 75, 255));
    gui.add(colorDeltaSlider.setup("Color Delta", 1, 0.5, 3));
    gui.add(dotLinesSlider.setup("Lines per dot", 1, 1, 10));
    gui.add(circleRadSlider.setup("Circle Rad", 0, 0, 8));
    gui.add(lineMinSlider.setup("Line Min", 0, 0, 80));
    gui.add(lineRangeSlider.setup("Line Range", 7, 1, 25));
    gui.add(cloudSizeSlider.setup("Cloud Size", 1.5, 1.0, 6.0));
    
    webColor = ofColor(redSlider, greenSlider, blueSlider, 50);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Update and get the spectrum array
    ofSoundUpdate();
    float* val = ofSoundGetSpectrum(NUM_BANDS);
    
    // Update smooth spectrum buffer
    for (int i=0; i<NUM_BANDS; i++) {
        spectrum[i] *= .96;
        spectrum[i] = max(spectrum[i], val[i]);
    }
    
    // Get the updated bands from the smoothed spectrum buffer
    // and map them to a usable range (given the music playing)
    rad = ofMap(spectrum[radBand], 0.5, 4.0, 50.0*cloudSizeSlider, 250.0*cloudSizeSlider);
    vel = ofMap(spectrum[velBand], 0.0, 0.1, 0.1, 1.5);
    
    // Get difference between current time and last frames time
    // and clamp (restrict) it's range
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp(dt, 0.0, 0.1);
    time0 = time;
    
    numPoints = numParticleSlider;
    // This loop will update the position of our points
    for (int i=0; i<numPoints; i++) {
        // First we update our 2d-perlin noise positions
        // using vel*dt (we're scaling vel by dt)
        tx[i] += (vel*dt);
        ty[i] += (vel*dt);
        
        // Then we pass that into ofSignedNoise (2d perlin noise)
        // and scale by rad to update the current points position
        p[i].x = ofSignedNoise(tx[i]) * rad;
        p[i].y = ofSignedNoise(ty[i]) * rad;
    }
    
    webColor = ofColor(redSlider, greenSlider, blueSlider, 50);
    
    if (colorToggle){
        for (int i=0; i<NUM_BANDS; i++) {
            if (i >= 0 && i < 4){
                spectrum[i] *= 0.15;
            }
            adjustedSpectrum[i] = spectrum[i] * (4*i+10) * 10;
            if(adjustedSpectrum[i] > 200.0){
                std::cout << i << " : " << adjustedSpectrum[i] << std::endl;
                if (i < NUM_BANDS*0.1){
                    if (redSlider > 250){
                        redDir = -1;
                        redSlider = 250;
                    }
                    else if(redSlider < 65){
                        redDir = 1;
                        redSlider = 65;
                    }
                    redSlider = redSlider + redDir*colorDeltaSlider;
                }
                else if (i < NUM_BANDS*0.4){
                    if (greenSlider > 250){
                        greenDir = -1;
                        greenSlider = 250;
                    }
                    else if(greenSlider < 65){
                        greenDir = 1;
                        greenSlider = 65;
                    }
                    greenSlider = greenSlider + greenDir*colorDeltaSlider;
                }
                else{
                    if (blueSlider > 250){
                        blueDir = -1;
                        blueSlider = 250;
                    }
                    else if (blueSlider < 65){
                        blueDir = 1;
                        blueSlider = 65;
                    }
                    blueSlider = blueSlider + blueDir*colorDeltaSlider;
                }
                
                webColor.set(ofColor(redSlider, greenSlider, blueSlider, 50));
            }
        }
    }
    else{
        webColor = ofColor(255,255,255);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushMatrix();
    
    if (guiToggle){
        gui.draw();
    }
    
    ofSetColor(0, 0, 0, alphaFadeSlider);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);
    
    ofSetColor(webColor);
    for (int i=0; i<numPoints; i++) {
        // ofSetColor(ofColor(webColor[0]+ofRandom(-3,3), webColor[1]+ofRandom(-3,3), webColor[2]+ofRandom(-3,3)));
        ofCircle(p[i], circleRadSlider);
    }
    
    ofSetColor(webColor);
    if (lineToggle){
        for (int i=0; i<numPoints; i++) {
            int foundLines = 0;
            for (int j=0; j<numPoints; j++) {
                if (j !=i && foundLines < dotLinesSlider) {
                    if (ofDist(p[i].x, p[i].y, p[j].x, p[j].y) <= lineMinSlider + lineRangeSlider) {
                        if (ofDist(p[i].x, p[i].y, p[j].x, p[j].y) >= lineMinSlider) {
                            ofLine(p[i], p[j]);
                            foundLines += 1;
                        }
                    }
                }
            }
        }
    }
    
    ofPopMatrix();
    
    if (spectrumToggle){
        for (int i=0; i<NUM_BANDS; i++){
            adjustedSpectrum[i] = spectrum[i] * (2*i+1) * 4;
            if (adjustedSpectrum[i] > 70){
                ofSetColor(255, 20, 20);
                ofRect(ofGetWidth()/NUM_BANDS * i + 10, ofGetHeight()-10, 3, -spectrum[i] * (2*i+1) * 4);
            }
            else {
                ofSetColor(75, 75, 75);
                ofRect(ofGetWidth()/NUM_BANDS * i + 10, ofGetHeight()-10, 3, -spectrum[i] * (2*i+1) * 4);
            }
        }
    }
    if (displayHelp){
        ofSetColor(50,50,255);
        ofDrawBitmapString("<space> to display GUI", ofGetWidth()-310, 30);
        ofDrawBitmapString("<d> to increase dot size", ofGetWidth()-310, 70);
        ofDrawBitmapString("<s> to show spectro-bars", ofGetWidth()-310, 90);
        ofDrawBitmapString("<l> to toggle the lines", ofGetWidth()-310, 110);
        ofDrawBitmapString("<f> to increase line numbers", ofGetWidth()-310, 130);
        ofDrawBitmapString("<c> to toggle colors", ofGetWidth()-310, 150);
        ofDrawBitmapString("<a> to increase alpha (motion blur)", ofGetWidth()-310, 170);
        ofDrawBitmapString("<-> and <+> to adjust cloud size", ofGetWidth()-310, 200);
        ofDrawBitmapString("<[> and <]> to adjust min line length", ofGetWidth()-310, 220);
        ofDrawBitmapString("<;> and <'> to adjust line range", ofGetWidth()-310, 240);
        ofDrawBitmapString("<.> and </> to adjust the number of particles", ofGetWidth()-310, 260);
        
        ofDrawBitmapString("<left mouse> to mix up line rules", ofGetWidth()-310, 290);
        ofDrawBitmapString("<right mouse> to change particle num", ofGetWidth()-310, 310);
        
        ofDrawBitmapString("<h> to hide this help text", ofGetWidth()-310, 340);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 32){
        guiToggle = !guiToggle;
    }
    else if (key == 67 || key == 99){
        colorToggle = !colorToggle;
    }
    else if (key == 68 || key == 100){
        circleRadSlider = circleRadSlider + 1;
        if (circleRadSlider > 8){
            circleRadSlider = 0;
            if (!lineToggle){
                lineToggle = !lineToggle;
            }
        }
    }
    else if (key == 83 || key == 115){
        spectrumToggle = ! spectrumToggle;
    }
    else if (key == 72 || key == 104){
        displayHelp = !displayHelp;
    }
    else if (key == 76 || key == 108){
        lineToggle = !lineToggle;
        if (!lineToggle && circleRadSlider == 0){
            circleRadSlider = 1;
        }
    }
    else if (key == 65 || key == 97){
        alphaFadeSlider = alphaFadeSlider + (int)alphaFadeSlider/10 + 1;
        if (alphaFadeSlider > 50){
            alphaFadeSlider = 10;
        }
    }
    else if (key == 43 || key == 61){
        // + pressed
        if (cloudSizeSlider < 5.75){
            cloudSizeSlider = cloudSizeSlider + 0.25;
        }
    }
    else if (key == 45 || key == 95){
        // - pressed
        if (cloudSizeSlider > 0.25){
            cloudSizeSlider = cloudSizeSlider - 0.25;
        }
    }
    else if (key == 91 || key == 123){
        // [ pressed
        if (lineMinSlider >= 1){
            lineMinSlider = lineMinSlider - (int)lineMinSlider/5 - 1;
        }
    }
    else if (key == 93 || key == 125){
        // ] pressed
        if (lineMinSlider < 150){
            lineMinSlider = lineMinSlider + (int)lineMinSlider/5 + 1;
        }
    }
    else if (key == 58 || key == 59){
        // ; pressed
        if (lineRangeSlider >= 1){
            lineRangeSlider = lineRangeSlider - 1 - (int)lineRangeSlider/10;
        }
        else if (lineRangeSlider > 0.075){
            lineRangeSlider - 0.075;
        }
    }
    else if (key == 39 || key == 34){
        // ' pressed
        if (lineRangeSlider < 30){
            lineRangeSlider = lineRangeSlider + 1 + (int)lineRangeSlider/10;
        }
    }
    else if (key == 46 || key == 62){
        // ; pressed
        if (numParticleSlider > 1){
            numParticleSlider = numParticleSlider - 1 - (int)numParticleSlider/15;
        }
    }
    else if (key == 47 || key == 63){
        // ; pressed
        if (numParticleSlider < MAX_PARTICLES){
            numParticleSlider = numParticleSlider + 1 + (int)numParticleSlider/15;
        }
    }
    else if (key == 70 || key == 102){
        // l pressd
        if (dotLinesSlider < 10){
            dotLinesSlider = dotLinesSlider + 1;
        }
        else{
            dotLinesSlider = 1;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    ofSetColor(255, 255, 255);
    mousePos = ofVec2f(x, y);
    ofDrawCone(mousePos.x, mousePos.y, 5, 5);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == 0){
        lineMinSlider = ofRandom(0, 50);
        lineRangeSlider = ofRandom(0, 12);
        ofSetColor(255,0,0);
        ofDrawBitmapString("Min and Max Dist Changed", 50, ofGetHeight()-50);
        if (DEBUG){
            std::cout << "Min Dist : " << lineMinSlider << "  Max Dist : " << lineMinSlider + lineRangeSlider << std::endl;
        }
    }
    else{
        numPoints = ofRandom(20, MAX_PARTICLES);
        ofSetColor(255,0,0);
        ofDrawBitmapString("Num Points Now : ", 50, ofGetHeight()-50);
        numParticleSlider = numPoints;
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
