#pragma once
#pragma warning(push, 0)
#include "BulletPhysicsWorld.h"
#pragma warning(pop)
#include "CapsuleObject.h"
#include "FPSCamera.h"
#include "Settings.h"

class PC : public CapsuleObject
{
	FPSCamera camera;

public:
	PC(std::shared_ptr<BulletPhysicsWorld> physics, Settings& settings);
	~PC() { std::cout << "Deleted PC" << std::endl; }
	FPSCamera& getCamera();
};

