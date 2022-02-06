#pragma once
#include "config.h"

// Describes an area in space that uses a specific coordinate system
class CoordinateSystem
{
	glm::mat4 transform;
	glm::mat4 inverse;
	glm::uvec3 limit;

public:
	CoordinateSystem(glm::mat4 transform, glm::uvec3 size);
	
	glm::vec3 convertToSystem(glm::vec3 point);
	glm::vec3 convertFromSystem(glm::vec3 point);

	// All below functions assume that "point" is in this coordinate system.

	bool inSpace(glm::vec3 point);
	glm::uvec3 hash(glm::vec3 point);

	unsigned int pointToIndex(glm::vec3 point);
	unsigned int pointToIndex(glm::uvec3 point);
	glm::uvec3 indexToPoint(unsigned int i);
};

