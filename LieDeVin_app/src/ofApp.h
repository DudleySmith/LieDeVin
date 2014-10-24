#pragma once

#include "ofMain.h"

#include "ofxOPC.h"
#include "ofxGui.h"
#include "ofxSyphon.h"

class ofApp : public ofBaseApp{

	public:
    void setup();
    void setupSyphon();
    void setupGUI();
    void setupStrips();
    
    void update();
    void updateStrips();

    void draw();
    void drawGUI();
    void drawOPC();
    
    void exit();
    
    void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
    // OPC ---------------------------------
    ofxOPC m_oOpcClient;
    vector<NeoPixelStripByVector> m_oStrips;
    
    // Gui ---------------------------------
	ofParameterGroup m_gWindow;
    ofParameter<int> m_pWinWidth;
	ofParameter<int> m_pWinHeight;
    
	ofParameterGroup m_gOPC;
	ofParameter<string> m_pOPCAddress;
	ofParameter<int> m_pOPCPort;
	ofParameter<bool> m_pDrawGrabRegion;
	ofParameter<bool> m_pDrawStrips;
    
	ofParameterGroup m_gSyphon;
	ofParameter<int> m_iDirIdx;
	ofParameter<string> m_sServerName;
	ofParameter<string> m_sAppName;
	ofParameter<bool> m_pDrawSyphon;
    
	ofParameterGroup m_gBackground;
    ofParameter<bool> m_pDrawBackgroundGradient;
    
	ofxPanel m_gui;
    
    // Syphon ------------------------------
	ofxSyphonClient m_oSyphonClient;
	ofxSyphonServerDirectory m_oSyphonDir;
    
    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverUpdated(ofxSyphonServerDirectoryEventArgs &args);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    

};
