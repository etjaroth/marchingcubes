#pragma once
#include "config.h"
class Settings
{
	enum class SettingAction {
		screenSize,
		cameraSpeed,
		spawnpoint,
		spawnViewDir,
		chunkSize,
		chunkRadius,
		controlConfigFile,
		freeKey,
		farClipPlain,

		INVALID_ACTION
	};

	enum class ControlAction {
		MouseRotatesCamera,
		INVALID_ACTION
	};

	enum class WorldAction {
		quit,
		resetPos,
		resetViewDir,
		toggleMeshView,
		toggleTerrainUpdates,
		moveNorth,
		moveWest,
		moveSouth,
		moveEast,
		moveUp,
		moveDown,
		rotateCameraNorth,
		rotateCameraWest,
		rotateCameraSouth,
		rotateCameraEast,

		INVALID_ACTION
	};

	struct InputActions {
		bool quit = false;
		bool resetPos = false;
		bool resetViewDir = false;
		bool toggleMeshView = false;
		bool toggleTerrainUpdates = false;
		bool moveNorth = false;
		bool moveWest = false;
		bool moveSouth = false;
		bool moveEast = false;
		bool moveUp = false;
		bool moveDown = false;
		bool rotateCameraNorth = false;
		bool rotateCameraWest = false;
		bool rotateCameraSouth = false;
		bool rotateCameraEast = false;
	} inputActions;

	struct Constants {
		glm::uvec2 screenSize = glm::uvec2(100);
		float cameraSpeed = 0.0f;

		glm::vec3 spawnpoint = glm::vec3(0.0f);
		glm::vec3 spawnViewDir = glm::vec3(0.0, 0.0, -1.0);

		unsigned int chunkSize = 0;
		unsigned int chunkRadius = 0;

		float farClipPlain = 100.0f;

		std::string controlConfigFile;

	} constants;


	std::map<GLenum, std::pair<GLenum, WorldAction>> bindings;
	std::map<GLenum, bool> openGLSettings;
	std::map<GLenum, bool> keyLocks;

	GLenum convertToGLEnum(std::string str);
	SettingAction convertToSettingAction(std::string str);
	ControlAction convertToControlAction(std::string str);
	WorldAction convertToWorldAction(std::string str);

public:
	Settings(std::string filename="settings.txt");
	void loadControls(GLFWwindow* window);

	const Constants& getConstants() const;
	void setScreenSize(glm::uvec2 size);

	void readInput(GLFWwindow* window);
	const InputActions& getActions() const;
};