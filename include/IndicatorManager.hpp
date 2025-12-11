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

    // Renderiza la flecha indicadora
    inline void renderizarPistas(sf::RenderWindow& window, const Jugador& jugadorActual) {
        if (jugadorActual.pistas.empty()) return;

        sf::Vector2f winSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
        
        // 1. PUNTO DE ANCLAJE: Siempre arriba al centro (Tu "Norte")
        sf::Vector2f centroSuperior = {winSize.x / 2.f, 60.f};

        for (const auto& flecha : jugadorActual.pistas) {
            
            // 2. CÁLCULO DEL VECTOR UNITARIO INVERTIDO
            // El disparo vino de 'flecha.origen' (coords del enemigo).
            // Para ti, ese punto está rotado 180 grados.
            // Origen Aparente = (W - OrigenReal.x, H - OrigenReal.y)
            
            sf::Vector2f origenAparente;
            origenAparente.x = winSize.x - flecha.origen.x;
            origenAparente.y = winSize.y - flecha.origen.y;

            // Vector dirección: Desde tu radar (arriba) hacia donde vino el tiro (aparente)
            sf::Vector2f direccion = origenAparente - centroSuperior;

            // Ángulo
            float angulo = std::atan2(direccion.y, direccion.x) * 180.f / 3.14159f;

            // 3. DIBUJAR FLECHA
            float largoFlecha = 60.f;

            sf::RectangleShape cuerpo({largoFlecha, 4.f});
            cuerpo.setOrigin({0.f, 2.f});
            cuerpo.setPosition(centroSuperior);
            cuerpo.setRotation(sf::degrees(angulo));
            cuerpo.setFillColor(sf::Color(255, 50, 50)); // Rojo Alerta

            sf::CircleShape cabeza(10.f, 3);
            cabeza.setOrigin({10.f, 10.f});
            
            float rad = angulo * 3.14159f / 180.f;
            sf::Vector2f puntaPos = centroSuperior + sf::Vector2f(std::cos(rad) * largoFlecha, std::sin(rad) * largoFlecha);
            
            cabeza.setPosition(puntaPos);
            cabeza.setRotation(sf::degrees(angulo + 90.f)); 
            cabeza.setFillColor(sf::Color(255, 50, 50));

            // Pivote Visual
            sf::CircleShape pivote(5.f);
            pivote.setOrigin({5.f, 5.f});
            pivote.setPosition(centroSuperior);
            pivote.setFillColor(sf::Color::White);

            window.draw(cuerpo);
            window.draw(cabeza);
            window.draw(pivote);
        }
    }
}