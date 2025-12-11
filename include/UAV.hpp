#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Clase UAV (Unmanned Aerial Vehicle - Dron)
class UAV {
public:
    UAV(float x, float y, const std::string& modelo);
    ~UAV();

    // Getters
    float getX() const;
    float getY() const;
    std::string getModelo() const;
    float getVelocidad() const;
    float getAltitud() const;
    float getBateria() const;
    bool estaActivo() const;

    // Setters
    void setX(float x);
    void setY(float y);
    void setAltitud(float altitud);

    // Métodos
    void patrullar(float deltaTime);
    void disparar();
    void gastarBateria(float cantidad);
    void recargarBateria();
    void encender();
    void apagar();
    void dibujar(sf::RenderWindow& window);
    std::string getInfo() const;

private:
    float x, y;
    std::string modelo;
    float velocidad;
    float altitud; // Altura sobre el agua
    float bateria;
    float bateriaMax;
    bool activo;
    sf::CircleShape forma;
};
