#pragma once
#include "PhysicsObject.h"
#include <vector>

class TriangleMeshObject :
    public PhysicsObjectShape, public PhysicsObject
{

public:

    struct Triangle {
        btVector3 v1;
        btVector3 v2;
        btVector3 v3;
    };

private:

    std::shared_ptr<btTriangleMesh> physicsMesh;
    std::shared_ptr<btBvhTriangleMeshShape> shape;

    btTriangleMesh* genPhysicsMesh(std::vector<Triangle>& data);

public:

    TriangleMeshObject(std::shared_ptr<BulletPhysicsWorld> physics, float mass, btVector3 position, std::vector<Triangle>& data);
    virtual ~TriangleMeshObject();

};

