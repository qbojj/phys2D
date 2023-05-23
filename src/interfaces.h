#pragma once
#include "PhysicsObject.h"
#include <vector>

class IGuiRenderer {
public:
    virtual ~IGuiRenderer() = default;
    virtual void onDraw( std::vector<PhysicsObject> & ) = 0;
};

class IPhysicsEngine {
public:
    virtual ~IPhysicsEngine() = default;
    virtual void onTick( std::vector<PhysicsObject> & ) = 0;
};