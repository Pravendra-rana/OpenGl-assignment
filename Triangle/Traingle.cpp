
#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"


// Global Variables
const char* APP_TITLE = "Introduction to Modern OpenGL - Hello Colored Triangle";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;


// Shaders
const GLchar* vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"layout (location = 1) in vec3 color;"
"\n"
"out vec3 vert_color;"
"void main()"
"{"
"    vert_color = color;"
"    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);"
"}";

const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"in vec3 vert_color;"
"out vec4 frag_color;"
"void main()"
"{"
"    frag_color = vec4(vert_color, 1.0f);"
"}";

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);
bool initOpenGL();

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

	GLfloat vert_pos[] = {
		 0.0f,  0.5f, 0.0f,	// Top
		 0.5f, -0.5f, 0.0f,	// Right 
		-0.5f, -0.5f, 0.0f,	// Left
	};

	/*GLfloat vert_colors[] = {

		1.0f*time,(60-1.0f*time)/30,0.0f*time/60,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};*/


	GLfloat vert_colors[] = {
		1.0f, 0.0f, 0.0f,  // vertex 0 color
		0.0f, 1.0f, 0.0f,  // vertex 1 color
		0.0f, 0.0f, 1.0f,  // vertex 2 color
	};

	// 2. Set up buffers on the GPU
	GLuint vbo, vbo2, vao;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_pos), vert_pos, GL_STATIC_DRAW);

	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_colors), vert_colors, GL_DYNAMIC_DRAW);


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Now bind the color VBO and set the vertix attrib pointer
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glEnableClientState(GL_VERTEX_ARRAY);

	glEnableClientState(GL_COLOR_ARRAY);

	// 3. Create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShaderSrc, NULL);
	glCompileShader(vs);

	glColorPointer(3, GL_FLOAT, 0, vert_colors);

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


	// Rendering loop
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);

		// Poll for and process events
		glfwPollEvents();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Render the quad (two triangles
		glUseProgram(shaderProgram);
		glBindVertexArray(vao);

		double time = glfwGetTime();
		float transitionDuration = 10.0f;  // Duration in seconds
		float t = static_cast<float>(fmod(time, transitionDuration) / transitionDuration);

		// Calculate the color values for each vertex
		//GLfloat vert_colors[] = {
		//	1.0f - t, 0.0f + t, 0.0f,    // vertex 0 color (Red to Blue)
		//	0.0f + t, 1.0f - t, 0.0f,    // vertex 1 color (Blue to Green)
		//	0.0f + t, 0.0f, 1.0f - t     // vertex 2 color (Blue to Red)
		//};

		GLfloat vert_colors[] = {
	   1.0f - t, t, 0.0f,     // vertex 0 color (Red to Green)
	   0.0f, 1.0f - t, t,     // vertex 1 color (Green to Blue)
	   t, 0.0f, 1.0f - t      // vertex 2 color (Blue to Red)
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert_colors), vert_colors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);



		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);



		// Swap front and back buffers
		glfwSwapBuffers(gWindow);
	}

	// Clean up
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();

	return 0;
}

//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------
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

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	// Define the viewport dimensions
	glfw_onFramebufferSize(gWindow, gWindowWidth, gWindowHeight);

	return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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