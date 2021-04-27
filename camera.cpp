#include "camera.h"
#include <glfw3.h>
#include <iostream>

Camera::Camera(glm::vec3 startingPos, glm::vec3 targetPos) {
	direction = glm::vec3(0.0f, 0.0f, 0.0f);
	theta = 0;

	cameraPos = startingPos;
	cameraTarget = targetPos;
	cameraSpeed = 1.0f;
	pivotSpeed = 1.0f;

	recalculateFlyView();
}

glm::mat4 Camera::getView() {
	return view;
};

void Camera::setViewLoc(int loc) {
	viewMatLoc = loc;
};

int Camera::getViewLoc() {
	return viewMatLoc;
};

void Camera::recalculateFlyView() {
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

void Camera::recalculateFpsView() {
	view = glm::mat4(1.0f);
	//cameraDirection = glm::normalize(cameraDirection); // Remove later?
	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	//cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	//cameraUp = glm::cross(cameraDirection, cameraRight);
	//view = glm::rotate(view, glm::radians(theta), direction);
	view = glm::translate(view, cameraPos);
	//view = glm::rotate(view, glm::radians(theta), direction);
	//view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
}

// Fps movement

void Camera::pan(glm::vec3 panvec) {
	//panvec = glm::normalize(panvec);
	glm::vec3 moveVec = 
		(panvec.x * cameraRight * cameraSpeed) +
		(panvec.y * cameraUp * cameraSpeed) +
		(panvec.z * cameraDirection * cameraSpeed);
	cameraPos += moveVec;
	cameraTarget += moveVec;

	recalculateFlyView();
}

void Camera::setPos(glm::vec3 newpos) {
	cameraPos = newpos;
}

void Camera::rotate(float pitch, float yaw, float roll) {
	direction.x += pitch;
	direction.y += yaw;
	direction.z += roll;
	if (direction.x > 89.9f)
		direction.x = 89.9f;
	if (direction.x < -89.9f)
		direction.x = -89.9f;
	cameraDirection.x += cos(glm::radians(direction.y)) * cos(glm::radians(direction.x));
	cameraDirection.y += sin(glm::radians(direction.x));
	cameraDirection.z += sin(glm::radians(direction.y)) * cos(glm::radians(direction.x));
	
	cameraDirection = glm::normalize(cameraDirection);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

}

void Camera::setRotate(float pitch, float yaw, float roll) {
	cameraDirection = glm::vec3(pitch, yaw, roll);
	
	//theta = angle;
	//direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	//direction.y = sin(glm::radians(pitch));
	//direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	recalculateFpsView();
}


// Fly movement
void Camera::pivot(glm::vec3 pivotvec) {
	glm::vec3 moveVec =
		(pivotvec.x * cameraRight * pivotSpeed) +
		(pivotvec.y * cameraUp * pivotSpeed) +
		(pivotvec.z * cameraDirection * pivotSpeed);
	cameraPos += moveVec;
	recalculateFlyView();
};

void Camera::setPivot(float xRotate, float yRotate, float radius) {};

void Camera::setTargetPoint(glm::vec3 newpoint) {
	cameraTarget = newpoint;
};


/*
void Camera::pointInDirection(float roll, float pitch, float yaw) {
	cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraDirection.y = sin(glm::radians(pitch));
	cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);

	view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
};

void Camera::pointAtPoint() {

};
//*/