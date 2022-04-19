#pragma once
#include "PhysicsObject.h"
class CapsuleObject :
    public PhysicsObjectShape, public PhysicsObject
{
public:
    CapsuleObject(std::shared_ptr<BulletPhysicsWorld> physics, btScalar radius, btScalar height, float mass, btVector3 position);
};
