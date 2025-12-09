#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Clase Jugador
class Jugador {
public:
    Jugador(const std::string& nombre, float x, float y);
    ~Jugador();

    // Getters
    std::string getNombre() const;
    float getX() const;
    float getY() const;
    int getOro() const;
    int getVidas() const;
    int getNivel() const;

    // Setters
    void setNombre(const std::string& nombre);
    void setX(float x);
    void setY(float y);
    void setOro(int oro);
    void setVidas(int vidas);
    void setNivel(int nivel);

    // Métodos
    void ganarOro(int cantidad);
    void perderOro(int cantidad);
    void ganarVida();
    void perderVida();
    void subirNivel();
    void dibujar(sf::RenderWindow& window);
    std::string getInfo() const;

private:
    std::string nombre;
    float x, y;
    int oro;
    int vidas;
    int nivel;
    sf::CircleShape forma;
};
