#include "Settings.h"

GLenum Settings::convertToGLEnum(std::string str) {
	if (str == "GLFW_KEY_ESCAPE") {
		return GLFW_KEY_ESCAPE;
	}
	else if (str == "GLFW_KEY_0") {
		return GLFW_KEY_0;
	}
	else if (str == "GLFW_KEY_1") {
		return GLFW_KEY_1;
	}
	else if (str == "GLFW_KEY_2") {
		return GLFW_KEY_2;
	}
	else if (str == "GLFW_KEY_3") {
		return GLFW_KEY_3;
	}
	else if (str == "GLFW_KEY_4") {
		return GLFW_KEY_4;
	}
	else if (str == "GLFW_KEY_5") {
		return GLFW_KEY_5;
	}
	else if (str == "GLFW_KEY_6") {
		return GLFW_KEY_6;
	}
	else if (str == "GLFW_KEY_7") {
		return GLFW_KEY_7;
	}
	else if (str == "GLFW_KEY_8") {
		return GLFW_KEY_8;
	}
	else if (str == "GLFW_KEY_9") {
		return GLFW_KEY_9;
	}
	else if (str == "GLFW_KEY_A") {
		return GLFW_KEY_A;
	}
	else if (str == "GLFW_KEY_B") {
		return GLFW_KEY_B;
	}
	else if (str == "GLFW_KEY_C") {
		return GLFW_KEY_C;
	}
	else if (str == "GLFW_KEY_D") {
		return GLFW_KEY_D;
	}
	else if (str == "GLFW_KEY_E") {
		return GLFW_KEY_E;
	}
	else if (str == "GLFW_KEY_F") {
		return GLFW_KEY_F;
	}
	else if (str == "GLFW_KEY_G") {
		return GLFW_KEY_G;
	}
	else if (str == "GLFW_KEY_H") {
		return GLFW_KEY_H;
	}
	else if (str == "GLFW_KEY_I") {
		return GLFW_KEY_I;
	}
	else if (str == "GLFW_KEY_J") {
		return GLFW_KEY_J;
	}
	else if (str == "GLFW_KEY_K") {
		return GLFW_KEY_K;
	}
	else if (str == "GLFW_KEY_L") {
		return GLFW_KEY_L;
	}
	else if (str == "GLFW_KEY_M") {
		return GLFW_KEY_M;
	}
	else if (str == "GLFW_KEY_N") {
		return GLFW_KEY_N;
	}
	else if (str == "GLFW_KEY_O") {
		return GLFW_KEY_O;
	}
	else if (str == "GLFW_KEY_P") {
		return GLFW_KEY_P;
	}
	else if (str == "GLFW_KEY_Q") {
		return GLFW_KEY_Q;
	}
	else if (str == "GLFW_KEY_R") {
		return GLFW_KEY_R;
	}
	else if (str == "GLFW_KEY_S") {
		return GLFW_KEY_S;
	}
	else if (str == "GLFW_KEY_T") {
		return GLFW_KEY_T;
	}
	else if (str == "GLFW_KEY_U") {
		return GLFW_KEY_U;
	}
	else if (str == "GLFW_KEY_V") {
		return GLFW_KEY_V;
	}
	else if (str == "GLFW_KEY_W") {
		return GLFW_KEY_W;
	}
	else if (str == "GLFW_KEY_X") {
		return GLFW_KEY_X;
	}
	else if (str == "GLFW_KEY_Y") {
		return GLFW_KEY_Y;
	}
	else if (str == "GLFW_KEY_Z") {
		return GLFW_KEY_Z;
	}
	else if (str == "GLFW_KEY_SPACE") {
		return GLFW_KEY_SPACE;
	}
	else if (str == "GLFW_KEY_ESCAPE") {
		return GLFW_KEY_ESCAPE;
	}
	else {
		return GL_INVALID_ENUM;
	}
}

