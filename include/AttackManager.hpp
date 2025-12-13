#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "BarcoEntity.hpp"
#include "Jugador.hpp"
#include "IndicatorManager.hpp"

namespace AttackManager {

    // AHORA ACEPTA EL CUARTO PARÁMETRO: std::vector<sf::Vector2f>& bajasReportadas
    inline bool procesarDisparo(sf::Vector2f posicionImpacto, 
                                sf::Vector2f origenDisparo, 
                                Jugador& enemigo,
                                std::vector<sf::Vector2f>& bajasReportadas) { 
        
        IndicatorManager::agregarPista(enemigo, origenDisparo, posicionImpacto);

        auto& flota = enemigo.getFlota();
        for (auto& barco : flota) {
            if (barco.destruido) continue;

            if (barco.sprite.getGlobalBounds().contains(posicionImpacto)) {
                barco.recibirDano(9999); 
                
                // SI SE DESTRUYÓ, GUARDAMOS LA POSICIÓN EN EL VECTOR
                if (barco.destruido) {
                    sf::FloatRect b = barco.sprite.getGlobalBounds();
                    bajasReportadas.push_back({b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f});
                }
                return true; 
            }
        }
        return false; 
    }

    // AHORA ACEPTA EL TERCER PARÁMETRO: std::vector<sf::Vector2f>& bajasReportadas
    inline void procesarAtaqueAereo(float xImpacto, Jugador& enemigo, std::vector<sf::Vector2f>& bajasReportadas) {
        float anchoAtaque = 60.f;
        enemigo.columnaFuegoX = xImpacto; // Marca visual para el turno enemigo

        sf::FloatRect zonaAtaque({xImpacto - (anchoAtaque / 2.f), -500.f}, {anchoAtaque, 2000.f});

        auto& flota = enemigo.getFlota();
        for (auto& barco : flota) {
            if (barco.destruido) continue;

            sf::FloatRect boundsBarco = barco.sprite.getGlobalBounds();
            
            // Usamos findIntersection (SFML 3 devuelve std::optional, evaluamos si tiene valor)
            if (boundsBarco.findIntersection(zonaAtaque)) {
                barco.recibirDano(10000); 
                
                // GUARDAR POSICIÓN DE LA BAJA
                if (barco.destruido) {
                    sf::FloatRect b = barco.sprite.getGlobalBounds();
                    bajasReportadas.push_back({b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f});
                }
            }
        }
    }
}