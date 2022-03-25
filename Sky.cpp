#include "Sky.h"

glm::mat3 rotateAlign(glm::vec3 v1, glm::vec3 v2)
{
	glm::vec3 axis = glm::cross(v1, v2);

	const float cosA = glm::dot(v1, v2);
	const float k = 1.0f / (1.0f + cosA);

	glm::mat3 result((axis.x * axis.x * k) + cosA,
		(axis.y * axis.x * k) - axis.z,
		(axis.z * axis.x * k) + axis.y,
		(axis.x * axis.y * k) + axis.z,
		(axis.y * axis.y * k) + cosA,
		(axis.z * axis.y * k) - axis.x,
		(axis.x * axis.z * k) - axis.y,
		(axis.y * axis.z * k) + axis.x,
		(axis.z * axis.z * k) + cosA
	);

	return result;
}

Sky::Sky(glm::uvec2 resolution)
	: skyTexture{ resolution },
	skyGenerator{ "SkyGenerator.comp", resolution.x, resolution.y, 1 },
	resolution{ resolution } {

	//skyTexture.setWrap(GL_CLAMP_TO_EDGE);
	skyTexture.setWrap(GL_REPEAT);
	skyTexture.setFilters(GL_LINEAR);

	skyGenerator.use();
	const float earthRadius = 6371000.0f;
	const float atmosphereRadius = 400000.0f;
	//const float earthRadius = 637100.0f;
	//const float atmosphereRadius = 40000.0f;
	skyGenerator.setFloat("atmosphereRadius", earthRadius + atmosphereRadius);
	skyGenerator.setFloat("earthRadius", earthRadius);
	skyGenerator.setFloat("sunDistanceRatio", 8.0f);

	skyGenerator.setUvec2("skyTextureSize", resolution);

	//skyGenerator.setVec3("incidentLight", glm::vec3(0.9922f, 0.9843f, 0.8275f));
	const glm::vec3 lambda = glm::vec3(700.0f, 530.0f, 440.0f);
	skyGenerator.setVec3("RAY_BETA", glm::vec3(5.5e-6, 13.0e-6, 22.4e-6));
	skyGenerator.setVec3("MIE_BETA", glm::vec3(21e-6));
	skyGenerator.setVec3("ABSORPTION_BETA", glm::vec3(2.04e-5, 4.97e-5, 1.95e-6));
	skyGenerator.setFloat("ABSORPTION_FALLOFF", 4000);

	skyGenerator.setFloat("G", 0.7);
	// and the heights (how far to go up before the scattering has no effect)
	skyGenerator.setFloat("HEIGHT_RAY", 8000.0);
	skyGenerator.setFloat("HEIGHT_MIE", 1200.0);
	skyGenerator.setFloat("HEIGHT_ABSORPTION", 30000.0);
	skyGenerator.dontuse();

}

void Sky::generateSky(FPSCamera& camera, double time) {
	skyGenerator.use();
	skyTexture.use();

	GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	time = 1.0 * time;
	skyGenerator.setFloat("time", time);

	glm::vec3 rotationVec = camera.getCameraRotationVec();

	glm::mat4 m(1.0f);
	m = glm::rotate(m, rotationVec.x, glm::vec3(1.0f, 0.0f, 0.0f));
	m = glm::rotate(m, rotationVec.y, glm::vec3(0.0f, 1.0f, 0.0f));
	m = glm::rotate(m, rotationVec.z, glm::vec3(0.0f, 0.0f, 1.0f));

	m = glm::translate(m, glm::vec3(0.0f, 0.26, 0.0f)); // 0.26 is from #define ELEVATION_CONSTANT 0.26

	skyGenerator.setMat4("viewMatInverse", glm::inverse(m));
	
	skyGenerator.fillTexture();
	
	// Wait until texture is filled
	GLint syncStatus[1] = { GL_UNSIGNALED };
	while (syncStatus[0] != GL_SIGNALED) {
		glGetSynciv(fence, GL_SYNC_STATUS, sizeof(GLint), NULL, syncStatus);
	}
	glDeleteSync(fence);

	skyTexture.dontuse();
	skyGenerator.dontuse();
}

void Sky::render(FPSCamera& camera, double time) {
	box.render(skyTexture, camera, 2.0 * 0.25 * time);
}