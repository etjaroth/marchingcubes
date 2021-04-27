#pragma once
#include "Config.h"
class FlyCamera
{
public:
	//private:
	glm::vec3 cameraDirection;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;

	glm::mat4 view;
	int viewMatLoc;

	// Fps movement
	glm::vec3 cameraPos;

	// Movement
	float radius;
	float pivotSpeed;
	glm::vec3 cameraTarget;

	// General
	float cameraSpeed;
	void recalculateView();

	//public:
	FlyCamera(glm::vec3 startingPos, glm::vec3 targetPos);

	// Fly movement
	void move(glm::vec3 panvec);
	void pivot(glm::vec3 pivotvec);
	void setPivot(float xRotate, float yRotate, float radius);
	void setTargetPoint(glm::vec3 point);

	// Shader
	glm::mat4 getView();
	void setViewLoc(int loc);
	int getViewLoc();
};

