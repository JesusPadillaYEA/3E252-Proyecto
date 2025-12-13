#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "BarcoEntity.hpp"
#include "Jugador.hpp"
#include "IndicatorManager.hpp"

namespace AttackManager {

    // Se añade el parámetro: std::vector<sf::Vector2f>& bajasReportadas
    inline bool procesarDisparo(sf::Vector2f posicionImpacto, 
                                sf::Vector2f origenDisparo, 
                                Jugador& enemigo,
                                std::vector<sf::Vector2f>& bajasReportadas) { // <--- NUEVO PARAMETRO
        
        //IndicatorManager::agregarPista(enemigo, origenDisparo, posicionImpacto);

        auto& flota = enemigo.getFlota();
        for (auto& barco : flota) {
            if (barco.destruido) continue;

            if (barco.sprite.getGlobalBounds().contains(posicionImpacto)) {
                std::cout << ">>> IMPACTO CONFIRMADO EN: " << barco.nombre << std::endl;
                barco.recibirDano(9999); 
                
                // Si el disparo destruyó el barco, guardamos su posición central
                if (barco.destruido) {
                    sf::FloatRect b = barco.sprite.getGlobalBounds();
                    bajasReportadas.push_back({b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f});
                }
                return true; 
            }
        }

        std::cout << ">>> AGUA en (" << posicionImpacto.x << ", " << posicionImpacto.y << ")" << std::endl;
        return false; 
    }

    // Se añade el parámetro: std::vector<sf::Vector2f>& bajasReportadas
    inline void procesarAtaqueAereo(float xImpacto, Jugador& enemigo, std::vector<sf::Vector2f>& bajasReportadas) { // <--- NUEVO PARAMETRO
        float anchoAtaque = 60.f;
        enemigo.columnaFuegoX = xImpacto;

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
                
                // Guardar posición central de la baja
                sf::FloatRect b = barco.sprite.getGlobalBounds();
                bajasReportadas.push_back({b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f});
                
                huboImpacto = true;
            }
        }

        if (!huboImpacto) {
            std::cout << ">>> EL AIR STRIKE NO IMPACTO NINGUN OBJETIVO." << std::endl;
        }
    }
}