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
        
        auto& flota = jugador.grid.flota;
        flota.clear();

        // Escalas fijas
        sf::Vector2f sDest = {0.3f, 0.3f};
        sf::Vector2f sPort = {0.5f, 0.5f};
        sf::Vector2f sSub  = {0.4f, 0.4f};

        if (jugador.id == 1) {
            // --- JUGADOR 1 (Zona: 0 a 500) ---
            // Distribuimos entre X=50 y X=400 para usar el centro.
            
            // 1. Portaviones (Centro-Atrás)
            flota.emplace_back(tPortaviones, "Portaviones P1");
            flota.back().sprite.setScale(sPort);
            flota.back().sprite.setPosition({100.f, 250.f}); 

            // 2. Submarinos (Flancos avanzados)
            // Arriba-Derecha (dentro de su zona)
            flota.emplace_back(tSubmarino, "Submarino Alpha P1");
            flota.back().sprite.setScale(sSub);
            flota.back().sprite.setPosition({350.f, 80.f});

            // Abajo-Derecha (dentro de su zona)
            flota.emplace_back(tSubmarino, "Submarino Beta P1");
            flota.back().sprite.setScale(sSub);
            flota.back().sprite.setPosition({350.f, 450.f});

            // 3. Destructores (Dispersos para cubrir huecos)
            // Medio-Arriba
            flota.emplace_back(tDestructor, "Destructor A P1");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition({220.f, 150.f});

            // Medio-Abajo
            flota.emplace_back(tDestructor, "Destructor B P1");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition({200.f, 450.f});

            // Frente-Centro (Vanguardia)
            flota.emplace_back(tDestructor, "Destructor C P1");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition({420.f, 320.f});
        } 
        else {
            // --- JUGADOR 2 (Zona: 500 a 1000) ---
            // Distribuimos entre X=550 y X=850 (Dejando espacio a la derecha para botones)
            
            // 1. Portaviones (Centro-Atrás, lado derecho)
            flota.emplace_back(tPortaviones, "Portaviones P2");
            flota.back().sprite.setScale(sPort);
            flota.back().sprite.setPosition({820.f, 280.f}); 

            // 2. Submarinos (Flancos avanzados hacia el centro del mapa)
            // Arriba-Izquierda (cerca de la frontera con P1)
            flota.emplace_back(tSubmarino, "Submarino Alpha P2");
            flota.back().sprite.setScale(sSub);
            flota.back().sprite.setPosition({550.f, 100.f});

            // Abajo-Izquierda
            flota.emplace_back(tSubmarino, "Submarino Beta P2");
            flota.back().sprite.setScale(sSub);
            flota.back().sprite.setPosition({550.f, 450.f});

            // 3. Destructores (Defensa interna)
            // Medio-Arriba
            flota.emplace_back(tDestructor, "Destructor A P2");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition({700.f, 150.f}); 

            // Medio-Abajo
            flota.emplace_back(tDestructor, "Destructor B P2");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition({720.f, 480.f}); 

            // Centro (Protegiendo al portaviones)
            flota.emplace_back(tDestructor, "Destructor C P2");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition({650.f, 350.f});

            // Color distintivo para P2
            for (auto& barco : flota) {
                barco.sprite.setColor({200, 200, 255});
            }
        }
    }
}