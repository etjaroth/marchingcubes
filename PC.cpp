#include "PC.h"

PC::PC(std::shared_ptr<BulletPhysicsWorld> physics, Settings& settings)
	: camera{ settings.getConstants().spawnpoint, settings.getConstants().spawnViewDir, settings.getConstants().cameraSpeed },
	CapsuleObject{
	physics,
	1.0f,
	3.0f,
	50.0f,
	btVector3(settings.getConstants().spawnpoint.x, settings.getConstants().spawnpoint.y, settings.getConstants().spawnpoint.z)
} {std::cout << "Made PC" << std::endl; }

FPSCamera& PC::getCamera() {
	return camera;
}