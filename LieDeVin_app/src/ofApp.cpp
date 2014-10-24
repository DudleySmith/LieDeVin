#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    /*
     ofBackground(ofColor::black);
     ofSetBackgroundAuto(true);
     */
    setupGUI();
    
    // Setup OPC
    setupStrips();
    
    // Syphon Setup
    // Using Syphon app Simple Server, found at http://syphon.v002.info/
    m_oSyphonDir.setup();
    //register for our directory's callbacks
    ofAddListener(m_oSyphonDir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    ofAddListener(m_oSyphonDir.events.serverRetired, this, &ofApp::serverRetired);
    
    m_oSyphonDir.getServerList();
    m_oSyphonClient.setup();
    setupSyphon();
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
}

//--------------------------------------------------------------
void ofApp::setupGUI(){
    
    m_gui.setup("settings");
    
    m_gWindow.setName("Window");
    m_gWindow.add(m_pWinHeight.set("Height", 0, 1024, 0));
    m_gWindow.add(m_pWinWidth.set("Width", 0, 576, 0));
    m_gui.add(m_gWindow);
    
    m_gOPC.setName("OPC");
    m_gOPC.add(m_pOPCAddress.set("address", "127.0.0.1"));
    m_gOPC.add(m_pOPCPort.set("port", 7890));
    m_gOPC.add(m_pDrawGrabRegion.set("DrawGrab", false));
    m_gOPC.add(m_pDrawStrips.set("DrawStrips", true));
    m_gui.add(m_gOPC);
    
    m_gSyphon.setName("Syphon");
    m_gSyphon.add(m_iDirIdx.set("index", 0));
    m_gSyphon.add(m_sServerName.set("server", "defServer"));
    m_gSyphon.add(m_sAppName.set("app", "defApp"));
    m_gSyphon.add(m_pDrawSyphon.set("draw", true));
    m_gui.add(m_gSyphon);

    m_gBackground.setName("Background");
    m_gBackground.add(m_pDrawBackgroundGradient.set("draw", false));
    m_gui.add(m_gBackground);
    
    m_gui.loadFromFile("settings.xml");
    
}

//--------------------------------------------------------------
void ofApp::setupStrips(){
    // Connection
    m_oOpcClient.setup(m_pOPCAddress, m_pOPCPort);
    
    m_oStrips.clear();
    
    for (int i = 0; i <= 7; i++)
    {
        NeoPixelStripByVector strip;
        
        ofPoint st = ofPoint(((float)i/8) * ofGetWidth(), 0);
        ofPoint nd = ofPoint(((float)i/8) * ofGetWidth(), ofGetHeight());
        
        strip.setupLedStripByVector(st, nd, 30);
        m_oStrips.push_back(strip);
    }
    
}

//--------------------------------------------------------------
void ofApp::setupSyphon(){
    
    if (m_iDirIdx>(m_oSyphonDir.size()-1)) {
        // Dont'go through limits of syphon ervers available
        m_iDirIdx=0;
    }
    if (m_iDirIdx<0) {
        // Dont'go to weird values
        m_iDirIdx=(m_oSyphonDir.size()-1);
    }
        
    if(m_oSyphonDir.size() > 0){
        m_oSyphonClient.set(m_oSyphonDir.getDescription(m_iDirIdx));
    }
    
    string serverName = m_oSyphonClient.getServerName();
    string appName = m_oSyphonClient.getApplicationName();
    
    if(serverName == ""){
        serverName = "null";
    }
    if(appName == ""){
        appName = "null";
    }
    
    m_sServerName = serverName;
    m_sAppName = appName;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(m_oSyphonClient.isSetup()){
        // Set Window as Syphon
        if(m_pWinWidth!=m_oSyphonClient.getWidth() && m_pWinHeight!=m_oSyphonClient.getHeight()){
            // --
            m_pWinWidth = m_oSyphonClient.getWidth();
            m_pWinHeight = m_oSyphonClient.getHeight();
        }
        // Set Window as Syphon
        if(m_pWinWidth>0 && m_pWinHeight>0){
            ofSetWindowShape(m_pWinWidth, m_pWinHeight);
        }
    }
    
    //m_oSyphonClient.bind();
    
    //updateStrips();
    
}

//--------------------------------------------------------------
void ofApp::updateStrips(){
    
    if (!m_oStrips.empty())
    {
        for (int i = 0; i <= 7; i++)
        {
            m_oStrips[i].grabImageData();
            m_oStrips[i].update();
        }
    }
    
    // If the client is not connected do not try and send information
    if (!m_oOpcClient.isConnected())
    {
        // Will continue to try and reconnect to the Pixel Server
    }
    else
    {
        // Write out the first set of data
        if (!m_oStrips.empty())
        {
            for (int i = 0; i <= 7; i++)
            {
                //Offset to Channel One
                m_oOpcClient.writeChannel(i+1, m_oStrips[i].colorData());
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(ofColor::black);
    
    if(m_pDrawBackgroundGradient==true){
        ofBackgroundGradient(ofColor::black, ofColor::white);
    }
    
    if(m_pDrawSyphon==true){
        // Syphon Stuff
        if(m_oSyphonClient.isSetup() && m_oSyphonDir.size()>0){
            m_oSyphonClient.draw(0, 0, ofGetWidth(), ofGetHeight());
        }else{
            ofDrawBitmapString("Syphon OUT", ofPoint(0.5*ofGetWidth(), 0.5*ofGetHeight()));
        }
    }
    
    updateStrips();
    
    drawOPC();
    drawGUI();
    
}

//--------------------------------------------------------------
void ofApp::drawOPC(){
    // Visual Representation of the Grab Area
    if (m_pDrawGrabRegion)
    {
        for (int i = 0; i <= 7; i++)
        {
            m_oStrips[i].drawGrabRegion();
        }
    }
    
    if (m_pDrawStrips)
    {
        for (int i = 0; i <= 7; i++)
        {
            m_oStrips[i].drawPreview(ofPoint(10,10), 0.25);
        }
    }
}

//--------------------------------------------------------------
void ofApp::drawGUI(){
    
    // Draw gui
    m_gui.setPosition(ofGetWidth() - 210, 50);
    m_gui.draw();
    
    // Indications
    ofPushStyle();
    if(m_oOpcClient.isConnected()){
        ofSetColor(ofColor::green);
        ofDrawBitmapString("OPC connected", ofGetWidth() - 200, 20);
    }else{
        ofSetColor(ofColor::red);
        ofDrawBitmapString("OPC not connected", ofGetWidth() - 200, 20);
    }
    ofRect(ofGetWidth() - 210, 10, 10, 10);
    ofPopStyle();
    
}

void ofApp::exit(){
    // Close Connection
    m_oOpcClient.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case OF_KEY_RIGHT:
            m_iDirIdx++;
            setupSyphon();
            break;
        case OF_KEY_LEFT:
            m_iDirIdx--;
            setupSyphon();
            break;
        default:
            break;
    }
    
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
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    setupStrips();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//-------------------------------------------------------------
void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    m_iDirIdx = 0;
}
//-------------------------------------------------------------
void ofApp::serverUpdated(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Updated")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    m_iDirIdx = 0;
}
//-------------------------------------------------------------
void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    m_iDirIdx = 0;
}

