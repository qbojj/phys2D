#pragma once
#include "interfaces.h"

#include <memory>

class App {
public:
    std::unique_ptr<IGuiRenderer> gui;
    std::unique_ptr<IPhysicsEngine> engine;

    App( std::unique_ptr<IGuiRenderer> gui, std::unique_ptr<IPhysicsEngine> engine )
        : gui(std::move(gui))
        , engine(std::move(engine)) {}
    
    int run();
};