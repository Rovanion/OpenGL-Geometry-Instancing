#ifndef LEAPMUSIC_GRAPHICS
#define LEAPMUSIC_GRAPHICS

#include <atomic>
#include <string>

#include <GL/gl.h>

#include "libraries/VectorUtils3.h"
#include "libraries/LoadObject.h"


class Main {
public:
	/**
	 * @brief Initializes the graphics engine.
	 *
	 * @param argc The number of arguments passed.
	 * @param argv Arguments which are passed on to freeGLUT.
	 */
	static int init(int argc, char** argv);

	/**
	 * @brief If you want the graphics system to show the user a sign
	 * with something on, this is the function to call.
	 *
	 * @param pathToImage A string with the path to the texture which is
	 * to be shown on the billboard.
	 */
	static void showSign(std::string);

	/**
	 * @brief Hides the currently showing sign, if there is one.
	 */
	static void hideSign(void);

	static void openMenu(void);
	static void closeMenu(void);
	static void onDisplay(void);


	static std::atomic<float> handX;
	static std::atomic<float> handY;
	static std::atomic<bool> playing;
	static std::atomic<bool> recording;

private:
	static int initResources(void);
	static void freeResources(void);
	static void onTimer(int);
	static void drawObject(mat4, Model*, GLuint);

	static GLuint program;
	static GLuint green;
	static GLuint white;
	static GLuint red;
	static GLuint darkBlue;
	static GLuint signTexture;
	static Model* bunny;
	static Model* plane;
	static mat4   transHand;
	static mat4   transSign;
	static mat4   transPlane;

	static std::atomic<bool> signShowing;
	static std::atomic<bool> loadNewSign;
	static std::atomic<bool> menuOpen;
	static std::string signTexturePath;

	static constexpr GLfloat near = 1.0;
	static constexpr GLfloat far = 300.0;
	static constexpr GLfloat right = 0.5;
	static constexpr GLfloat left = -0.5;
	static constexpr GLfloat top = 0.5;
	static constexpr GLfloat bottom = -0.5;
	static const GLfloat viewFrustum[];
  static const vec3 cameraPosition;
  static const vec3 cameraTarget;
	static const vec3 cameraNormal;
  static const mat4 lookMatrix;
};

#endif
