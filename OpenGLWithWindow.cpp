#include "OpenGLWithWindow.h"

OpenGLWithWindow::OpenGLWithWindow(FPSCamera& camera, Settings& settings)
	: camera{ camera },
	settings{ settings },
	lastmousex{ settings.getConstants().screenSize.x / 2.0 },
	lastmousey{ settings.getConstants().screenSize.y / 2.0 } {

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
	window =
		glfwCreateWindow(settings.getConstants().screenSize.x,
			settings.getConstants().screenSize.y, "Marching Cubes Demo", NULL, NULL); // Make window
	settings.loadControls(window);

	if (window == NULL) // Window failed
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate(); // End program
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	glfwSetWindowUserPointer(window, this);

	auto framebuffer_size_callback_func = [](GLFWwindow* w, int a, int b)
	{
		static_cast<OpenGLWithWindow*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback(w, a, b);
	};

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_func); // Resize the window with framebuffer_size_callback()

	auto mouse_callback_func = [](GLFWwindow* w, double a, double b)
	{
		static_cast<OpenGLWithWindow*>(glfwGetWindowUserPointer(w))->mouse_callback(w, a, b);
	};

	glfwSetCursorPosCallback(window, mouse_callback_func);

	// Debug Output
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	// Initilize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate(); // End program
		exit(-1);
	}

	// Declare window size and GL settings
	glViewport(0, 0, settings.getConstants().screenSize.x, settings.getConstants().screenSize.y);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);

	if (settings.getConstants().cullFace) {
		glEnable(GL_CULL_FACE);
	}
	glEnable(GL_DEBUG_OUTPUT);
}

OpenGLWithWindow::~OpenGLWithWindow() {
	glfwTerminate();
}

///////////////////////////////////////////////////////////////////

bool OpenGLWithWindow::update() {
	glfwSwapBuffers(window);
	glfwPollEvents(); // Calls functions from inputs
	return glfwWindowShouldClose(window);
}

void OpenGLWithWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height) { // Resizes window when needed
	settings.setScreenSize(glm::uvec2(width, height));
	glViewport(0, 0, width, height);
}

void OpenGLWithWindow::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

////////////////////////////////////////////////////////

bool OpenGLWithWindow::shouldClose() const {
	return glfwWindowShouldClose(window);
}

void OpenGLWithWindow::close() const {
	glfwSetWindowShouldClose(window, true);
}

void OpenGLWithWindow::readInputToSettings() {
	settings.readInput(window);
}