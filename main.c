#include <math.h>
#include <GL/gl.h>

#include "./libraries/GLUtilities.h"
#include "./libraries/LoadObject.h"
#include "./libraries/LoadTGA.h"
#include "./libraries/VectorUtils3.h"

#include "main.h"
#include "instancing.h"

#define near 1.0
#define far 300.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

// vertex array object
unsigned int vertexArrayObjID;
GLuint program;
GLuint skyboxProgram;
GLuint instancingProgram;

Model *walls;
Model *roof;
Model *blade;
Model *balcony;
Model *ground;
Model *bunny;
Model *octagon;
Model *teapot;
Model *skybox;
mat4 transWalls;
mat4 transRoof;
mat4 transBlade;
mat4 transBalcony;
mat4 transGround;
mat4 transBunny;
mat4 transTeapot;
mat4 transCubes;

GLuint concrete;
GLuint grass;
GLuint skyTexture;
GLuint maskrosTexture;

mat4 lookMatrix;
vec3 cameraPos;
vec3 cameraTarget;
vec3 cameraNormal;
vec3 cameraDirection;
vec3 bladePos;


void init(void)
{
  bladePos = (vec3){0, 9, 4.5};
  cameraPos = (vec3){1.5f, 1.6f, 2.0f};
  cameraTarget = (vec3){-10.0f, -10.0f, -10.0f};
  cameraNormal = (vec3){0.0f, 1.0f, 0.0f};
  lookMatrix = lookAtv(cameraPos, cameraTarget, cameraNormal);

	dumpInfo();

	walls = LoadModelPlus("./models/windmill/windmill-walls.obj");
	roof = LoadModelPlus("./models/windmill/windmill-roof.obj");
	blade = LoadModelPlus("./models/windmill/blade.obj");
	balcony = LoadModelPlus("./models/windmill/windmill-balcony.obj");
	ground = LoadModelPlus("./models/ground.obj");
	bunny = LoadModelPlus("./models/bunnyplus.obj");
	octagon = LoadModelPlus("./models/octagon.obj");
	teapot = LoadModelPlus("./models/teapot.obj");
	skybox = LoadModelPlus("./models/skybox.obj");
	transWalls = T(0, 0, 0);
	transRoof = Mult(transWalls, T(0, 0, 0));
	transBalcony = Mult(transWalls, Ry(M_PI / 2));
	transGround = Mult(T(-5.3,1.8,-1.5), Rz(M_PI/2));
	transBunny = T(-2.2, -2.3, 10.2);
	transTeapot = T(34.4, 6.4, -30.4);
	transCubes = T(-2.2, -2.3, 10.2);

	// Load textures
	LoadTGATextureSimple("./textures/white.tga", &concrete);
	LoadTGATextureSimple("./textures/green.tga", &grass);
	LoadTGATextureSimple("./textures/SkyBox512.tga", &skyTexture);
	LoadTGATextureSimple("./textures/darkblue.tga", &maskrosTexture);

	// GL inits
	printError("GL inits");
	glClearColor(1.0,0.0,0.0,0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	// Load and compile shader
	skyboxProgram = loadShaders("./shaders/skybox.vert", "./shaders/skybox.frag");
	program = loadShaders("./shaders/world.vert", "./shaders/world.frag");
	instancingProgram = loadShaders("./shaders/instancing.vert", "./shaders/instancing.frag");

	GLfloat projectionMatrix[] = {2.0f*near/(right-left), 0.0f,
                                (right+left)/(right-left), 0.0f,
                                0.0f, 2.0f*near/(top-bottom),
                                (top+bottom)/(top-bottom), 0.0f,
                                0.0f, 0.0f, -(far + near)/(far - near),
                                -2*far*near/(far - near),
                                0.0f, 0.0f, -1.0f, 0.0f };

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, maskrosTexture);
	glUniform1i(glGetUniformLocation(program, "maskrosen"), 1);
	printError("init(): Multitexturing");
	glActiveTexture(GL_TEXTURE0);

  Point3D lightSourcesColorsArr[] = { {1.0f, 0.0f, 0.0f},   // Red light
                                      {0.0f, 1.0f, 0.0f},   // Green light
                                      {0.0f, 0.0f, 1.0f},   // Blue light
                                      {1.0f, 1.0f, 1.0f} }; // White light
	printError("init(): After lightSourcescolorsarr");
	GLfloat specularExponent[] = {10.0, 20.0, 60.0, 5.0};
	GLint isDirectional[] = {0,0,1,1};
	Point3D lightSourcesDirectionsPositions[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional
																								{0.0f, 5.0f, 10.0f}, // Green light, positional
																								{-1.0f, 0.0f, 0.0f}, // Blue light along X
																								{0.0f, 0.0f, -1.0f} }; // White light along Z
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"),
							 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"),
							 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"),
							 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"),
							 4, isDirectional);
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0);

	glUseProgram(skyboxProgram);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(skyboxProgram, "texUnit"), 0);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

	glUseProgram(instancingProgram);
	glUniformMatrix4fv(glGetUniformLocation(instancingProgram, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
	setupInstancedVertexAttributes(instancingProgram, bunny);
	printError("init(): End");
}

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
	printError("OnTimer()");
}

