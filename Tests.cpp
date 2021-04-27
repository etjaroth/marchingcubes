#include "config.h"
#ifndef STB_IMAGE_IMPLEMENTATION // Causes errors if in Config.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "Cube.h"
// Cube test dependency
namespace tables {
	extern std::map<triple<int>, glm::vec3> faceNormals;
}
#include "TerrainGenerator.h"
#include "FPSCamera.h" // Used for opengl too



// If this fails, cube tests fail
// OPENGL INITILIZATION
// Display
unsigned int screenx = 800;
unsigned int screeny = 600;

FPSCamera camera(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, -1.0f));
unsigned int loadTexture(const std::string filename, unsigned int colortype, bool flip);

TEST_CASE("OPENGL INITILIZATION") {
	// Callbacks
	//void framebuffer_size_callback(GLFWwindow * window, int width, int height);
	//void mouse_callback(GLFWwindow * window, double xpos, double ypos);
	glm::vec3 cubeDistance = glm::vec3(30.0f, 6.0f, 30.0f);
	float cubeSize = 1.0f;
	glm::vec3 cubeOrgin(-15.0f, -4.0f, -15.0f);
	std::srand((int)glfwGetTime()); // make random numbers available
	// Setup
		// GLFW Initilisation
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// Window Creation
	GLFWwindow* window = glfwCreateWindow(screenx, screeny, "openGL Tutorial", NULL, NULL); // Make window
	if (window == NULL) // Window failed
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate(); // End program
		//return -1;
	}
	glfwMakeContextCurrent(window);
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Resize the window with framebuffer_size_callback()
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initilize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate(); // End program
		//return -1;
	}

	// Declare window size and GL settings
	glViewport(0, 0, screenx, screeny);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); // Transparencey isn't perfect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

TEST_CASE("Testing class Cube") {
	SECTION("Cube Case 0 (Empty Cube)") {
		triple<int> normalKey = { {0, 0, 0} };
		cube testcubestruct = {
			{0.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{0.0f, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{0.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{0.0f, glm::vec3(1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{0.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{0.0f, glm::vec3(0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])}
		};

		//Cube* testcube = NULL;
		//std::cout << "Empty Cube Start" << std::endl;
		Cube testcube = Cube(testcubestruct, glm::vec3(0.0f), 1.0f);

		REQUIRE(testcube.getCaseIndex() == 0);

		SECTION("Testing that render data is stored correctly") {
			std::vector<Vertex> expectedRenderData;
			SECTION("Testing that Cube is empty") {
				REQUIRE((testcube.isEmpty()));
			}
			SECTION("Testing that renderData is the correct length") {
				REQUIRE((testcube.getRenderData().size() == 0));
				REQUIRE((testcube.getDataLen() == 0));
				REQUIRE((testcube.getRenderData().size() == expectedRenderData.size()));
				REQUIRE((testcube.getDataLen() == expectedRenderData.size()));
			}
			SECTION("Testing that renderData is correct") {
				REQUIRE((testcube.getRenderData() == expectedRenderData));
			}
		}
	}


	SECTION("Cube Case 1") { // {{8, 0, 3}} -> {0, 4} {0, 1} {3, 0} -> {0, 0, 0}/{0, 0, 1} {0, 0, 0}/{1, 0, 0} {0, 1, 0}/{0, 0, 0}
		triple<int> normalKey = { {8, 0, 3} };
		cube testcubestruct = {
			{1.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},//9
			{0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},//0
			{0.0f, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},//6
			{0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},//0
			{0.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},//16
			{0.0f, glm::vec3(1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},//32
			{0.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},//64
			{0.0f, glm::vec3(0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])}//128
		};

		SECTION("Testing constructer input") {
			// See cube constructer
			std::vector<mVertex> corners;
			corners.push_back(testcubestruct.v000);
			corners.push_back(testcubestruct.v100);
			corners.push_back(testcubestruct.v110);
			corners.push_back(testcubestruct.v010);
			corners.push_back(testcubestruct.v001);
			corners.push_back(testcubestruct.v101);
			corners.push_back(testcubestruct.v111);
			corners.push_back(testcubestruct.v011);
			REQUIRE((corners[0].fill > 0 ? 1 : 0) == 1);
			REQUIRE((corners[1].fill > 0 ? 2 : 0) == 0);
			REQUIRE((corners[2].fill > 0 ? 4 : 0) == 0);
			REQUIRE((corners[3].fill > 0 ? 8 : 0) == 0);
			REQUIRE((corners[4].fill > 0 ? 16 : 0) == 0);
			REQUIRE((corners[5].fill > 0 ? 32 : 0) == 0);
			REQUIRE((corners[6].fill > 0 ? 64 : 0) == 0);
			REQUIRE((corners[7].fill > 0 ? 128 : 0) == 0);
		}

		Cube testcube = Cube(testcubestruct, glm::vec3(0.0f), 1.0f);
		REQUIRE(testcube.getCaseIndex() == 1);

		SECTION("Testing that render data is stored correctly") {
			std::vector<Vertex> expectedRenderData;
			expectedRenderData.push_back({ glm::vec3(0.0f, 0.0f, 0.5f), glm::vec4(1.0f),
				glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey]) }); // 8 -> {0, 4}
			expectedRenderData.push_back({ glm::vec3(0.5f, 0.0f, 0.0f), glm::vec4(1.0f),
				glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey]) }); // 0 -> {0, 1}
			expectedRenderData.push_back({ glm::vec3(0.0f, 0.5f, 0.0f), glm::vec4(1.0f),
				glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey]) }); // 3 -> {3, 0}

			SECTION("Testing that Cube is not empty") {
				REQUIRE((testcube.isNotEmpty()));
			}
			SECTION("Testing that renderData is the correct length") {
				REQUIRE((testcube.getRenderData().size() == 3));
				REQUIRE((testcube.getDataLen() == 3));
				REQUIRE((testcube.getRenderData().size() == expectedRenderData.size()));
				REQUIRE((testcube.getDataLen() == expectedRenderData.size()));
			}
			SECTION("Testing that renderData is correct") {
				for (int i = 0; i <= (testcube.getRenderData().size() - 1); i++) {
					std::cout << "renderData Index: " << i << std::endl;
					std::cout << (testcube.getRenderData()[i].TextureCoords == expectedRenderData[i].TextureCoords) << std::endl;
				}
				REQUIRE((testcube.getRenderData() == expectedRenderData));
			}
		}
	}


	SECTION("Cube Case 255 (Second empty cube case)") {
		triple<int> normalKey = { {0, 0, 0} };
		cube testcubestruct = {
			{1.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{1.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{1.0f, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{1.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{1.0f, glm::vec3(1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{1.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])},
			{1.0f, glm::vec3(0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f), (glm::vec3)(tables::faceNormals[normalKey])}
		};

		Cube testcube = Cube(testcubestruct, glm::vec3(0.0f), 1.0f);
		REQUIRE(testcube.getCaseIndex() == 255);

		SECTION("Testing that render data is stored correctly") {
			std::vector<Vertex> expectedRenderData;
			SECTION("Testing that Cube is empty") {
				REQUIRE((testcube.isEmpty()));
			}
			SECTION("Testing that renderData is the correct length") {
				REQUIRE((testcube.getRenderData().size() == 0));
				REQUIRE((testcube.getDataLen() == 0));
				REQUIRE((testcube.getRenderData().size() == expectedRenderData.size()));
				REQUIRE((testcube.getDataLen() == expectedRenderData.size()));
			}
			SECTION("Testing that renderData is correct") {
				REQUIRE((testcube.getRenderData() == expectedRenderData));
			}
		}
	}
}
TEST_CASE("Testing linear interpolation") {
	TerrainGenerator t = TerrainGenerator();
	cube c = t.getCube(glm::vec3(0.0f), 1.0f);
	Cube testcube(c, glm::vec3(0.0f), 1.0f);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f).x == glm::vec3(0.5f, 0.0f, 0.0f).x);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f).y == glm::vec3(0.5f, 0.0f, 0.0f).y);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f).z == glm::vec3(0.5f, 0.0f, 0.0f).z);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f).x == glm::vec3(0.0f, 0.5f, 0.0f).x);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f).y == glm::vec3(0.0f, 0.5f, 0.0f).y);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f).z == glm::vec3(0.0f, 0.5f, 0.0f).z);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), 1.0f).x == glm::vec3(0.0f, 0.0f, 0.5f).x);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), 1.0f).y == glm::vec3(0.0f, 0.0f, 0.5f).y);
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), 1.0f).z == glm::vec3(0.0f, 0.0f, 0.5f).z);

	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(1.0f, 0.0f, 0.0f), 4.0f) == glm::vec3(0.2f, 0.0f, 0.0f));
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f), 4.0f) == glm::vec3(0.0f, 0.2f, 0.0f));
	REQUIRE(testcube.interpolate(glm::vec3(0.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), 4.0f) == glm::vec3(0.0f, 0.0f, 0.2f));
}

