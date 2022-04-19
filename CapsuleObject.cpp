#include "CapsuleObject.h"

CapsuleObject::CapsuleObject(std::shared_ptr<BulletPhysicsWorld> physics, btScalar radius, btScalar height, float mass, btVector3 position)
	: PhysicsObjectShape{ std::make_shared<btCapsuleShape>(radius, height) },
	PhysicsObject{ physics, mass, shape.get(), position } {}