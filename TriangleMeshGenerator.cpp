#include "TriangleMeshGenerator.h"

btVector3 TriangleMeshGenerator::getPos(unsigned int idx) {

	const unsigned int trueIdx = vertexSize * idx;
	return btVector3(verticies[trueIdx + 0], verticies[trueIdx + 1], verticies[trueIdx + 2]);

}

TriangleMeshGenerator::TriangleMeshGenerator(std::shared_ptr<BulletPhysicsWorld> physics, int cubeSize, std::vector<float>& verticies, std::vector<unsigned int>& indicies)
	: physics{physics},
	cubeSize{cubeSize},
	verticies{ verticies },
	indicies{ indicies } {}

void TriangleMeshGenerator::operator()() {

	std::vector<TriangleMeshObject::Triangle> vec;
	for (unsigned int i = 0; i < indicies.size();) {
		TriangleMeshObject::Triangle triangle;
		triangle.v1 = btVector3(getPos(indicies[i++]));
		triangle.v2 = btVector3(getPos(indicies[i++]));
		triangle.v3 = btVector3(getPos(indicies[i++]));
	}

	data = std::make_shared<TriangleMeshObject>(
		physics, 
		0.0f, 
		btVector3(0.0f, 0.0f, 0.0f),
		vec
		);

}