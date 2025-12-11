#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "BarcoEntity.hpp"

// Estructura que representa el tablero del jugador (su flota y su base)
struct Grid {
    std::vector<BarcoEntity> flota;
    sf::Vector2f posicionBase; // Punto (0,0) relativo de este grid en la pantalla

    Grid(sf::Vector2f base) : posicionBase(base) {}
};

class Jugador {
public:
    int id;
    Grid grid;

    // Nuevo constructor que espera ID y Posición Base (Vector2f)
    Jugador(int numeroJugador, sf::Vector2f basePos) 
        : id(numeroJugador), grid(basePos) {
    }

    // Método helper para acceder a la flota fácilmente
    std::vector<BarcoEntity>& getFlota() {
        return grid.flota;
    }

    // Helper constante
    const std::vector<BarcoEntity>& getFlota() const {
        return grid.flota;
    }
};