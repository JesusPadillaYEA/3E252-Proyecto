#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "BarcoEntity.hpp"

// Estructura para la pista visual
struct FlechaPista {
    sf::Vector2f origen;   // Desde dónde disparó el enemigo
    sf::Vector2f impacto;  // Dónde cayó el disparo
    int turnosRestantes;   // Vida de la flecha (normalmente 1)
};

struct Grid {
    std::vector<BarcoEntity> flota;
    sf::Vector2f posicionBase;

    Grid(sf::Vector2f base) : posicionBase(base) {}
};

class Jugador {
public:
    int id;
    Grid grid;
    // NUEVO: Lista de pistas visuales que este jugador ve
    std::vector<FlechaPista> pistas;

    Jugador(int numeroJugador, sf::Vector2f basePos) 
        : id(numeroJugador), grid(basePos) {
    }

    std::vector<BarcoEntity>& getFlota() {
        return grid.flota;
    }
    const std::vector<BarcoEntity>& getFlota() const {
        return grid.flota;
    }
};