Settings::SettingAction Settings::convertToSettingAction(std::string str) {
	if (str == "screenSize") {
		return SettingAction::screenSize;
	}
	else if (str == "cameraSpeed") {
		return SettingAction::cameraSpeed;
	}
	else if (str == "spawnpoint") {
		return SettingAction::spawnpoint;
	}
	else if (str == "spawnViewDir") {
		return SettingAction::spawnViewDir;
	}
	else if (str == "chunkSize") {
		return SettingAction::chunkSize;
	}
	else if (str == "chunkRadius") {
		return SettingAction::chunkRadius;
	}
	else if (str == "controlConfigFile") {
		return SettingAction::controlConfigFile;
	}
	else if (str == "farClipPlain") {
		return SettingAction::farClipPlain;
	}

	else {
		return SettingAction::INVALID_ACTION;
	}
}

Settings::ControlAction Settings::convertToControlAction(std::string str) {
	if (str == "MouseRotatesCamera") {
		return ControlAction::MouseRotatesCamera;
	}
	else {
		return ControlAction::INVALID_ACTION;
	}
}

Settings::WorldAction Settings::convertToWorldAction(std::string str) {
	if (str == "quit") {
		return WorldAction::quit;
	}
	else if (str == "resetPos") {
		return WorldAction::resetPos;
	}
	else if (str == "resetViewDir") {
		return WorldAction::resetViewDir;
	}
	else if (str == "toggleMeshView") {
		return WorldAction::toggleMeshView;
	}
	else if (str == "toggleTerrainUpdates") {
		return WorldAction::toggleTerrainUpdates;
	}
	else if (str == "moveNorth") {
		return WorldAction::moveNorth;
	}
	else if (str == "moveWest") {
		return WorldAction::moveWest;
	}
	else if (str == "moveSouth") {
		return WorldAction::moveSouth;
	}
	else if (str == "moveEast") {
		return WorldAction::moveEast;
	}
	else if (str == "moveUp") {
		return WorldAction::moveUp;
	}
	else if (str == "moveDown") {
		return WorldAction::moveDown;
	}
	else if (str == "rotateCameraNorth") {
		return WorldAction::rotateCameraNorth;
	}
	else if (str == "rotateCameraWest") {
		return WorldAction::rotateCameraWest;
	}
	else if (str == "rotateCameraSouth") {
		return WorldAction::rotateCameraSouth;
	}
	else if (str == "rotateCameraEast") {
		return WorldAction::rotateCameraEast;
	}
	else {
		return WorldAction::INVALID_ACTION;
	}
}

Settings::Settings(std::string filename) {
	std::fstream file{ filename };

	std::string line;
	while (getline(file, line)) {
		if (line == "") { continue; }
		std::stringstream linestream{ line };

		std::string action;
		linestream >> action;
		SettingAction actionEnum = convertToSettingAction(action);
		GLenum GLactionEnum = convertToGLEnum(action);

		if (actionEnum != SettingAction::INVALID_ACTION) { // Apply Setting
			switch (actionEnum) {
			case SettingAction::screenSize:
				linestream >> constants.screenSize.x;
				linestream >> constants.screenSize.y;
				break;
			case SettingAction::cameraSpeed:
				linestream >> constants.cameraSpeed;
				break;
			case SettingAction::spawnpoint:
				linestream >> constants.spawnpoint.x;
				linestream >> constants.spawnpoint.y;
				linestream >> constants.spawnpoint.z;
				break;
			case SettingAction::spawnViewDir:
				linestream >> constants.spawnViewDir.x;
				linestream >> constants.spawnViewDir.y;
				linestream >> constants.spawnViewDir.z;
				break;
			case SettingAction::chunkSize:
				linestream >> constants.chunkSize;
				break;
			case SettingAction::chunkRadius:
				linestream >> constants.chunkRadius;
				break;
			case SettingAction::controlConfigFile:
				linestream >> constants.controlConfigFile;
				break;
			case SettingAction::farClipPlain:
				linestream >> constants.farClipPlain;
				break;
			}
		}
	}

	file.close();
}

