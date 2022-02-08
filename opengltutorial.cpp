// General
#include "Config.h"
#ifndef STB_IMAGE_IMPLEMENTATION // Causes errors if in Config.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
#include <iomanip>

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

float cameraSpeed = 20.0f;

//glm::vec3 cam_spawn = glm::vec3(-1030.0f, -75.0f, 1000.0f);
//glm::vec3 cam_spawn = glm::vec3(0.0f, 0.0, -26.0f);
glm::vec3 cam_spawn = glm::vec3(0.0f, 0.0, 0.0f);

FPSCamera camera(cam_spawn, glm::vec3(0.0f, 0.0f, -1.0f), cameraSpeed);
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
	glEnable(GL_MULTISAMPLE);

	// Generate terrain
	ChunkManager terrain((32) + 1 - 2, glm::vec3(0.0f), 3, "genHeightmap.comp", "drawTexture.comp");
	std::cout << std::setprecision(6);
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

	//////////////////////////////////////////////////////////////////////

		// Lighting

	//Light sun = Light();
	//sun.setPos(camera.getPos());
	//sun.setBrightness(0.9f);
	//sun.setLighting(glm::vec3(0.5f), glm::vec3(0.75f), glm::vec3(1.0f));
	//sun.useAsDirectionalLight(objectShader, 0);
	Light sun = Light();
	glm::vec3 light_dir = glm::vec3((float)cos(glfwGetTime() * 1), -1.0f, (float)sin(glfwGetTime() * 1));
	sun.setPos(camera.getPos());
	sun.setDir(light_dir);
	sun.setBrightness(1.2f);
	sun.setLighting(glm::vec3(0.5f), glm::vec3(0.75f), glm::vec3(1.0f));
	sun.useAsDirectionalLight(objectShader, 0);

	// filler lights (need at least 1 spotlight and 1 point light)
	sun.setBrightness(0.0f);
	sun.setAngle(glm::radians(30.0f));
	sun.setPointLightFade(1.0f, 0.09f, 0.032f);
	sun.useAsPointLight(objectShader, 0);
	sun.useAsSpotlight(objectShader, 0);

	objectShader.setVec3("viewPos", camera.getPos());
	objectShader.setMat4("model", glm::mat4(1.0f));

	//////////////////////////////////////////////////////////////////////

	bool should_close = glfwWindowShouldClose(window);
	int frameCount = 0;
	Stopwatch fpsCounter = Stopwatch();
	double oldtime = 0.0f;
	double deltatime = 0.0f;
	bool update_terrain = true;

	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point b = std::chrono::system_clock::now();
	const double framerateFrequency = 500 / 60.0;
	while (!should_close) // Loop
	{
		// Limit framerate
		a = std::chrono::system_clock::now();
		const std::chrono::duration<double, std::milli> work_time = a - b;
		deltatime = std::chrono::duration<double>(work_time).count();

		if (work_time.count() < framerateFrequency)
		{
			std::chrono::duration<double, std::milli> delta_ms(framerateFrequency - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		b = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> sleep_time = b - a;

		// Lighting
		const double nowtime = glfwGetTime();
		const glm::vec3 light_dir((float)cos(nowtime * 1.1), -1.0f, (float)sin(nowtime * 1.1));
		sun.setDir(light_dir);
		

		
		// Clear window
		glClearColor(0.0f, 0.3f, 0.3f, 1.0f); // RGBA, f makes the literal a float instead of a double
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // RGBA, f makes the literal a float instead of a double
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//////////////////////////////////////////////////////////////////////
		// Handle input
		
		
		// Keyboard
				// Misc Important
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // exit on escape
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
			camera = FPSCamera(cam_spawn, glm::vec3(0, 0, -1.0f), cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
			camera = FPSCamera(camera.getPos(), glm::vec3(0, 0, -1.0f), cameraSpeed);
		// Misc Unimportant
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			update_terrain = true;
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
			update_terrain = false;
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
		objectShader.setFloat("wavetime", 2.0f * (float)glfwGetTime());

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

		if (update_terrain) {
			terrain.set_pos(-camera.getPos());
		}
		terrain.set_direction(camera.getDirection());
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