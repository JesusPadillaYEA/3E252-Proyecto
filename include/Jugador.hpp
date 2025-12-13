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

    // --- REGLAS DE RADAR ---
    int cooldownRadar;           // Contador de turnos para volver a usar (0 = listo)
    bool radarRefuerzoPendiente; // Si true, el UAV llega al inicio del siguiente turno

    // --- REGLAS DE AIR STRIKE (NUEVO) ---
    int cooldownAtaqueAereo;     // 0 = listo, >0 = esperando
    float columnaFuegoX;         // Coordenada X de la franja roja (-1 si no hay)

    // Memoria de Inteligencia (Notas)
    std::vector<sf::Vector2f> memoriaRadar;

    // --- NUEVO: REPORTE DE DAÑOS (EXPLOSIONES) ---
    std::vector<sf::Vector2f> explosionesPendientes; // Coordenadas de barcos destruidos

    Jugador(int numeroJugador, sf::Vector2f basePos) 
        : id(numeroJugador), grid(basePos) {
        
        // Inicializar estados
        cooldownRadar = 0; // Listo para usar al inicio
        radarRefuerzoPendiente = false;

        // Inicializar Air Strike
        cooldownAtaqueAereo = 0; 
        columnaFuegoX = -1.f;
    }

    std::vector<BarcoEntity>& getFlota() {
        return grid.flota;
    }
    const std::vector<BarcoEntity>& getFlota() const {
        return grid.flota;
    }
};