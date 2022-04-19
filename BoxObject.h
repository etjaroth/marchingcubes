#pragma once
#include "PhysicsObject.h"
class BoxObject :
    public PhysicsObjectShape, public PhysicsObject
{
public:
    BoxObject(std::shared_ptr<BulletPhysicsWorld> physics, btVector3 size, float mass, btVector3 position);
};

