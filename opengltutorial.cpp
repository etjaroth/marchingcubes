// General
#include "Config.h"
#ifndef STB_IMAGE_IMPLEMENTATION // Causes errors if in Config.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

// My Classes
#include "Shader.h" // Shading
#include "Light.h"

#include "camera.h" // Cameras
#include "FPSCamera.h"
#include "FlyCamera.h"

#include "MinModel.h"
#include "MarchingCubes.h"
#include "ChunkManager.h"

// FPS control
#include "Stopwatch.h"
#include <chrono>
#include <thread>

// Display
unsigned int screenx = 800;
unsigned int screeny = 600;

float cameraSpeed = 7.0f;


//glm::vec3 cam_spawn = glm::vec3(-1030.0f, -75.0f, 1020.0f);
glm::vec3 cam_spawn = glm::vec3(0.0f, 0.0f, -3.0f);



FPSCamera camera(cam_spawn, glm::vec3(0, 0, -1.0f), cameraSpeed);
//unsigned int loadTexture(const std::string filename, unsigned int colortype, bool flip);

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void error_callback(int error, const char* description);

int main() {
	// Setup
		// GLFW Initilisation
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// Window Creation
	GLFWwindow* window = glfwCreateWindow(screenx, screeny, "openGL Tutorial", NULL, NULL); // Make window
	if (window == NULL) // Window failed
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate(); // End program
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Resize the window with framebuffer_size_callback()
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetErrorCallback(error_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initilize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate(); // End program
		return -1;
	}

	// Declare window size and GL settings
	glViewport(0, 0, screenx, screeny);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); // Transparencey isn't perfect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glEnable(GL_MULTISAMPLE);

	// Generate terrain
	ChunkManager terrain(31, glm::vec3(0.0f), 1, "drawTexture.comp");


	// Describe Shapes(s)
	Shader objectShader("VertexShader.vert", "FragmentShader.frag");
	Shader lightingShader("VertexShader.vert", "lightingShader.frag");

	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	// Load Shaders
		// Normal Shader
	//Shader objectShader("VertexShader.vert", "FragmentShader.frag");
	objectShader.use();
	objectShader.setInt("texture1", 0);
	objectShader.setInt("texture2", 1);
	objectShader.dontuse();
	// Lighting Shaders

	// Load texture
	objectShader.use();
	glm::mat4 texturetransformmat(1.0f);

	// Misc.
	unsigned int transformLoc = glGetUniformLocation(objectShader.shaderProgram, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(texturetransformmat));

	//////////////////////////////////////////////////////////////////////////

	bool should_close = glfwWindowShouldClose(window);
	int frameCount = 0;
	Stopwatch clock = Stopwatch();
	Stopwatch fpsCounter = Stopwatch();
	double oldtime = 0.0f;
	double deltatime = 0.0f;
	while (!should_close) // Loop
	{
		// DeltaTime
		double nowtime = glfwGetTime();
		deltatime = nowtime - oldtime; // Change in time
		oldtime = nowtime;

		//////////////////////////////////////////////////////////////////////

		// Lighting

		Light sun = Light();
		glm::vec3 light_dir = glm::vec3((float)cos(glfwGetTime()), -1.0f, (float)sin(glfwGetTime()));

		std::cout << light_dir.x << ", " << light_dir.y << ", " << light_dir.z << std::endl;

		sun.setPos(camera.getPos());
		sun.setDir(light_dir);
		sun.setBrightness(1.0f);
		sun.setLighting(glm::vec3(0.5f), glm::vec3(0.75f), glm::vec3(1.0f));
		sun.useAsDirectionalLight(objectShader, 0);

		// filler lights
		sun.setBrightness(0.0f);
		sun.setAngle(glm::radians(30.0f));
		sun.setPointLightFade(1.0f, 0.09f, 0.032f);
		sun.useAsPointLight(objectShader, 0);
		sun.useAsSpotlight(objectShader, 0);

		////// Moving Light
		//float lightspeed = 1.0f;
		//float lightRadius = 10.0f + 3.0f * (float)cos(glfwGetTime()) * (float)sin(glfwGetTime());
		//glm::vec3 lightPos = glm::vec3(0.0f, -1.0f, 0.0f);
		//lightPos.x = lightRadius * (float)sin(glfwGetTime() * lightspeed);
		//lightPos.z = lightRadius * (float)cos(glfwGetTime() * lightspeed);

		//// Lighting -> objectShader
		//Light light1 = Light();
		////light1.setPos(lightPos);
		//light1.setPos(glm::vec3(1.0f));
		//light1.setDir(glm::vec3(0.0f, -1.0f, 0.0f));
		//light1.setAngle(glm::radians(30.0f));
		//light1.setLighting(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//light1.setPointLightFade(1.0f, 0.09f, 0.032f);
		//light1.setBrightness(1.0f);
		//light1.useAsDirectionalLight(objectShader, 0);
		//light1.useAsPointLight(objectShader, 0);




		objectShader.setVec3("viewPos", camera.getPos());

		//////////////////////////////////////////////////////////////////////

		// Clear window
		glClearColor(0.0f, 0.3f, 0.3f, 1.0f); // RGBA, f makes the literal a float instead of a double
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//////////////////////////////////////////////////////////////////////
		// Handle input

		// Keyboard
				// Misc Important
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // exit on escape
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
			camera = FPSCamera(cam_spawn, glm::vec3(0, 0, -1.0f), cameraSpeed);
		// Misc Unimportant
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Fps Movement

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.pan(glm::vec3(0.0f, 0.0f, -deltatime)); // z
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.pan(glm::vec3(0.0f, 0.0f, deltatime)); // z
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.pan(glm::vec3(-deltatime, 0.0f, 0.0f)); // x
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.pan(glm::vec3(deltatime, 0.0f, 0.0f)); // x
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			camera.pan(glm::vec3(0.0f, -deltatime, 0.0f)); // yaw
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			camera.pan(glm::vec3(0.0f, deltatime, 0.0f)); // yaw
				// Fly Movement
			// Rotation
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
			camera.rotate((float)(-deltatime), 0.0f, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			camera.rotate((float)deltatime, 0.0f, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			camera.rotate(0.0f, (float)(-deltatime), 0.0f);
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			camera.rotate(0.0f, (float)deltatime, 0.0f);

		//////////////////////////////////////////////////////////////////////

		// Perspective
		objectShader.use();
		// View Matrix (Camera) (World -> View)
		camera.setViewLoc(glGetUniformLocation(objectShader.shaderProgram, "view"));
		glUniformMatrix4fv(camera.getViewLoc(), 1, GL_FALSE, glm::value_ptr(camera.getView())); // Pass to shader

		// Projection Matrix
		glm::mat4 projection = glm::mat4(1.0f); // Projection Matrix (View -> Clip)
		if (((float)screenx / (float)screeny) < 1) { std::cout << "Perspective might be disorted" << std::endl; }
		projection = glm::perspective(glm::radians(45.0f), std::max(((float)screenx / (float)screeny), 1.0f), 0.1f, 100.0f);
		//projection = glm::ortho(glm::radians(45.0f), (float)screenx / (float)screeny, 0.1f, 100.0f);
		objectShader.setMat4("projection", projection);

		// Pass to lighting shader
		lightingShader.use();
		int lightingprojLoc = glGetUniformLocation(lightingShader.shaderProgram, "projection"); // Find shader
		glUniformMatrix4fv(lightingprojLoc, 1, GL_FALSE, glm::value_ptr(projection));
		camera.setViewLoc(glGetUniformLocation(lightingShader.shaderProgram, "view"));
		glUniformMatrix4fv(camera.getViewLoc(), 1, GL_FALSE, glm::value_ptr(camera.getView())); // Pass to shader (shatters cube)

		///////////////////////////////////////////////////////////////////////

		terrain.set_pos(-camera.getPos());
		terrain.render(&objectShader);

		///////////////////////////////////////////////////////////////////////

		// Display
		glfwSwapBuffers(window);
		glfwPollEvents(); // Calls functions from inputs
		should_close = glfwWindowShouldClose(window);

		///////////////////////////////////////////////////////////////////////

		// Measure fps
		frameCount++;
		// If a second has passed.
		if (fpsCounter.get_time() >= 1.0)
		{
			// Print the frame count
			std::cout << "FPS: " << frameCount << "\n" << std::endl;

			fpsCounter.reset();
			frameCount = 0;
		}

		// Limit framerate
		const unsigned int max_framerate = 120; // why does this give ~63fps?
		clock.stop();
		unsigned int sleep = (1000 / max_framerate - clock.get_time());
		if (sleep < 100) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
		}
		clock.start();

	}

	// Cleanup
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // Resizes window when needed
	screenx = width;
	screeny = height;
	glViewport(0, 0, width, height);
}

double lastmousex = screenx / 2.0;
double lastmousey = screeny / 2.0;
bool firstmousecallback = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstmousecallback) {
		firstmousecallback = false;
		lastmousex = xpos;
		lastmousey = ypos;
	}
	else {
		double xoffset = xpos - lastmousex;
		double yoffset = ypos - lastmousey; // y-coordinates are inverted

		const float sensitivity = 0.001f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		lastmousex = xpos;
		lastmousey = ypos;

		camera.rotate((float)yoffset, (float)xoffset, 0.0f);
	};
}

void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

unsigned int loadTexture(const std::string filename, unsigned int colortype, bool flip) {
	stbi_set_flip_vertically_on_load(flip);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	unsigned int texture;
	glGenTextures(1, &texture);
	//glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, texture); // using texture

	glTexImage2D(GL_TEXTURE_2D, 0, colortype, width, height, 0, colortype, GL_UNSIGNED_BYTE, data); // Create texture
	glGenerateMipmap(GL_TEXTURE_2D); // lowers resolution for far away objects
	if (data) { // Error handling
		glTexImage2D(GL_TEXTURE_2D, 0, colortype, width, height, 0, colortype, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data); // Frees memory used when loading texture
	return texture;
}
//*/