#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "BarcoEntity.hpp"
#include "Jugador.hpp"
#include "IndicatorManager.hpp"

namespace AttackManager {

    // --- ATAQUE AÉREO MODIFICADO ---
    inline void procesarAtaqueAereo(float xImpacto, Jugador& enemigo) {
        float anchoAtaque = 60.f;
        
        // 1. MARCAR LA FRANJA ROJA PARA EL TURNO ENEMIGO
        enemigo.columnaFuegoX = xImpacto;

        // 2. CALCULAR DAÑO
        sf::FloatRect zonaAtaque({xImpacto - (anchoAtaque / 2.f), -500.f}, {anchoAtaque, 2000.f});

        std::cout << ">>> INICIANDO AIR STRIKE EN COLUMNA X: " << xImpacto << std::endl;

        auto& flota = enemigo.getFlota();
        bool huboImpacto = false;

        for (auto& barco : flota) {
            if (barco.destruido) continue;

            sf::FloatRect boundsBarco = barco.sprite.getGlobalBounds();
            
            if (boundsBarco.findIntersection(zonaAtaque)) {
                std::cout << ">>> BOMBARDEO EXITOSO EN: " << barco.nombre << std::endl;
                barco.recibirDano(10000); 
                huboImpacto = true;
            }
        }

        if (!huboImpacto) {
            std::cout << ">>> EL AIR STRIKE NO IMPACTO NINGUN OBJETIVO." << std::endl;
        }
    }

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