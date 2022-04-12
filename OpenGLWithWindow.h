#pragma once
#include "Config.h"
#include "FPSCamera.h"
#include "Settings.h"

class OpenGLWithWindow
{
	GLFWwindow* window = nullptr;
	FPSCamera& camera;
	Settings& settings;

	double lastmousex;
	double lastmousey;
	bool firstmousecallback = true;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	

public:
	explicit OpenGLWithWindow(FPSCamera& camera, Settings& settings);
	~OpenGLWithWindow();

	bool update();

	bool shouldClose() const;
	void close() const;
	void readInputToSettings();
};

