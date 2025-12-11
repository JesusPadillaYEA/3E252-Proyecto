#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Jugador.hpp"
#include "BarcoEntity.hpp"

namespace GeneradorFlotas {

    // Función para poblar el grid de un jugador
    inline void inicializarFlota(Jugador& jugador, 
                                 const sf::Texture& tDestructor, 
                                 const sf::Texture& tPortaviones,
                                 const sf::Texture& tSubmarino) {
        
        sf::Vector2f base = jugador.grid.posicionBase;
        auto& flota = jugador.grid.flota;

        // Limpiamos por si acaso
        flota.clear();

        if (jugador.id == 1) {
            // --- CONFIGURACIÓN JUGADOR 1 (Izquierda) ---
            
            // Barco 1: Destructor (Relativo a la base)
            flota.emplace_back(tDestructor, "Destructor P1");
            flota.back().sprite.setScale({0.3f, 0.3f});
            // Posición relativa: Base + (50, 50)
            flota.back().sprite.setPosition({base.x + 50.f, base.y + 50.f});

            // Barco 2: Portaviones
            flota.emplace_back(tPortaviones, "Portaviones P1");
            flota.back().sprite.setScale({0.5f, 0.5f});
            flota.back().sprite.setPosition({base.x + 50.f, base.y + 250.f});
        } 
        else {
            // --- CONFIGURACIÓN JUGADOR 2 (Derecha) ---
            
            // Nota: Usamos la base para posicionar. Si la base es (700, 0), sumamos desde ahí.
            
            // Barco 1: Submarino A
            flota.emplace_back(tSubmarino, "Submarino A");
            flota.back().sprite.setScale({0.4f, 0.4f});
            flota.back().sprite.setPosition({base.x, base.y + 50.f});
            flota.back().sprite.setColor({200, 200, 255}); // Tinte diferente

            // Barco 2: Submarino B
            flota.emplace_back(tSubmarino, "Submarino B");
            flota.back().sprite.setScale({0.4f, 0.4f});
            flota.back().sprite.setPosition({base.x, base.y + 350.f});
            flota.back().sprite.setColor({200, 200, 255});
        }
    }
}