#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "BarcoEntity.hpp"
#include "Jugador.hpp"
#include "IndicatorManager.hpp"

namespace AttackManager {

    inline bool procesarDisparo(sf::Vector2f posicionImpacto, 
                                sf::Vector2f origenDisparo, 
                                Jugador& enemigo) {
        
        // 1. GENERAR PISTA VISUAL
        // Guardamos el origen real para que el IndicatorManager dibuje la flecha
        IndicatorManager::agregarPista(enemigo, origenDisparo, posicionImpacto);

        // 2. CÁLCULO DE IMPACTO (Coordenadas Globales)
        // Ya no invertimos coordenadas. Si disparas a X=800, compruebas en X=800.
        auto& flota = enemigo.getFlota();
        for (auto& barco : flota) {
            if (barco.destruido) continue;

            if (barco.sprite.getGlobalBounds().contains(posicionImpacto)) {
                std::cout << ">>> IMPACTO CONFIRMADO EN: " << barco.nombre << std::endl;
                barco.recibirDano(9999); 
                return true; 
            }
        }

        std::cout << ">>> AGUA en (" << posicionImpacto.x << ", " << posicionImpacto.y << ")" << std::endl;
        return false; 
    }
}