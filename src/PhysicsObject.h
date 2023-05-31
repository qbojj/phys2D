/*
Jakub Janeczko
nagłówek klasy obiektu fizyki i odcinka
31.05.2023
*/

#pragma once

#include <glm/glm.hpp>

#include <vector>

/**
 * \brief skierowany odcinek
 */
class Edge
{
public:
    glm::dvec2 a, b;

    /**
     * @brief oblicza odległość ze znakiem punktu od prostej a-b
     * 
     * zwraca dodatnią wartość po jej lewej stronie patrząc się z a do b
     * równą zeru gdy punkt jest na prostej
     * a ujemną w przeciwnym przypadku
     * 
     * mkduł zwrócowaj wartości jest równy odległości kartezjańskiej punktu od prostej
     * 
     * @param p punkt do rozpatrzenia odległości
     * @return double odległość ze znakiem prostej od punktu
     */
    double signed_distance( const glm::dvec2 &p ) const;


    /**
     * @brief oblicza wektor normalny do odcinka
     * 
     * wektor normalny skierowany jest w 'lewo' patrząc się z a do b
     * 
     * @param p punkt do rozpatrzenia odległości
     * @return glm::dvec2 wektor normalny do odcinka
     */
    glm::dvec2 get_normal() const;
};

/**
 * \brief symulowany obiekt
 * 
 * zawiera wszystkie informacje potrzebe do obliczenia jego stanu po upływie
 * cząstki czasu
 * 
 * jednostki:
 * - długości: metr [m]
 * - masy: kilogram [kg]
 * - czasu: sekunda [s]
 * - kąta: radian [rad]
 */
class PhysicsObject {
public:
    /**
     * @brief tworzy nowy symulowany obiekt z chmury punktów i gęstości
     * 
     * tworzy z chmury punktów otoczkę wypukłą zawiniętą odwrotnie do
     * ruchu wskazówek zegara
     * 
     * następnie oblicza masę, środek oraz moment bezwładności z otoczki i gęstości
     * 
     * jeżeli podane było mniej niż 3 punkty std::invalid_argument zostaje rzucony
     * 
     * @param point_cloud chmura punktów obiektu [m]
     * @param density gęstość materiału [kg/m2]
     * @param flags 0 lub więcej dodatkowych opcji (zobacz FlagBits)
     */
    PhysicsObject(std::vector<glm::dvec2> point_cloud, double density, uint32_t flags = 0 );

    std::vector<glm::dvec2> points; ///< pozycje punktów względem środka [m]
    double inv_mass; ///< odwrotność masy [1/kg]
    double inv_moment_of_intertia; ///< odwrotność momentu bezwładności [1/(kg*m2)]
    
    glm::dvec2 center; ///< pozycja środka obiektu [m]
    glm::dvec2 velocity; ///< prędkość obiektu [m/s]
    double angle; ///< kąt obiektu zwględem początkowej pozycji [rad]
    double ang_velocity; ///< prędkość kątowa obiektu [rad/s]

    /**
     * @brief dodatkowe opcje symulowanego obiektu
     */
    enum FlagBits {
        Immovable = 1 << 0 ///< obiekt który nie powinien się poruszać
    };

    uint32_t flags; ///< dodatkowe opcje, 0 lub więcej bitów FlagBits

    /**
     * @brief zaaplikuj impuls do obiektu
     * 
     * @param point_of_application punkt przyłożenia siły
     * @param value impuls przyłożony [N * s]
     */
    void add_impulse( glm::dvec2 point_of_application, glm::dvec2 value );
    
    /**
     * @brief oblicza stan po upływie dt sekund
     * 
     * zgodnie z prędkością liniową i prędkością kątową
     * 
     * @param dt różnica czasu
     */
    void time_step( double dt );

    /**
     * @brief przesuwa obiekt o zadany wektor i kąt
     * 
     * @param vec wektor do przesunięcia
     * @param angle kąt do obrócenia
     */
    void move_by( glm::dvec2 vec, double angle );


    /**
     * get the closest edge to the point
     */

    /**
     * @brief znajduje krawędź która jest najbliżej punktu
     * 
     * zwraca Edge tak że dodatnia odległość jest na zewnątrz obiektu
     * 
     * @param point punkt
     * @return Edge 
     */
    Edge get_closest_edge( const glm::dvec2 &point ) const;

    /**
     * @brief sprawdza czy punkt jest we wnętrzu obiektu
     * 
     * @param p punkt
     * @return true jeżeli p jest we wnętrzu obiektu
     * @return false w przeciwnym przypadku
     */
    bool is_point_inside_object( const glm::dvec2 &p ) const;
};

/**
 * @brief oblicza trzecią współrzędną cross-produktu pomiędzy wektorami
 */
inline double vec_cross( const glm::dvec2 &a, const glm::dvec2 &b )
{
    return a.x*b.y - a.y*b.x;
}