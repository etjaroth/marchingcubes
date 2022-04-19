#pragma once
#include "PhysicsObject.h"
class SphereObject :
    public PhysicsObjectShape, public PhysicsObject
{
public:
    SphereObject(std::shared_ptr<BulletPhysicsWorld> physics, btScalar radius, float mass, btVector3 position);
};

