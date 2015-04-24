#include <iostream>

#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Main.hpp"
#include "libraries/GLUtilities.h"
#include "libraries/LoadObject.h"
#include "libraries/LoadTGA.h"

// These initializations are here because C++ sucks.
// They should be in the header file in my opinion.
const vec3 Main::cameraPosition = vec3(0.0f, 20.0f, 10.0f);
const vec3 Main::cameraTarget = vec3(0.0f, 20.0f, 0.0f);
const vec3 Main::cameraNormal = vec3(0.0f, 1.0f, 0.0f);
const mat4 Main::lookMatrix =
	lookAtv(cameraPosition, cameraTarget, cameraNormal);
const GLfloat Main::viewFrustum[] =
	{0.07f/(right-left), 0.0f, 0.0f, 0.0f,
	 0.0f, 0.07f/(top-bottom), 0.0f, 0.0f,
	 0.0f, 0.0f, 1/(far - near), -near/(far - near),
	 0.0f, 0.0f, 0.0f, 1.0f };


// The following will be reassigned in initResources.
GLuint Main::program     = 0;
Model* Main::bunny       = NULL;
Model* Main::plane       = NULL;
GLuint Main::green       = 0;
GLuint Main::white       = 0;
GLuint Main::red         = 0;
GLuint Main::darkBlue    = 0;
GLuint Main::signTexture = 0;
mat4   Main::transHand   = T(0, 0, 0);
mat4   Main::transSign   = Mult(T(0, 20, 0), Rx(-M_PI_2));
mat4   Main::transPlane  = Mult(Mult(T(0, 20, 1), Rx(-M_PI_2)), S(1.5, 1.5, 1.5));
std::atomic<bool> Main::signShowing = {false};
std::atomic<bool> Main::loadNewSign = {false};
std::atomic<bool> Main::menuOpen = {false};
std::string Main::signTexturePath = "";

// The following will immediately be overwritten by Consumer.
std::atomic<float> Main::handX = {0};
std::atomic<float> Main::handY = {0};
std::atomic<bool>  Main::playing = {false};
std::atomic<bool>  Main::recording = {false};


int main(int argc, char** argv){
	return Main::init(argc, argv);
}

int Main::init(int argc, char** argv){
	int* pargc = &argc;
	glutInit(pargc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	glutCreateWindow("Leap Music!");
	printError("Main::init()");

	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	printError("Main::init() in call to glewInit()");
	if (glew_status != GLEW_OK) {
		std::cout << glewGetErrorString(glew_status) << std::endl;
	}

	dumpInfo();

	if (initResources()) {
		/* We can display it if everything goes OK */
		glutDisplayFunc(Main::onDisplay);
		glutMainLoop();
		std::cout << "Exited main graphics loop." << std::endl;
	}

	// Upon exiting the main loop;
	freeResources();
}


int Main::initResources(void) {
	program = loadShaders((char*)"./shaders/main.vert",
												(char*)"./shaders/main.frag");
	bunny = LoadModelPlus((char*)"./models/bunnyplus.obj");
	plane = LoadModelPlus((char*)"./models/plane2.obj");
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "viewFrustum"),
	                   1, GL_TRUE, viewFrustum);
	glUniformMatrix4fv(glGetUniformLocation(program, "lookMatrix"),
										 1, GL_TRUE, lookMatrix.m);

	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
	LoadTGATextureSimple((char*)"./textures/green.tga", &green);
	LoadTGATextureSimple((char*)"./textures/white.tga", &white);
	LoadTGATextureSimple((char*)"./textures/red.tga", &red);
	LoadTGATextureSimple((char*)"./textures/darkblue.tga", &darkBlue);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glutTimerFunc(16, &Main::onTimer, 0);

	// From GLUtilities.h, to enable keyIsdown().
	initKeymapManager();

	printError("Main::initResources()");
	return 1;
}

void Main::onDisplay(void) {
	GLfloat time = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 500;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	transHand = T(0, handY / 10, 0);
	if (recording)
		glBindTexture(GL_TEXTURE_2D, red);
	else if(playing)
		glBindTexture(GL_TEXTURE_2D, green);
	else if(menuOpen){
		glBindTexture(GL_TEXTURE_2D, darkBlue);
		transHand = T(handX / 10, handY / 10, 0);
	}
	else
		glBindTexture(GL_TEXTURE_2D, white);

	drawObject(transPlane, plane, program);

	if (loadNewSign){
		LoadTGATextureSimple((char*)signTexturePath.c_str(), &signTexture);
		loadNewSign = false;
	}

	if (signShowing) {
		glBindTexture(GL_TEXTURE_2D, signTexture);
		drawObject(transSign, plane, program);
	}

	glutSwapBuffers();
	printError("Main::onDisplay()");
}

void Main::freeResources(void) {
	printError("Main::freeResources()");
}

void Main::drawObject(mat4 transform, Model* model, GLuint p) {
	glUniformMatrix4fv(glGetUniformLocation(p, "transform"),
	                   1, GL_TRUE, transform.m);
	DrawModel(model, p, (char*)"vertPosition",
						(char*)"vertNormal", (char*)"vertTexCoord");
	printError("Main::drawObject()");
}

void Main::onTimer(int value) {
	glutPostRedisplay();
	glutTimerFunc(16, &Main::onTimer, value);
	printError("Main::onTimer()");

}

void Main::showSign(std::string imagePath){
	signTexturePath = imagePath;
	signShowing = true;
	loadNewSign = true;
}

void Main::hideSign(void){
	signShowing = false;
}

void Main::openMenu(void){

	menuOpen = true;
}

void Main::closeMenu(void){
	menuOpen = false;
}