void display(void)
{
	printError("pre display");
	cameraPos = moveOnKeyInputRelativeCamera(cameraPos);
	cameraTarget = moveOnKeyInputRelativeCamera(cameraTarget);
	lookMatrix = lookAtv(cameraPos, cameraTarget, cameraNormal);

	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 5000;
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(skyboxProgram);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "lookMatrix"), 1, GL_TRUE, lookMatrix.m);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "transform"), 1, GL_TRUE, T(cameraPos.x, cameraPos.y, cameraPos.z).m);
	DrawModel(skybox, skyboxProgram, "in_Position", NULL, "in_TexCoord");
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "lookMatrix"), 1, GL_TRUE, lookMatrix.m);
	glUniform3fv(glGetUniformLocation(program, "cameraPos"), 1, &cameraPos);
	transBlade = T(bladePos.x, bladePos.y, bladePos.z);
	for (int i = 0; i < 4; i++){
		mat4 rotBlade = Mult(Rz(M_PI / 2 * i + t), Ry(M_PI / 2));
		mat4 transform = Mult(transBlade, rotBlade);
		drawObject(transform, blade, program);
	}

	glBindTexture(GL_TEXTURE_2D, concrete);
	//drawObject(transWalls, walls, program);
	drawObject(transRoof, roof, program);
	drawObject(transBalcony, balcony, program);

	glBindTexture(GL_TEXTURE_2D, grass);
	//drawObject(transGround,ground, program);
	drawObject(transTeapot,balcony, program);


	glUseProgram(instancingProgram);
	glUniformMatrix4fv(glGetUniformLocation(instancingProgram, "viewMatrix"), 1, GL_TRUE, lookMatrix.m);
	drawModelInstanced(octagon, instancingProgram, 10, t, transCubes);
	drawModelInstanced(octagon, instancingProgram, 10, t, transTeapot);
	glutSwapBuffers();
}

void drawObject(mat4 transform, Model* model, GLuint p)
{
	glUniformMatrix4fv(glGetUniformLocation(p, "transform"), 1, GL_TRUE, transform.m);
	DrawModel(model, p, "in_Position", "in_Normal", "in_TexCoord");
	printError("drawObject()");
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("Lab 3");
	glutDisplayFunc(display);
	initKeymapManager();
	glutPassiveMotionFunc(handleMouse);
	init ();
	glutTimerFunc(16.7, &OnTimer, 0);
	glutMainLoop();
}

void handleMouse(int x, int y)
{
  cameraTarget = (vec3)
		{cos((float)x / 400 * M_PI * 2) * sin((float)y / 300 * M_PI),
		 -(float)y / 300 + 0.5,
		 sin((float)x / 400 * M_PI * 2) * sin((float)y / 300 * M_PI)};
	cameraTarget = VectorAdd(cameraTarget, cameraPos);

	lookMatrix = lookAtv(cameraPos, cameraTarget, cameraNormal);
	cameraDirection = Normalize(VectorSub(cameraTarget, cameraPos));
	printError("handleMouse()");
}

vec3 moveOnKeyInputRelativeCamera(vec3 in)
{
	vec3 forward;
	vec3 leftV;
	if(keyIsDown('x')){
		forward = ScalarMult(cameraDirection, 1.0f);
		leftV = ScalarMult(CrossProduct(cameraDirection, cameraNormal), 1.0f);
	}
	else{
		forward = ScalarMult(cameraDirection, 0.1f);
		leftV = ScalarMult(CrossProduct(cameraDirection, cameraNormal), 0.1f);
	}

  if(keyIsDown('w')) {
    in.x += forward.x;
    in.y += forward.y;
    in.z += forward.z;
	}
  else if (keyIsDown('s')) {
    in.x -= forward.x;
    in.y -= forward.y;
    in.z -= forward.z;
	}

  if(keyIsDown('a')){
		in.x -= leftV.x;
    in.y -= leftV.y;
    in.z -= leftV.z;
	}
  else if(keyIsDown('d')){
		in.x += leftV.x;
    in.y += leftV.y;
    in.z += leftV.z;
	}

  if(keyIsDown('p')){
    printf("(e%f, ", cameraPos.x);
    printf("%f, ", cameraPos.y);
    printf("%f)\n", cameraPos.z);
	}
  if(keyIsDown('q'))
    in.y += 0.1;
  else if(keyIsDown('e'))
    in.y -= 0.1;

	printError("moveonkeyinputrelativecamera()");
  return in;
}
