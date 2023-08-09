#include "ofMain.h"
#include "ofApp.h"

//========================================================================

int main(int argc,char *argv[]){
	string stx;
    ofSetupOpenGL(1025,650, OF_WINDOW);
	//ofSetupOpenGL(850, 645, OF_WINDOW); // <-------- setup the GL context
     ofApp *app = new ofApp();
     stx=argv[1];
      printf("XXXargs=%s\n",argv[1]);
     app->arg_x=argv[1];

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(app);
}
