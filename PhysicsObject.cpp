#include "PhysicsObject.h"
#include <iostream>

PhysicsObject::PhysicsObject(std::shared_ptr<BulletPhysicsWorld> physics, float mass, btCollisionShape* shape, btVector3 position)
	: physics{ physics } {
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -56, 0));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic) {
		shape->calculateLocalInertia(mass, localInertia);
	}

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	//motionState = std::make_shared<btDefaultMotionState>(groundTransform);
	motionState = new btDefaultMotionState(groundTransform);

	//btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), shape, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	collisionObj = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	physics->addRigidBody(collisionObj);
}

PhysicsObject::~PhysicsObject() {
	delete motionState;
	physics->getWorld()->removeCollisionObject(collisionObj);
	delete collisionObj;
	std::cout << "Deleted Physics Object" << std::endl;
}

void PhysicsObject::push(btVector3 force) {
	collisionObj->applyCentralForce(force);
}

void PhysicsObject::turn(btVector3 force) {
	collisionObj->applyTorque(force);
}

void PhysicsObject::setMaxSpeed(float max) {
	maxSpeed = max;
}