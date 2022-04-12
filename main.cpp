// General
#include "Config.h"
#ifndef STB_IMAGE_IMPLEMENTATION // Causes errors if in Config.h
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push, 0)
#include "stb_image.h"
#pragma warning(pop)
#endif
#include <iomanip>

// My Classes
#include "Shader.h" // Shading
#include "Light.h"

#include "camera.h" // Cameras
#include "FPSCamera.h"
#include "FlyCamera.h"

#include "MarchingCubes.h"
#include "ChunkManager.h"

#include "Settings.h"
#include "Sky.h"

#include "BulletPhysicsWorld.h"
#include "OpenGLWithWindow.h"

// FPS control
#include "Stopwatch.h"
#include <chrono>
#include <thread>

// Settings
Settings settings{};

// Camera
FPSCamera camera(settings.getConstants().spawnpoint,
	settings.getConstants().spawnViewDir,
	settings.getConstants().cameraSpeed);

// Callback
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
	GLsizei length, const char* message, const void* userParam); // Debug log

int main() {
	OpenGLWithWindow opengl{ camera, settings };

	// Debug output
	{
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_FALSE);
	}

	// Generate terrain
	const unsigned long long chunkSize = settings.getConstants().chunkSize; // should be a power of 2
	const unsigned long long chunkRadius = settings.getConstants().chunkRadius;

	ChunkManager terrain(chunkSize + 1 - 2, glm::vec3(0.0f), chunkRadius, "genHeightmap.comp", "drawTexture.comp");
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

	//Sky sky{};
	Sky sky{ glm::uvec2(1024), "SimpleSkyGenerator.comp" };

	//////////////////////////////////////////////////////////////////////

	bool should_close = opengl.shouldClose();
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
	bool meshViewToggle = false;
	bool updateTerrain = true;

	// Bullet Setup
	BulletPhysicsWorld physics();


	unsigned int counter = 0;
	//double originalFrameStartTime = glfwGetTime() + 10.0;
	double originalFrameStartTime = glfwGetTime() + 32.0;
	while (!should_close) // Loop
	{

		double frameStartTime = glfwGetTime() - originalFrameStartTime;
		//frameStartTime = -3.0;
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
		opengl.readInputToSettings();

		if (settings.getActions().quit) // exit on escape
			opengl.close();
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

		const double dayNightSpeed = 1.0 / 32.0;
		//const double dayNightSpeed = 1.0 / 24.0;

		glDisable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		if (true) {
			sky.generateSky(camera, dayNightSpeed * frameStartTime);
		}
		sky.render(camera, dayNightSpeed * frameStartTime);
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
		{
			std::cout << "Perspective might be disorted" << std::endl;
		}
		projection = glm::perspective(glm::radians(45.0f), std::max(((float)settings.getConstants().screenSize.x / (float)settings.getConstants().screenSize.y), 1.0f), 0.1f, settings.getConstants().farClipPlain);
		//projection = glm::ortho(glm::radians(45.0f), (float)settings.getConstants().screenSize.x / (float)settings.getConstants().screenSize.y, 0.1f, 100.0f);
		objectShader.setMat4("projection", projection);

		///////////////////////////////////////////////////////////////////////

		if (updateTerrain) {
			terrain.set_pos(-camera.getPos());
		}
		//std::cout << glm::to_string(-camera.getPos()) << std::endl;
		terrain.set_direction(camera.getDirection());
		terrain.render(&objectShader, frameStartTime, dayNightSpeed);

		///////////////////////////////////////////////////////////////////////

		// Display

		should_close = opengl.update();

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

	return 0;
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

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	bool printNewline = true;
	bool printLastNewline = false;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	default:                              printNewline = false; break;
	}

	if (printNewline) {
		std::cout << std::endl;
		printLastNewline = true;
	}
	printNewline = true;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	default:                              printNewline = false; break;
	}

	if (printNewline) {
		std::cout << std::endl;
		printLastNewline = true;
	}
	printNewline = true;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	default:                             printNewline = false; break;
	}

	if (printNewline) {
		std::cout << std::endl;
		printLastNewline = true;
	}

	if (printLastNewline) {
		std::cout << std::endl;
	}
}