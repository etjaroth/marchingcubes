#include "FPSCamera.h"

FPSCamera::FPSCamera(glm::vec3 startingPos, glm::vec3 dir, float speed) {
	rotationVec = glm::vec3(0.0f, 0.0f, 0.0f);

	cameraPos = startingPos;
	if (dir == glm::vec3(0.0f, 0.0f, 0.0f)) {
		std::cout << "Camera can't not have a direction!" << std::endl;
		dir = glm::vec3(0.0f, 0.0f, -1.0f);
	}
	originalCameraDirection = glm::vec4(dir.x, dir.y, dir.z, 1.0f);
	cameraDirection = dir;
		
	cameraSpeed = speed;
	cameraTurnSpeed = speed / 10;

	recalculateFPSView();
}

void FPSCamera::recalculateFPSView() {
	view = glm::mat4(1.0f);
	if (rotationVec != glm::vec3(0.0f, 0.0f, 0.0f)) {
		glm::vec4 temp = originalCameraDirection * glm::rotate(glm::mat4(1.0f), glm::length(rotationVec), rotationVec);
		cameraDirection = glm::vec3(temp.x, temp.y, temp.z);

		view = getCameraRotationMat();
	}
		
	view = glm::translate(view, cameraPos);

	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
}

// Movement

void FPSCamera::pan(glm::vec3 panvec) {
	//panvec = glm::normalize(panvec);
	glm::vec3 fdir = glm::normalize(glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
	glm::vec3 moveVec =
		(panvec.x * cameraRight * cameraSpeed) +
		(panvec.y * up * cameraSpeed) +
		(panvec.z * fdir * cameraSpeed);
	cameraPos += moveVec;
	recalculateFPSView();
}

void FPSCamera::setPos(glm::vec3 newpos) {
	cameraPos = newpos;
}

void FPSCamera::rotate(float pitch, float yaw, float roll) {
	rotationVec.x += pitch * cameraTurnSpeed;
	rotationVec.y += yaw * cameraTurnSpeed;
	rotationVec.z += roll * cameraTurnSpeed;
	if (rotationVec.x > glm::radians(89.9f))
		rotationVec.x = glm::radians(89.9f);
	if (rotationVec.x < glm::radians(-89.9f))
		rotationVec.x = glm::radians(-89.9f);

	recalculateFPSView();

}

void FPSCamera::setRotate(float pitch, float yaw, float roll) {
	cameraDirection = glm::vec3(pitch, yaw, roll);
	recalculateFPSView();
}

glm::mat4 FPSCamera::getCameraRotationMat() const {
	glm::mat4 m(1.0f);

	m = glm::rotate(m, rotationVec.x, glm::vec3(1.0f, 0.0f, 0.0f));
	m = glm::rotate(m, rotationVec.y, glm::vec3(0.0f, 1.0f, 0.0f));
	m = glm::rotate(m, rotationVec.z, glm::vec3(0.0f, 0.0f, 1.0f));

	return m;
}

// Misc.
glm::mat4 FPSCamera::getView() {
	return view;
};

void FPSCamera::setViewLoc(int loc) {
	viewMatLoc = loc;
};

int FPSCamera::getViewLoc() {
	return viewMatLoc;
};

glm::vec3 FPSCamera::getPos() {
	return cameraPos;
};

glm::vec3 FPSCamera::getDirection() {
	return cameraDirection;
}