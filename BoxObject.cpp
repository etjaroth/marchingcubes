#include "BoxObject.h"

BoxObject::BoxObject(std::shared_ptr<BulletPhysicsWorld> physics, btVector3 size, float mass, btVector3 position)
	: PhysicsObjectShape{ std::make_shared<btBoxShape>(size) },
	PhysicsObject{ physics, mass, shape.get(), position } {}