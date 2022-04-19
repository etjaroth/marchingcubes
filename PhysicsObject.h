#pragma once
#pragma warning(push, 0)
#include "BulletPhysicsWorld.h"
#pragma warning(pop)
#include <memory>
#include <iostream>

// So that shape can be initilized before PhysicsObject
class PhysicsObjectShape {
public:
	std::shared_ptr<btCollisionShape> shape;
	explicit PhysicsObjectShape(std::shared_ptr<btCollisionShape> shape) : shape{ shape } { std::cout << "Created Object Shape" << std::endl; }
	virtual ~PhysicsObjectShape() { std::cout << "Deleted Object Shape" << std::endl; }
};

class PhysicsObject
{
	float maxSpeed = 100.0f;

protected:
	std::shared_ptr<BulletPhysicsWorld> physics;
	//std::shared_ptr<btMotionState> motionState;
	btMotionState* motionState;
	btRigidBody* collisionObj;

public:
	PhysicsObject(std::shared_ptr<BulletPhysicsWorld> physics, float mass, btCollisionShape* shape, btVector3 position);
	virtual ~PhysicsObject();

	virtual void push(btVector3 force);
	virtual void turn(btVector3 force);
	void setMaxSpeed(float max);
};

