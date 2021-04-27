#include "FlyCamera.h"
#include <glfw3.h>
#include <iostream>

FlyCamera::FlyCamera(glm::vec3 startingPos, glm::vec3 targetPos) {
	cameraPos = startingPos;
	cameraTarget = targetPos;
	cameraSpeed = 1.0f;
	pivotSpeed = 1.0f;

	recalculateView();
}

// Get/Set
glm::mat4 FlyCamera::getView() { return view; };
void FlyCamera::setViewLoc(int loc) { viewMatLoc = loc; };
int FlyCamera::getViewLoc() { return viewMatLoc; };

// Own Catagory
void FlyCamera::recalculateView() {
	view = glm::mat4(1.0f);
	if (cameraPos == cameraTarget) {
		cameraDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	else {
		cameraDirection = glm::normalize(cameraPos - cameraTarget);
	}
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
	radius = glm::length(cameraPos - cameraTarget); // remove later?

	view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
}

// Transformations
void FlyCamera::move(glm::vec3 panvec) {
	//panvec = glm::normalize(panvec);
	glm::vec3 moveVec =
		(panvec.x * cameraRight * cameraSpeed) +
		(panvec.y * cameraUp * cameraSpeed) +
		(panvec.z * cameraDirection * cameraSpeed);
	cameraPos += moveVec;
	cameraTarget += moveVec;

	recalculateView();
}
void FlyCamera::pivot(glm::vec3 pivotvec) {
	glm::vec3 moveVec =
		(pivotvec.x * cameraRight * pivotSpeed) +
		(pivotvec.y * cameraUp * pivotSpeed) +
		(pivotvec.z * cameraDirection * pivotSpeed);
	cameraPos += moveVec;
	recalculateView();
};

void FlyCamera::setPivot(float xRotate, float yRotate, float radius) {};

void FlyCamera::setTargetPoint(glm::vec3 newpoint) {
	cameraTarget = newpoint;
};