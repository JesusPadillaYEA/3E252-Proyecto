#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "BarcoEntity.hpp"
#include "Jugador.hpp"
#include "IndicatorManager.hpp" // Necesario para crear la pista

namespace AttackManager {

    // Ahora recibe el origen del disparo y al objeto Jugador enemigo completo
    inline bool procesarDisparo(sf::Vector2f posicionDisparo, sf::Vector2f origenDisparo, Jugador& enemigo) {
        
        // 1. GENERAR PISTA VISUAL (Siempre se genera, acierte o falle)
        // Esto permite al enemigo ver de dónde vino el tiro
        IndicatorManager::agregarPista(enemigo, origenDisparo, posicionDisparo);

        // 2. CÁLCULO DE IMPACTO
        auto& flota = enemigo.getFlota();
        for (auto& barco : flota) {
            if (barco.destruido) continue;

            if (barco.sprite.getGlobalBounds().contains(posicionDisparo)) {
                std::cout << ">>> IMPACTO CONFIRMADO EN: " << barco.nombre << std::endl;
                barco.recibirDano(9999); 
                return true; 
            }
        }

        std::cout << ">>> AGUA." << std::endl;
        return false; 
    }
}