TEST_CASE("Testing class TerrainGenerator") { // Tests only guarenteed for seed = 1134
	TerrainGenerator generator = TerrainGenerator(1134);
	glm::vec3 orgin = glm::vec3(0.0f);
	cube spot = generator.getCube(orgin, 1.0f);
	std::cout << "------- Orgin Cube -------" << std::endl;
	std::cout << "Vertex   |   Fill" << std::endl;
	std::cout << "v000     |   " << spot.v000.fill << std::endl;
	std::cout << "v100     |   " << spot.v100.fill << std::endl;
	std::cout << "v110     |   " << spot.v110.fill << std::endl;
	std::cout << "v010     |   " << spot.v010.fill << std::endl;
	std::cout << "v001     |   " << spot.v001.fill << std::endl;
	std::cout << "v101     |   " << spot.v101.fill << std::endl;
	std::cout << "v111     |   " << spot.v111.fill << std::endl;
	std::cout << "v011     |   " << spot.v011.fill << std::endl;

	std::cout << "\n" << std::endl;

	spot = generator.getCube(orgin + glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
	std::cout << "---- Adjacent Cube ----" << std::endl;
	std::cout << "Vertex   |   Fill" << std::endl;
	std::cout << "v000     |   " << spot.v000.fill << std::endl;
	std::cout << "v100     |   " << spot.v100.fill << std::endl;
	std::cout << "v110     |   " << spot.v110.fill << std::endl;
	std::cout << "v010     |   " << spot.v010.fill << std::endl;
	std::cout << "v001     |   " << spot.v001.fill << std::endl;
	std::cout << "v101     |   " << spot.v101.fill << std::endl;
	std::cout << "v111     |   " << spot.v111.fill << std::endl;
	std::cout << "v011     |   " << spot.v011.fill << std::endl;
	//std::cout << "\n----------------\n" << std::endl;

	SECTION("Testing cube - generator interaction") {
		cube genCube = generator.getCube(orgin, 1.0f);
		Cube cCube = Cube(genCube, orgin, 1.0f);
		//REQUIRE(cCube.getCaseIndex() == 64);

	}

}