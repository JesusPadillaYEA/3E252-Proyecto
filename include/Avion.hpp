#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Clase Avión
class Avion {
public:
    Avion(float x, float y, const std::string& tipo);
    ~Avion();

    // Getters
    float getX() const;
    float getY() const;
    std::string getTipo() const;
    float getVelocidad() const;
    float getAltitud() const;
    float getCombustible() const;

    // Setters
    void setX(float x);
    void setY(float y);
    void setAltitud(float altitud);

    // Métodos
    void volar(float deltaTime);
    void disparar();
    void gastarCombustible(float cantidad);
    void recargarCombustible();
    void dibujar(sf::RenderWindow& window);
    std::string getInfo() const;

private:
    float x, y;
    std::string tipo;
    float velocidad;
    float altitud; // Altura sobre el agua
    float combustible;
    float combustibleMax;
    sf::ConvexShape forma;
};
