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
    
    rad = 2000;
    vel = 0.1;
    
    // Set the bands that we want to respond to from our FFT
    radBand = 2;
    velBand = 125;

    ofSetBackgroundAuto(false);
    ofEnableAlphaBlending();
    
    // How many of the points will be use?
    numPoints = 250;
    
    // is the gui currently being displayed?
    displayGui = false;
    
    gui.setBackgroundColor(ofColor(0,0,0,50));
    gui.setup();
    gui.setPosition(30,10);
    
    gui.add(guiToggle.setup("Display GUI", displayGui));
    gui.add(colorToggle.setup("Line Color", true));
    gui.add(lineToggle.setup("Line Draw", true));
    gui.add(spectrumToggle.setup("Show Spectrum", false));
    
    gui.add(numParticleSlider.setup("Particle Number", numPoints, 10, 500));
    
    gui.add(alphaFadeSlider.setup("Alpha Fade", 10, 10, 150));
    gui.add(redSlider.setup("Red", 255, 75, 255));
    gui.add(greenSlider.setup("Green", 255, 75, 255));
    gui.add(blueSlider.setup("Blue", 255, 75, 255));
    
    gui.add(circleRadSlider.setup("Circle Rad", 0, 0, 8));
    
    gui.add(lineMinSlider.setup("Line Min", 0, 0, 80));
    gui.add(lineRangeSlider.setup("Line Range", 7, 1, 25));
    
    gui.add(cloudSizeSlider.setup("Cloud Size", 1.5, 1.0, 6.0));
    
    webColor = ofColor(redSlider, greenSlider, blueSlider, 50);
    
    red = redSlider;
    green = greenSlider;
    blue = blueSlider;
    redDir = -1;
    greenDir = -1;
    blueDir = -1;
    
    displayHelp = true;
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
    
    if (spectrumToggle){
        for (int i=0; i<NUM_BANDS; i++){
            adjustedSpectrum[i] = spectrum[i] * (2*i+1) * 5;
            if (adjustedSpectrum[i] > 70){
                ofSetColor(255, 20, 20);
                ofRect(ofGetWidth()/NUM_BANDS * i + 10, ofGetHeight()-10, 3, -spectrum[i] * (2*i+1) * 5);
            }
            else {
                ofSetColor(75, 75, 75);
                ofRect(ofGetWidth()/NUM_BANDS * i + 10, ofGetHeight()-10, 3, -spectrum[i] * (2*i+1) * 5);
            }
        }
    }
    if (colorToggle){
        for (int i=0; i<NUM_BANDS; i++) {
            bool redLock, greenLock, blueLock = true;
            adjustedSpectrum[i] = spectrum[i] * (2*i+10) * 3;
            if(adjustedSpectrum[i] > 70.0){
                if (i < NUM_BANDS*0.3 && redLock){
                    std::cout << i << " : " << adjustedSpectrum[i] << std::endl;
                    if (red > 235 || red < 70){
                        redDir *= -1;
                        red += 10*redDir;
                    }
                    red += ofRandom(1, 7)*redDir;
                    redSlider = red;
                    redLock = false;
                }
                else if (i < NUM_BANDS*0.6 && greenLock){
                    if (green > 235 || green < 70){
                        greenDir *= -1;
                        green += 10 * greenDir;
                    }
                    green += ofRandom(1, 7)*greenDir;
                    greenSlider = green;
                    greenLock = false;
                }
                else if (blueLock){
                    if (blue > 235 || blue < 70){
                        blueDir *= -1;
                        blue += 10 * blueDir;
                    }
                    blue += ofRandom(1, 7)*blueDir;
                    blueSlider = blue;
                    blueLock = false;
                }
                
                webColor.set(ofColor(red, green, blue, 50));
                if (DEBUG){
                    std::cout << redDir << " : " << greenDir << " : " << blueDir << std::endl;
                    std::cout << webColor << std::endl;
                    std::cout << "- - - - - - - - - - - -" << std::endl;
                }
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
    
    if (displayGui){
        gui.draw();
    }
    
    ofSetColor(0, 0, 0, alphaFadeSlider);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);
    
    ofSetColor(webColor);
    for (int i=0; i<numPoints; i++) {
        ofCircle(p[i], circleRadSlider);
    }
    
    if (lineToggle){
        for (int i=0; i<numPoints; i++) {
            for (int j=0; j<numPoints; j++) {
                if (j !=i) {
                    if (ofDist(p[i].x, p[i].y, p[j].x, p[j].y) <= lineMinSlider + lineRangeSlider) {
                        if (ofDist(p[i].x, p[i].y, p[j].x, p[j].y) >= lineMinSlider) {
                            ofLine(p[i], p[j]);
                        }
                    }
                }
            }
        }
    }
    
    ofPopMatrix();
    
    if (displayHelp){
        ofSetColor(50,50,255);
        ofDrawBitmapString("<space> to display GUI", ofGetWidth()-290, 30);
        ofDrawBitmapString("<d> to increase dot size", ofGetWidth()-290, 70);
        ofDrawBitmapString("<s> to show spectro-bars", ofGetWidth()-290, 90);
        ofDrawBitmapString("<l> to turn off the lines", ofGetWidth()-290, 110);
        ofDrawBitmapString("<c> to turn on colors", ofGetWidth()-290, 130);
        ofDrawBitmapString("<a> to increate alpha", ofGetWidth()-290, 150);
        ofDrawBitmapString("<left mouse> to mix up line rules", ofGetWidth()-290, 170);
        ofDrawBitmapString("<right mouse> to change particle num", ofGetWidth()-290, 190);
        ofDrawBitmapString("<-> to decrease cloud size", ofGetWidth()-290, 210);
        ofDrawBitmapString("<+> to increate cloud size", ofGetWidth()-290, 230);
        
        ofDrawBitmapString("<h> to hide this help text", ofGetWidth()-290, 270);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 32){
        if (DEBUG){
            std::cout << "space pressed" << std::endl;
        }
        displayGui = !displayGui;
        guiToggle = displayGui;
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
        if (cloudSizeSlider < 6){
            cloudSizeSlider = cloudSizeSlider + 1;
        }
    }
    else if (key == 45 || key == 95){
        // - pressed
        if (cloudSizeSlider > 0){
            cloudSizeSlider = cloudSizeSlider - 1;
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
        lineRangeSlider = ofRandom(1, 12);
        ofSetColor(255,0,0);
        ofDrawBitmapString("Min and Max Dist Changed", 50, ofGetHeight()-50);
        if (DEBUG){
            std::cout << "Min Dist : " << lineMinSlider << "  Max Dist : " << lineMinSlider + lineRangeSlider << std::endl;
        }
    }
    else{
        numPoints = ofRandom(20, 1000);
        if (DEBUG){
            std::cout << "NumPoints : " << numPoints << std::endl;
        }
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
