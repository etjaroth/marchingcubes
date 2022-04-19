#pragma once
#include "AbstractThreadTask.h"
#include "MarchingCubes.h"
#include "TriangleMeshObject.h"

class TriangleMeshGenerator : public AbstractThreadTaskTemplate<std::shared_ptr<TriangleMeshObject>>
{

	std::shared_ptr<BulletPhysicsWorld> physics;
	int cubeSize;
	std::vector<float>& verticies;
	std::vector<unsigned int>& indicies;

	const unsigned int vertexSize = 12;
	btVector3 getPos(unsigned int idx);

public:
	TriangleMeshGenerator(std::shared_ptr<BulletPhysicsWorld> physics, int cubeSize, std::vector<float>& verticies, std::vector<unsigned int>& indicies);
	
	virtual void operator()() override;

};

