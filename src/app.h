/*
Jakub Janeczko
nagłówek pętli głównej programu
31.05.2023
*/

#pragma once

#include "interfaces.h"
#include "GuiRenderer.h"

/**
 * @brief Klasa aplikacji
 * 
 * implementuje pętlę główną programu oraz
 * łączy GUI i silnik fizyki
 */
class App {
public:
    GuiRenderer gui; ///< gui aplikacji
    IPhysicsEngine *engine; ///< silnik fizyki

    App( GuiRenderer gui, IPhysicsEngine *engine )
        : gui(std::move(gui))
        , engine(engine) {}
    
    /**
     * @brief pętla główna programu
     */
    void run();
};