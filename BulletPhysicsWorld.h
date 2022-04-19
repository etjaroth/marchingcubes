#pragma once
#include "btBulletDynamicsCommon.h"

class BulletPhysicsWorld
{
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

public:
	BulletPhysicsWorld();
	BulletPhysicsWorld(BulletPhysicsWorld& world);
	BulletPhysicsWorld& operator=(BulletPhysicsWorld& world);
	BulletPhysicsWorld(BulletPhysicsWorld&& world) noexcept;
	BulletPhysicsWorld& operator=(BulletPhysicsWorld&& world) noexcept;
	~BulletPhysicsWorld();

	void update(double deltatime);

	btDiscreteDynamicsWorld* getWorld();

	void addRigidBody(btRigidBody* body);
	void removeRigidBody(btRigidBody* body);
	void setGravity(float strength);
};

