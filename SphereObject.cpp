#include "SphereObject.h"

SphereObject::SphereObject(std::shared_ptr<BulletPhysicsWorld> physics, btScalar radius, float mass, btVector3 position)
	: PhysicsObjectShape{ std::make_shared<btSphereShape>(radius) },
	PhysicsObject{ physics, mass, shape.get(), position } {}