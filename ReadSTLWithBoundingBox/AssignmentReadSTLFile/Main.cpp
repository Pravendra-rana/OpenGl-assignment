#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"
#include "Mesh.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include<cstring>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"

// Global Variables
const char* APP_TITLE = "Introduction to Modern OpenGL - Loading OBJ Models";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;


FPSCamera fpsCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(1.0, 1.0, 1.0));
OrbitCamera orbitCamera;
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;
float gRadius = 30.0f;
float gYaw = 1.0f;
float gPitch = 0.0f;

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow* window);
bool initOpenGL();

// Shaders
const GLchar* vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"layout (location = 1) in vec3 color;"

"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"\n"
"out vec3 vert_color;"
"void main()"
"{"
"    vert_color = color;"
"    gl_Position = projection * view * model * vec4(pos, 1.0);"
"}";

const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"in vec3 vert_color;"
"out vec4 frag_color;"
"void main()"
"{"
"    frag_color = vec4(0.0,0.0,0.0, 1.0);"
"}";

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);
bool initOpenGL();
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);

//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main()
{

	if (!initOpenGL())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	//const int numModels = 1;
	Mesh mesh[1];
	//Texture2D texture[numModels];

	mesh[0].loadSTL("models/Human.stl");
	//mesh[0].loadSTL("models/Data.stl");

   //mesh[0].loadSTL("models/stl_cube.stl");
	//mesh[1].loadOBJ("models/woodcrate.obj");
	
	//texture[0].loadTexture("textures/crate.jpg", true);
	

	// Model positions
	glm::vec3 modelPos[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),	// crate1
		//glm::vec3(2.5f, 1.0f, 0.0f),	// crate2
		//glm::vec3(0.0f, 0.0f, -2.0f),	// robot
		//glm::vec3(0.0f, 0.0f, 0.0f),// floor
		
	};

	// Model scale
	glm::vec3 modelScale[] = {
		glm::vec3(0.5f, 0.5f, 0.5f),	// crate1
		//glm::vec3(1.0f, 1.0f, 1.0f),	// crate2
		//glm::vec3(1.0f, 1.0f, 1.0f),	// robot
		//glm::vec3(10.0f, 1.0f, 10.0f)	// floor
	};

	// 3. Create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShaderSrc, NULL);
	glCompileShader(vs);

	//glColorPointer(3, GL_FLOAT, 0, vert_colors);

	// Check for compile errors
	GLint result;
	GLchar infoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Vertex shader failed to compile. " << infoLog << std::endl;
	}

	// 4. Create fragment shader
	GLint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fs);

	// Check for compile errors
	glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Fragment shader failed to compile. " << infoLog << std::endl;
	}

	// 5. Create shader program and link shaders to program
	GLint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

	// Check for linker errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Shader program linker failure. " << infoLog << std::endl;
	}

	// Clean up shaders, do not need them anymore since they are linked to a shader program
	glDeleteShader(vs);
	glDeleteShader(fs);

	double lastTime = glfwGetTime();
	glm::vec3 cubePos = glm::vec3(-15.0f, 5.0f, 3.0f);
	const float x = 3.14;
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		// Poll for and process events
		glfwPollEvents();
		//update(deltaTime);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0), view(1.0), projection(1.0);

		// Create the View matrix
		//view = fpsCamera.getViewMatrix();
		orbitCamera.setLookAt(cubePos);
		orbitCamera.rotate(gYaw, gPitch);
		orbitCamera.setRadius(gRadius);
		view = orbitCamera.getViewMatrix();

		// Create the projection matrix
		//projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 200.0f);
		projection = glm::perspective(glm::radians(orbitCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 200.0f);

		// Must be called BEFORE setting uniforms because setting uniforms is done
		// on the currently active shader program.

		glUseProgram(shaderProgram);

		// Pass the matrices to the shader
		
	 	glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		


		// Render the scene
		//for (int i = 0; i < numModels; i++)
		//{
			model = glm::translate(glm::mat4(1.0), modelPos[0]) * glm::scale(glm::mat4(1.0), modelScale[0])*glm::rotate(glm::mat4(1.0),x,glm::vec3(0.0f,1.0f,1.0f));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			//shaderProgram.setUniform("model", model);

			//texture[i].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[0].draw();			// Render the OBJ mesh
			//mesh[0].boundingbox();
			//mesh[0].draw();
			//texture[i].unbind(0);
		//}

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);

		//lastTime = currentTime;
	}

	glfwTerminate();

	return 0;
}

bool initOpenGL()
{
	// Intialize GLFW 
	// GLFW is configured.  Must be called before calling any GLFW functions
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3


	// Create an OpenGL 3.3 core, forward compatible context window
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gWindow == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

	// Hides and grabs cursor, unlimited movement
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);
	glfwSetCursorPosCallback(gWindow, glfw_onMouseMove);

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	// Define the viewport dimensions
	glViewport(0, 0, gWindowWidth, gWindowHeight);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY)
{
	static glm::vec2 lastMousePos = glm::vec2(0, 0);



	// Update angles based on Left Mouse Button input to orbit around the cube
	if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT) == 1)
	{
		// each pixel represents a quarter of a degree rotation (this is our mouse sensitivity)
		gYaw -= ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
		gPitch += ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
	}



	// Change orbit camera radius with the Right Mouse Button
	if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT) == 1)
	{
		float dx = 0.01f * ((float)posX - lastMousePos.x);
		float dy = 0.01f * ((float)posY - lastMousePos.y);
		gRadius += dx - dy;
	}



	lastMousePos.x = (float)posX;
	lastMousePos.y = (float)posY;
}

//-----------------------------------------------------------------------------
// Called by GLFW when the mouse wheel is rotated
//-----------------------------------------------------------------------------
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	//double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;
	double fov = orbitCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	//fpsCamera.setFOV((float)fov);
	orbitCamera.setFOV((float)fov);
}

//-----------------------------------------------------------------------------
// Update stuff every frame
//-----------------------------------------------------------------------------
void update(double elapsedTime)
{
	// Camera orientation
	double mouseX, mouseY;

	// Get the current mouse cursor position delta
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
	orbitCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Clamp mouse cursor to center of screen
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Camera FPS movement

	// Forward/backward
	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		orbitCamera.move(MOVE_SPEED * (float)elapsedTime * orbitCamera.getLook());
	//fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		orbitCamera.move(MOVE_SPEED * (float)elapsedTime * -orbitCamera.getLook());

	// Strafe left/right
	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		orbitCamera.move(MOVE_SPEED * (float)elapsedTime * -orbitCamera.getRight());
	else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		orbitCamera.move(MOVE_SPEED * (float)elapsedTime * orbitCamera.getRight());

	// Up/down
	if (glfwGetKey(gWindow, GLFW_KEY_Z) == GLFW_PRESS)
		orbitCamera.move(MOVE_SPEED * (float)elapsedTime * orbitCamera.getUp());
	else if (glfwGetKey(gWindow, GLFW_KEY_X) == GLFW_PRESS)
		orbitCamera.move(MOVE_SPEED * (float)elapsedTime * -orbitCamera.getUp());
}

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// The C++ way of setting the window title
		std::ostringstream outs;
		outs.precision(3);	// decimal places
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}