void Settings::loadControls(GLFWwindow* window) {
	std::fstream file{ constants.controlConfigFile };

	std::string line;
	while (getline(file, line)) {
		if (line == "") { continue; }
		std::stringstream linestream{ line };

		std::string action;
		linestream >> action;
		GLenum GLactionEnum = convertToGLEnum(action);
		ControlAction actionEnum = convertToControlAction(action);

		if (GLactionEnum != GL_INVALID_ENUM) { // Apply Setting
			linestream >> action;
			GLenum pressRelease = (action == "press") ? GLFW_PRESS : GLFW_RELEASE;

			linestream >> action;
			WorldAction actionEnum = convertToWorldAction(action);

			linestream >> action;
			if (action == "slow") {
				keyLocks.insert({GLactionEnum, false});
			}

			bindings.insert({ GLactionEnum , {pressRelease, actionEnum} });
		}
		else if (actionEnum != ControlAction::INVALID_ACTION) { // Connect action to button press/relese
			std::string boolStr;
			linestream >> boolStr;
			bool b = (boolStr == "true");

			switch (actionEnum) {
			case ControlAction::MouseRotatesCamera:
				if (b) {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				break;
			default:
				break;
			};
		}
	}

	file.close();
}

const Settings::Constants& Settings::getConstants() const {
	return constants;
}

void Settings::setScreenSize(glm::uvec2 size) {
	constants.screenSize = size;
}

const Settings::InputActions& Settings::getActions() const {
	return inputActions;
}

void Settings::readInput(GLFWwindow* window) {
	InputActions newInputActions;

	for (auto itr = bindings.begin(); itr != bindings.end(); ++itr) {
		const GLenum key = itr->first;
		const GLenum pressRelease = itr->second.first;
		const WorldAction action = itr->second.second;

		const auto keyLock = keyLocks.find(key);
		bool hasLock = (keyLock != keyLocks.end());
		bool isLocked = hasLock && keyLock->second;

		const GLenum keyPress = glfwGetKey(window, key);
		if (!isLocked && keyPress == pressRelease) {
			if (hasLock) {
				keyLock->second = true;
			}

			switch (action) {
			case WorldAction::quit:
				newInputActions.quit = true;
				break;
			case WorldAction::resetPos:
				newInputActions.resetPos = true;
				break;
			case WorldAction::resetViewDir:
				newInputActions.resetViewDir = true;
				break;
			case WorldAction::toggleMeshView:
				newInputActions.toggleMeshView = true;
				break;
			case WorldAction::toggleTerrainUpdates:
				newInputActions.toggleTerrainUpdates = true;
				break;
			case WorldAction::moveNorth:
				newInputActions.moveNorth = true;
				break;
			case WorldAction::moveWest:
				newInputActions.moveWest = true;
				break;
			case WorldAction::moveSouth:
				newInputActions.moveSouth = true;
				break;
			case WorldAction::moveEast:
				newInputActions.moveEast = true;
				break;
			case WorldAction::moveUp:
				newInputActions.moveUp = true;
				break;
			case WorldAction::moveDown:
				newInputActions.moveDown = true;
				break;
			case WorldAction::rotateCameraNorth:
				newInputActions.rotateCameraNorth = true;
				break;
			case WorldAction::rotateCameraWest:
				newInputActions.rotateCameraWest = true;
				break;
			case WorldAction::rotateCameraSouth:
				newInputActions.rotateCameraSouth = true;
				break;
			case WorldAction::rotateCameraEast:
				newInputActions.rotateCameraEast = true;
				break;
			case WorldAction::INVALID_ACTION:
				break;
			}
		}
		else if (isLocked && keyPress != pressRelease) {
			keyLock->second = false;
		}
	}

	inputActions = newInputActions;
}