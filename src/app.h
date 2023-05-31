/*
Jakub Janeczko
nagłówek pętli głównej programu
30.05.2023
*/

#pragma once

#include "interfaces.h"
#include "GuiRenderer.h"

#include <memory>

class App {
public:
    GuiRenderer gui;
    std::unique_ptr<IPhysicsEngine> engine;

    App( GuiRenderer gui, std::unique_ptr<IPhysicsEngine> engine )
        : gui(std::move(gui))
        , engine(std::move(engine)) {}
    
    void run();
};