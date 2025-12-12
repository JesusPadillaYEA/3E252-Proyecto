#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Jugador.hpp"

namespace IndicatorManager {

    inline void agregarPista(Jugador& victima, sf::Vector2f origenDisparo, sf::Vector2f puntoImpacto) {
        FlechaPista nueva = {origenDisparo, puntoImpacto, 1};
        victima.pistas.push_back(nueva);
    }

    inline void actualizarTurnos(Jugador& jugadorActual) {
        auto& lista = jugadorActual.pistas;
        lista.erase(std::remove_if(lista.begin(), lista.end(),
            [](FlechaPista& f) {
                f.turnosRestantes--; 
                return f.turnosRestantes < 0; 
            }), lista.end());
    }

    inline void renderizarPistas(sf::RenderWindow& window, const Jugador& jugadorActual, bool esAtaque) {
        if (jugadorActual.pistas.empty()) return;

        sf::Vector2f winSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
        
        // 1. DEFINIR PUNTO DE ORIGEN DEL VECTOR (El Radar)
        sf::Vector2f centroRadar;
        if (esAtaque) {
            // Modo Ataque: Desde abajo (Asistencia de apuntado)
            centroRadar = {winSize.x / 2.f, winSize.y - 50.f};
        } else {
            // Modo Defensa: Desde arriba (Alerta de impacto)
            centroRadar = {winSize.x / 2.f, 50.f};
        }

        for (const auto& flecha : jugadorActual.pistas) {
            
            // 2. CALCULAR POSICIÓN REAL DEL ENEMIGO EN TU PANTALLA (ESPEJO)
            // Si el enemigo disparó desde (100, 100), para ti eso es (900, 600)
            sf::Vector2f posEnemigoVisual;
            posEnemigoVisual.x = winSize.x - flecha.origen.x;
            posEnemigoVisual.y = winSize.y - flecha.origen.y;

            // 3. CALCULAR EL VECTOR (Dirección)
            // Vector = Destino - Origen
            sf::Vector2f vectorDireccion = posEnemigoVisual - centroRadar;

            // 4. CALCULAR ÁNGULO
            float angulo = std::atan2(vectorDireccion.y, vectorDireccion.x) * 180.f / 3.14159f;

            // --- DIBUJADO ---
            float largoFlecha = 80.f; // Un poco más larga para que se note la dirección
            sf::Color color = esAtaque ? sf::Color(255, 165, 0) : sf::Color(255, 50, 50);

            // Cuerpo
            sf::RectangleShape cuerpo({largoFlecha, 4.f});
            cuerpo.setOrigin({0.f, 2.f});
            cuerpo.setPosition(centroRadar);
            cuerpo.setRotation(sf::degrees(angulo));
            cuerpo.setFillColor(color);

            // Cabeza
            sf::CircleShape cabeza(10.f, 3);
            cabeza.setOrigin({10.f, 10.f});
            
            // Posición de la punta
            float rad = angulo * 3.14159f / 180.f;
            sf::Vector2f puntaPos = centroRadar + sf::Vector2f(std::cos(rad) * largoFlecha, std::sin(rad) * largoFlecha);
            
            cabeza.setPosition(puntaPos);
            cabeza.setRotation(sf::degrees(angulo + 90.f)); 
            cabeza.setFillColor(color);

            // Pivote (Centro del radar)
            sf::CircleShape pivote(5.f);
            pivote.setOrigin({5.f, 5.f});
            pivote.setPosition(centroRadar);
            pivote.setFillColor(sf::Color::White);

            window.draw(cuerpo);
            window.draw(cabeza);
            window.draw(pivote);

            // (Opcional) Si es ataque, dibujamos una "mira fantasma" donde deberías disparar
            if (esAtaque) {
                sf::CircleShape miraFantasma(5.f);
                miraFantasma.setOrigin({5.f, 5.f});
                miraFantasma.setPosition(posEnemigoVisual);
                miraFantasma.setFillColor(sf::Color(255, 255, 255, 100)); // Semi-transparente
                window.draw(miraFantasma);
            }
        }
    }
}