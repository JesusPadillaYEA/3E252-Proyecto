#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "BarcoEntity.hpp"
#include "Jugador.hpp"
#include "IndicatorManager.hpp"

namespace AttackManager {

    // Añadimos windowSize para calcular la inversión
    inline bool procesarDisparo(sf::Vector2f posicionClick, 
                                sf::Vector2f origenDisparo, 
                                Jugador& enemigo, 
                                sf::Vector2u windowSize) {
        
        // 1. TRANSFORMACIÓN DE COORDENADAS (Lógica de Espejo/Inversión)
        // El clic del atacante se invierte para coincidir con el mundo del enemigo
        // Ejemplo: Click en (900, 600) -> Se convierte en (100, 100)
        sf::Vector2f posImpactoReal;
        posImpactoReal.x = (float)windowSize.x - posicionClick.x;
        posImpactoReal.y = (float)windowSize.y - posicionClick.y;

        // 2. GENERAR PISTA VISUAL
        // Guardamos el origen REAL del disparo. El IndicatorManager se encargará
        // de invertirlo visualmente cuando le toque al otro jugador.
        IndicatorManager::agregarPista(enemigo, origenDisparo, posImpactoReal);

        // 3. CÁLCULO DE IMPACTO
        // Usamos posImpactoReal para chequear contra los barcos enemigos
        auto& flota = enemigo.getFlota();
        for (auto& barco : flota) {
            if (barco.destruido) continue;

            if (barco.sprite.getGlobalBounds().contains(posImpactoReal)) {
                std::cout << ">>> IMPACTO CONFIRMADO EN: " << barco.nombre << std::endl;
                barco.recibirDano(9999); 
                return true; 
            }
        }

        std::cout << ">>> AGUA EN COORDENADA INVERTIDA: " 
                  << posImpactoReal.x << "," << posImpactoReal.y << std::endl;
        return false; 
    }
}