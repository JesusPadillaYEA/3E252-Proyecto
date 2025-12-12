#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Jugador.hpp"
#include "BarcoEntity.hpp"

namespace GeneradorFlotas {

    inline void inicializarFlota(Jugador& jugador, 
                                 const sf::Texture& tDestructor, 
                                 const sf::Texture& tPortaviones,
                                 const sf::Texture& tSubmarino) {
        
        sf::Vector2f base = jugador.grid.posicionBase;
        auto& flota = jugador.grid.flota;

        // Limpiamos por seguridad
        flota.clear();

        if (jugador.id == 1) {
            // --- JUGADOR 1 (Izquierda) ---
            
            // 1. Destructor
            flota.emplace_back(tDestructor, "Destructor P1");
            flota.back().sprite.setScale({0.3f, 0.3f});
            flota.back().sprite.setPosition({base.x + 50.f, base.y + 50.f});

            // 2. Portaviones
            flota.emplace_back(tPortaviones, "Portaviones P1");
            flota.back().sprite.setScale({0.5f, 0.5f});
            flota.back().sprite.setPosition({base.x + 50.f, base.y + 250.f});
        } 
        else {
            // --- JUGADOR 2 (Derecha) ---
            
            // 1. Submarino P2 (Posición Y = 50)
            flota.emplace_back(tSubmarino, "Submarino P2");
            auto& sub = flota.back().sprite;
            sub.setScale({0.4f, 0.4f});
            sub.setPosition({base.x + 20.f, base.y + 50.f});
            sub.setColor({200, 200, 255}); 

            // 2. Portaviones P2 (MOVIDO A Y = 350)
            // Se aumentó la separación vertical para evitar que choque con el Submarino.
            flota.emplace_back(tPortaviones, "Portaviones P2");
            auto& carrier = flota.back().sprite;
            carrier.setScale({0.5f, 0.5f});
            
            // Nueva posición: Más abajo para dar espacio
            carrier.setPosition({base.x + 50.f, base.y + 350.f}); 
        }
    }
}