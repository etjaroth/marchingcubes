#pragma warning(push 0)
#include "BulletPhysicsWorld.h"
#pragma warning(pop)
#include <iostream>

BulletPhysicsWorld::BulletPhysicsWorld()
	: collisionConfiguration{ new btDefaultCollisionConfiguration() },
	dispatcher{ new btCollisionDispatcher(collisionConfiguration) },
	broadphase{ new btDbvtBroadphase() },
	solver{ new btSequentialImpulseConstraintSolver() },
	dynamicsWorld{ new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration) } {
	std::cout << "Creating Physics World" << std::endl;
}

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

	//remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = getWorld()->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	delete dynamicsWorld;
	delete solver;
	delete broadphase;
	delete dispatcher;
	delete collisionConfiguration;

	std::cout << "Destroyed Physics World" << std::endl;
}

void BulletPhysicsWorld::update(double deltatime) {
	dynamicsWorld->stepSimulation(deltatime, 10);
}

btDiscreteDynamicsWorld* BulletPhysicsWorld::getWorld() {
	return dynamicsWorld;
}

void BulletPhysicsWorld::addRigidBody(btRigidBody* body) {
	dynamicsWorld->addRigidBody(body);
}

void BulletPhysicsWorld::removeRigidBody(btRigidBody* body) {
	dynamicsWorld->removeCollisionObject(body);
}

void BulletPhysicsWorld::setGravity(float strength) {
	dynamicsWorld->setGravity(btVector3(0.0f, strength, 0.0f));
}