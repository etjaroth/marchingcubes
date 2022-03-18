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

#include "Settings.h"
#include "Sky.h"

// FPS control
#include "Stopwatch.h"
#include <chrono>
#include <thread>

#include "Binding.h"

// Settings
Settings settings{};

// Camera
FPSCamera camera(settings.getConstants().spawnpoint,
	settings.getConstants().spawnViewDir,
	settings.getConstants().cameraSpeed);

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
	GLFWwindow* window = 
		glfwCreateWindow(settings.getConstants().screenSize.x, 
			settings.getConstants().screenSize.y, "Marching Cubes Demo", NULL, NULL); // Make window
	settings.loadControls(window);

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
	
	// Initilize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate(); // End program
		return -1;
	}

	// Declare window size and GL settings
	glViewport(0, 0, settings.getConstants().screenSize.x, settings.getConstants().screenSize.y);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);



	// Generate terrain
	const unsigned long long chunkSize = settings.getConstants().chunkSize; // should be a power of 2
	const unsigned long long chunkRadius = settings.getConstants().chunkRadius;
	const unsigned long long chunkCubeLength = 2 * chunkRadius + 1;

	const unsigned long long renderDistance = chunkSize * (chunkRadius + 1);

	const unsigned long long VERTEX_SSBO_SIZE = 4 * sizeof(float) * 3 * (chunkSize - 1) * (chunkSize - 1) * (chunkSize - 1) * 12; // 12 edges per cube
	const unsigned long long HEIGHTMAP_SIZE = sizeof(float) * (chunkSize + 2); // 12 edges per cube
	const unsigned long long LANDSCAPE_SIZE = sizeof(float) * (chunkSize + 1) * (chunkSize + 1) * (chunkSize + 1); // 12 edges per cube
	const unsigned long long INDIRECT_SSBO_SIZE = 5 * sizeof(GLuint); // 12 edges per cube
	const unsigned long long EBO_SIZE = (chunkSize - 1) * (chunkSize - 1) * (chunkSize - 1) * 15 * sizeof(GLuint); // 12 edges per cube
	
	const unsigned long long VERTEX_SSBO_MEMORY = chunkCubeLength * chunkCubeLength * chunkCubeLength * VERTEX_SSBO_SIZE;
	const unsigned long long HEIGHTMAP_MEMORY = chunkCubeLength * chunkCubeLength * HEIGHTMAP_SIZE;
	const unsigned long long LANDSCAPE_MEMORY = chunkCubeLength * chunkCubeLength * chunkCubeLength * LANDSCAPE_SIZE;
	const unsigned long long INDIRECT_SSBO_MEMORY = chunkCubeLength * chunkCubeLength * chunkCubeLength * LANDSCAPE_SIZE;
	const unsigned long long EBO_MEMORY = chunkCubeLength * chunkCubeLength * chunkCubeLength * EBO_SIZE;

	const unsigned long long FINAL_MEMORY_USAGE = VERTEX_SSBO_MEMORY + HEIGHTMAP_MEMORY;
	const unsigned long long GENERATING_MEMORY_USAGE = FINAL_MEMORY_USAGE + LANDSCAPE_MEMORY + INDIRECT_SSBO_MEMORY + EBO_MEMORY;

	ChunkManager terrain(chunkSize + 1 - 2, glm::vec3(0.0f), chunkRadius, "genHeightmap.comp", "drawTexture.comp");
	std::cout << "Expected memory usage:\n    FINAL_MEMORY_USAGE (Upper Bound): "
		<< static_cast<float>(FINAL_MEMORY_USAGE) / 100000000.0 << 
		" GB\n    GENERATING_MEMORY_USAGE (Upper Bound): "
		<< static_cast<float>(GENERATING_MEMORY_USAGE) / 100000000.0 << " GB" << std::endl;

	//ChunkManager terrain((32) + 1 - 2, glm::vec3(0.0f), 3, "genHeightmap.comp", "drawTexture.comp");
	std::cout << std::setprecision(6);
	// Describe Shapes(s)
	Shader objectShader("VertexShader.vert", "FragmentShader.frag");

	// Load Shaders
		// Normal Shader
	objectShader.use();
	objectShader.setInt("texture1", 0);
	objectShader.setInt("texture2", 1);
	objectShader.dontuse();
	// Lighting Shaders

	// Load texture
	objectShader.use();
	glm::mat4 texturetransformmat(1.0f);
	objectShader.setMat4("model", glm::mat4(1.0f));

	// Misc.
	unsigned int transformLoc = glGetUniformLocation(objectShader.shaderProgram, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(texturetransformmat));

	Sky sky{};

	//////////////////////////////////////////////////////////////////////

	bool should_close = glfwWindowShouldClose(window);
	int frameCount = 0;
	Stopwatch fpsCounter = Stopwatch();
	double oldtime = 0.0f;
	double deltatime = 0.0f;


	glm::mat4 A{ 1.0f };
	glm::mat4 B{ 2.0f };
	glm::mat4 C = (A * glm::transpose(B));

	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point b = std::chrono::system_clock::now();
	const double framerateFrequency = 500 / 60.0;

	// Toggles
	bool meshViewToggle = true;
	bool updateTerrain = true;

	glEnable(GL_CULL_FACE);

	unsigned int counter = 0;
	double originalFrameStartTime = glfwGetTime();
	while (!should_close) // Loop
	{

		double frameStartTime = glfwGetTime() - originalFrameStartTime;
		frameStartTime *= 2.0;
		frameStartTime *= 0.0;

		++counter;
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
		
		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//////////////////////////////////////////////////////////////////////
		// Handle input
		
		
		// Keyboard
				// Misc Important
		settings.readInput(window);

		if (settings.getActions().quit) // exit on escape
			glfwSetWindowShouldClose(window, true);
		if (settings.getActions().resetPos)
			camera = FPSCamera(settings.getConstants().spawnpoint, settings.getConstants().spawnViewDir, settings.getConstants().cameraSpeed);
		if (settings.getActions().resetViewDir)
			camera = FPSCamera(camera.getPos(), glm::vec3(0, 0, -1.0f), settings.getConstants().cameraSpeed);
		// Misc Unimportant
		if (settings.getActions().toggleMeshView) {
			if (meshViewToggle) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			meshViewToggle = !meshViewToggle;
		}
		if (settings.getActions().toggleTerrainUpdates)
			updateTerrain = !updateTerrain;
		// Fps Movement
		if (settings.getActions().moveNorth)
			camera.pan(glm::vec3(0.0f, 0.0f, -deltatime)); // z
		if (settings.getActions().moveSouth)
			camera.pan(glm::vec3(0.0f, 0.0f, deltatime)); // z
		if (settings.getActions().moveWest)
			camera.pan(glm::vec3(-deltatime, 0.0f, 0.0f)); // x
		if (settings.getActions().moveEast)
			camera.pan(glm::vec3(deltatime, 0.0f, 0.0f)); // x
		if (settings.getActions().moveUp)
			camera.pan(glm::vec3(0.0f, -deltatime, 0.0f)); // yaw
		if (settings.getActions().moveDown)
			camera.pan(glm::vec3(0.0f, deltatime, 0.0f)); // yaw
				// Fly Movement
			// Rotation
		if (settings.getActions().rotateCameraNorth)
			camera.rotate((float)(-deltatime), 0.0f, 0.0f);
		if (settings.getActions().rotateCameraSouth)
			camera.rotate((float)deltatime, 0.0f, 0.0f);
		if (settings.getActions().rotateCameraWest)
			camera.rotate(0.0f, (float)(-deltatime), 0.0f);
		if (settings.getActions().rotateCameraEast)
			camera.rotate(0.0f, (float)deltatime, 0.0f);

		//////////////////////////////////////////////////////////////////////

		// Render in stages
		
		glDisable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		if (true) {
			sky.generateSky(camera, frameStartTime);
		}
		sky.render(camera, frameStartTime);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);

		// Perspective
		objectShader.use();
		objectShader.setVec3("viewPos", -camera.getPos());

		// View Matrix (Camera) (World -> View)
		camera.setViewLoc(glGetUniformLocation(objectShader.shaderProgram, "view"));
		glUniformMatrix4fv(camera.getViewLoc(), 1, GL_FALSE, glm::value_ptr(camera.getView())); // Pass to shader
		// Projection Matrix
		glm::mat4 projection = glm::mat4(1.0f); // Projection Matrix (View -> Clip)
		if ((static_cast<float>(settings.getConstants().screenSize.x) / 
			static_cast<float>(settings.getConstants().screenSize.y) < 1))
		{ std::cout << "Perspective might be disorted" << std::endl; }
		projection = glm::perspective(glm::radians(45.0f), std::max(((float)settings.getConstants().screenSize.x / (float)settings.getConstants().screenSize.y), 1.0f), 0.1f, settings.getConstants().farClipPlain);
		//projection = glm::ortho(glm::radians(45.0f), (float)settings.getConstants().screenSize.x / (float)settings.getConstants().screenSize.y, 0.1f, 100.0f);
		objectShader.setMat4("projection", projection);

		///////////////////////////////////////////////////////////////////////

		if (updateTerrain) {
			terrain.set_pos(-camera.getPos());
		}
		terrain.set_direction(camera.getDirection());
		//terrain.render(&objectShader, frameStartTime);

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
	//glDeleteFramebuffers(1, &framebuffer);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // Resizes window when needed
	settings.setScreenSize(glm::uvec2(width, height));
	glViewport(0, 0, width, height);
}

double lastmousex = settings.getConstants().screenSize.x / 2.0;
double lastmousey = settings.getConstants().screenSize.y / 2.0;
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