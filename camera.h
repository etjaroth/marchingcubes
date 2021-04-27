#pragma once
#include "Config.h"
class Camera
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
	glm::vec3 direction;
	float theta;

	// Fly movement
	float radius;
	float pivotSpeed;
	glm::vec3 cameraTarget;

	// General
	float cameraSpeed;
	void recalculateFlyView();
	void recalculateFpsView();

//public:
	Camera(glm::vec3 startingPos, glm::vec3 targetPos);

	// Fps movement
	
	void pan(glm::vec3 panvec);
	void setPos(glm::vec3 newpos);
	void rotate(float pitch, float yaw, float roll);
	void setRotate(float pitch, float yaw, float roll);
	
	// Fly movement
	void pivot(glm::vec3 pivotvec);
	void setPivot(float xRotate, float yRotate, float radius);
	void setTargetPoint(glm::vec3 point);
	
	// Shader
	glm::mat4 getView();
	void setViewLoc(int loc);
	int getViewLoc();
};

