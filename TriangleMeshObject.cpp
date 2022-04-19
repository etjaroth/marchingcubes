#include "TriangleMeshObject.h"

btTriangleMesh* TriangleMeshObject::genPhysicsMesh(std::vector<TriangleMeshObject::Triangle>& data) {

	physicsMesh = std::make_shared<btTriangleMesh>();

	for (Triangle& tri : data) {
		physicsMesh->addTriangle(tri.v1, tri.v2, tri.v3);
	}

	return physicsMesh.get();

}

TriangleMeshObject::TriangleMeshObject(std::shared_ptr<BulletPhysicsWorld> physics, float mass, btVector3 position, std::vector<Triangle>& data)
	: PhysicsObjectShape{ std::make_shared<btBvhTriangleMeshShape>(genPhysicsMesh(data), true) },
	PhysicsObject{ physics, mass, shape.get(), position } {}

TriangleMeshObject::~TriangleMeshObject() {}