#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Clase padre Barco
class Barco {
public:
    Barco(float x, float y, const std::string& tipo);
    virtual ~Barco();

    // Getters
    float getX() const;
    float getY() const;
    std::string getTipo() const;
    float getVelocidad() const;
    float getArmamento() const;
    float getResistencia() const;

    // Setters
    void setX(float x);
    void setY(float y);
    void setVelocidad(float vel);

    // Métodos virtuales
    virtual void disparar();
    virtual void mover(float deltaTime);
    virtual void dibujar(sf::RenderWindow& window);
    virtual std::string getInfo() const;

protected:
    float x, y;
    std::string tipo;
    float velocidad;
    float armamento;
    float resistencia;
    sf::CircleShape forma;
};
