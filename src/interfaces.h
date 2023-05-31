/*
Jakub Janeczko
definicje interfejsów
31.05.2023
*/

#pragma once

#include "PhysicsObject.h"

#include <glm/glm.hpp>

#include <vector>

/**
 * @brief obiekt zawierające dodatkowe informacje dla renderera
 */
struct renderer_info {
    bool fill_objects; ///< czy obiekty powinny być wypełnione
    std::vector<glm::dvec2> additional_points; ///< dodatkowe punkty
    std::vector<glm::dvec2> additional_lines; ///< dodatkowe krawędzie
    std::vector<glm::dvec2> additional_triangles; ///< dodatkowe trójkąty
    glm::dvec2 camPos; ///< pozycja kamery [m]
    double camZoom; ///< zoom kamery [px/m]

    std::vector<glm::u8vec4> object_colors; ///< kolory kolejnych obiektów
};

/**
 * @brief interfejs klasy obsługującej GUI
 */
class IGui {
public:
    virtual ~IGui() = default;

    /**
     * @brief obsługuje interakcje z użytkownikiem i przygotowuje GUI do narysowania
     * 
     * @param objs obiekty w symulacji
     * @param dt różnica czasu od poprzedniego kroku symulacji
     * @return renderer_info 
     */
    virtual renderer_info handle_gui( std::vector<PhysicsObject> &objs, double dt ) = 0;
};


/**
 * @brief interfejs klasy obsługującej rysowanie i okno aplikacji
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;

    /**
     * @brief rysuje GUI wraz z obiektami
     * 
     * @param objs obiekty w symulacji
     * @param dt różnica czasu od poprzedniego kroku symulacji
     * @return true program powinien kontynuować działanie
     * @return false program powinien się zakończyć
     */
    virtual bool draw( const renderer_info &ri,
        const std::vector<PhysicsObject> &objs, double dt ) = 0;
};

/**
 * @brief interfejs klasy obsługującej interakcje pomiędzy obiektami
 */
class IPhysicsEngine {
public:
    virtual ~IPhysicsEngine() = default;

    /**
     * @brief oblicza stan symulacji po upływie dt czasu
     * 
     * @param objs obiekty w symulacji
     * @param dt różnica czasu od poprzedniego kroku symulacji
     */
    virtual void onTick( std::vector<PhysicsObject> &objs, double dt ) = 0;
};