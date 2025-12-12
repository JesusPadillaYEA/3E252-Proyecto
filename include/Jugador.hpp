#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "BarcoEntity.hpp"

struct Grid {
    std::vector<BarcoEntity> flota;
    sf::Vector2f posicionBase;

    Grid(sf::Vector2f base) : posicionBase(base) {}
};

struct FlechaPista {
    sf::Vector2f origen;
    sf::Vector2f impacto;
    int turnosRestantes;
};

class Jugador {
public:
    int id;
    Grid grid;
    std::vector<FlechaPista> pistas;

    // --- NUEVAS REGLAS DE RADAR ---
    bool radarUsadoEnTurno;      // Limite: 1 vez por turno
    bool radarRefuerzoPendiente; // Si no hay portaviones, se activa al inicio del sig. turno

    Jugador(int numeroJugador, sf::Vector2f basePos) 
        : id(numeroJugador), grid(basePos) {
        
        // Inicializar estados
        radarUsadoEnTurno = false;
        radarRefuerzoPendiente = false;
    }

    std::vector<BarcoEntity>& getFlota() {
        return grid.flota;
    }
    const std::vector<BarcoEntity>& getFlota() const {
        return grid.flota;
    }
};