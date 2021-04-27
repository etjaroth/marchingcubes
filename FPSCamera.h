#pragma once
#include "Config.h"
class FPSCamera
{
//public:
private:
	glm::vec3 cameraDirection;
	glm::vec4 originalCameraDirection;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 view;
	int viewMatLoc;

	// Fps movement
	glm::vec3 cameraPos;
	glm::vec3 rotationVec;

	// General
	float cameraSpeed;
	float cameraTurnSpeed;
	void recalculateFPSView();

public:
	FPSCamera(glm::vec3 startingPos, glm::vec3 targetPos, float speed);
	FPSCamera(glm::vec3 startingPos, glm::vec3 targetPos) : FPSCamera(startingPos, targetPos, 5.0f) {};

	// Fps movement

	void pan(glm::vec3 panvec);
	void setPos(glm::vec3 newpos);
	void rotate(float pitch, float yaw, float roll);
	void setRotate(float pitch, float yaw, float roll);

	// Shader
	glm::mat4 getView();
	void setViewLoc(int loc);
	int getViewLoc();

	// Misc
	glm::vec3 getPos();
};

