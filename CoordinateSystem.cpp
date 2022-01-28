#include "CoordinateSystem.h"

CoordinateSystem::CoordinateSystem(glm::mat4 transform, glm::uvec3 limit)
	: transform{transform},
	inverse{glm::inverse(transform)},
	limit{ limit }
{}

glm::vec3 CoordinateSystem::convertToSystem(glm::vec3 point) {
	return transform * glm::vec4(point, 1.0f);
}

glm::vec3 CoordinateSystem::convertFromSystem(glm::vec3 point) {
	return inverse * glm::vec4(point, 1.0f);
}

// All below functions assume that "point" is in this coordinate system.

bool CoordinateSystem::inSpace(glm::vec3 point) {
	return (point.x >= 0 && point.y >= 0 && point.z >= 0) &&
		(point.x <= limit.x && point.y <= limit.y && point.z <= limit.z);
}

glm::uvec3 CoordinateSystem::hash(glm::vec3 point) {
	assert(inSpace(point));
	return glm::uvec3(point / glm::vec3(limit));
}


unsigned int CoordinateSystem::pointToIndex(glm::vec3 point) {
	assert(inSpace(point));
	return pointToIndex(hash(point));
}

unsigned int CoordinateSystem::pointToIndex(glm::uvec3 point) {
	// see https://stackoverflow.com/questions/7367770/how-to-flatten-or-index-3d-array-in-1d-array
	assert(inSpace(point));
	const glm::uvec3 size = limit + glm::uvec3{ 1 };
	return point.x + (point.y * size.x) + (point.z * size.x * size.y);
}

glm::uvec3 CoordinateSystem::indexToPoint(unsigned int i) {
	// see https://stackoverflow.com/questions/7367770/how-to-flatten-or-index-3d-array-in-1d-array
	const glm::uvec3 size = limit + glm::uvec1{ 1 };
	assert(i <= (size.x * size.y * size.z));
	
	glm::uvec3 point;
	
	point.z = i / (size.x * size.y);
	i -= point.z * size.x * size.y;

	point.y = i / size.x;
	point.x = i % size.x;

	return point;
}