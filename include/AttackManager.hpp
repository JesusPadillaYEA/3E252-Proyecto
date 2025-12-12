#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "BarcoEntity.hpp"
#include "Jugador.hpp"
#include "IndicatorManager.hpp"

namespace AttackManager {

    // Procesamos el disparo y guardamos la pista
    inline bool procesarDisparo(sf::Vector2f posicionClick, 
                                sf::Vector2f origenDisparo, // Coordenada REAL del barco atacante
                                Jugador& enemigo, 
                                sf::Vector2u windowSize) {
        
        // 1. Calcular dónde cae el disparo en el mundo del enemigo (Inversión)
        sf::Vector2f posImpactoReal;
        posImpactoReal.x = (float)windowSize.x - posicionClick.x;
        posImpactoReal.y = (float)windowSize.y - posicionClick.y;

        // 2. Guardar Pista: Guardamos el origen REAL. 
        // El IndicatorManager se encargará de invertirlo visualmente.
        IndicatorManager::agregarPista(enemigo, origenDisparo, posImpactoReal);

        // 3. Daño
        auto& flota = enemigo.getFlota();
        for (auto& barco : flota) {
            if (barco.destruido) continue;

            if (barco.sprite.getGlobalBounds().contains(posImpactoReal)) {
                std::cout << ">>> IMPACTO EN: " << barco.nombre << std::endl;
                barco.recibirDano(9999); 
                return true; 
            }
        }
        std::cout << ">>> AGUA" << std::endl;
        return false; 
    }
}