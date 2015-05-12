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

Model *bunny;
Model *octagon;
Model *skybox;
mat4 transBunny;
mat4 transCubes;

GLuint skyTexture;

mat4 lookMatrix;
vec3 cameraPos;
vec3 cameraTarget;
vec3 cameraNormal;
vec3 cameraDirection;


void init(void)
{
  cameraPos = (vec3){1.5f, 1.6f, 2.0f};
  cameraTarget = (vec3){-10.0f, -10.0f, -10.0f};
  cameraNormal = (vec3){0.0f, 1.0f, 0.0f};
  lookMatrix = lookAtv(cameraPos, cameraTarget, cameraNormal);

	dumpInfo();

	bunny = LoadModelPlus("./models/bunnyplus.obj");
	octagon = LoadModelPlus("./models/octagon.obj");
	skybox = LoadModelPlus("./models/skybox.obj");

	transBunny = T(34.4, 6.4, -30.4);
	transCubes = T(-2.2, -2.3, 10.2);

	LoadTGATextureSimple("./textures/SkyBox512.tga", &skyTexture);

	printError("GL inits");
	glClearColor(1.0, 1.0, 1.0, 0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	// Load and compile shader
	skyboxProgram = loadShaders("./shaders/skybox.vert", "./shaders/skybox.frag");
	instancingProgram = loadShaders("./shaders/instancing.vert", "./shaders/instancing.frag");

	GLfloat projectionMatrix[] = {2.0f*near/(right-left), 0.0f,
                                (right+left)/(right-left), 0.0f,
                                0.0f, 2.0f*near/(top-bottom),
                                (top+bottom)/(top-bottom), 0.0f,
                                0.0f, 0.0f, -(far + near)/(far - near),
                                -2*far*near/(far - near),
                                0.0f, 0.0f, -1.0f, 0.0f };

	glUseProgram(skyboxProgram);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(skyboxProgram, "texUnit"), 0);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

	glUseProgram(instancingProgram);
	glUniformMatrix4fv(glGetUniformLocation(instancingProgram, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
	glUniform1i(glGetUniformLocation(instancingProgram, "texUnit"), 0);
	setupInstancedVertexAttributes(instancingProgram, bunny);
	printError("init(): End");
}

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
	printError("OnTimer()");
}

void display(void) {
	printError("pre display");
	cameraPos = moveOnKeyInputRelativeCamera(cameraPos);
	cameraTarget = moveOnKeyInputRelativeCamera(cameraTarget);
	lookMatrix = lookAtv(cameraPos, cameraTarget, cameraNormal);

	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 5000;
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

	glUseProgram(instancingProgram);
	glUniformMatrix4fv(glGetUniformLocation(instancingProgram, "viewMatrix"), 1, GL_TRUE, lookMatrix.m);

	drawModelInstanced(octagon, instancingProgram, 10, t, transCubes);
	drawModelInstanced(bunny, instancingProgram, 10, t, transBunny);
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
