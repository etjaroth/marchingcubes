#include "BulletPhysicsWorld.h"

BulletPhysicsWorld::BulletPhysicsWorld()
	: collisionConfiguration{ new btDefaultCollisionConfiguration() },
	dispatcher{ new btCollisionDispatcher(collisionConfiguration) },
	broadphase{ new btDbvtBroadphase() },
	solver{ new btSequentialImpulseConstraintSolver() },
	dynamicsWorld{ new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration) } {}

BulletPhysicsWorld::BulletPhysicsWorld(BulletPhysicsWorld& world)
	: collisionConfiguration{ new btDefaultCollisionConfiguration(*(world.collisionConfiguration)) },
	dispatcher{ new btCollisionDispatcher(collisionConfiguration) },
	broadphase{ new btDbvtBroadphase() },
	solver{ new btSequentialImpulseConstraintSolver() },
	dynamicsWorld{ new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration) } {}

BulletPhysicsWorld& BulletPhysicsWorld::operator=(BulletPhysicsWorld& world) {
	return *this = BulletPhysicsWorld(world);
}

BulletPhysicsWorld::BulletPhysicsWorld(BulletPhysicsWorld&& world) noexcept
	: collisionConfiguration{ std::exchange(world.collisionConfiguration, nullptr) },
	dispatcher{ std::exchange(world.dispatcher, nullptr) },
	broadphase{ std::exchange(world.broadphase, nullptr) },
	solver{ std::exchange(world.solver, nullptr) },
	dynamicsWorld{ std::exchange(world.dynamicsWorld, nullptr) } {}

BulletPhysicsWorld& BulletPhysicsWorld::operator=(BulletPhysicsWorld&& world) noexcept {
	std::swap(collisionConfiguration, world.collisionConfiguration);
	std::swap(dispatcher, world.dispatcher);
	std::swap(broadphase, world.broadphase);
	std::swap(solver, world.solver);
	std::swap(dynamicsWorld, world.dynamicsWorld);
	return *this;
}

BulletPhysicsWorld::~BulletPhysicsWorld() {
	delete dynamicsWorld;
	delete solver;
	delete broadphase;
	delete dispatcher;
	delete collisionConfiguration;
}

void BulletPhysicsWorld::setGravity(float strength) {
	dynamicsWorld->setGravity(btVector3(0.0f, strength, 0.0f));
}