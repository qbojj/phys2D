#pragma once
#include "interfaces.h"
#include <vector>
#include "PhysicsObject.h"

class Simple_PhysicsEngine : public IPhysicsEngine {
public:
    void onTick( std::vector<PhysicsObject> & ) override;
};