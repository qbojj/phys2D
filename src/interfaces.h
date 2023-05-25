#pragma once
#include "PhysicsObject.h"
#include <vector>

class IGuiRenderer {
public:
    virtual ~IGuiRenderer() = default;

    // return false if should close
    virtual bool onDraw( std::vector<PhysicsObject> &objs, double dt ) = 0;
};

class IPhysicsEngine {
public:
    virtual ~IPhysicsEngine() = default;
    virtual void onTick( std::vector<PhysicsObject> &objs, double dt ) = 0;
};