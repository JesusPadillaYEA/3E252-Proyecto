#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm> // <--- ESTO FALTABA (necesario para std::remove_if)
#include "Jugador.hpp"

namespace IndicatorManager {

    // Registra una nueva pista en la víctima
    inline void agregarPista(Jugador& victima, sf::Vector2f origenDisparo, sf::Vector2f puntoImpacto) {
        // La pista dura 1 turno completo (el turno de la víctima)
        FlechaPista nueva = {origenDisparo, puntoImpacto, 1};
        victima.pistas.push_back(nueva);
    }

    // Reduce la vida de las flechas y borra las viejas
    inline void actualizarTurnos(Jugador& jugadorActual) {
        auto& lista = jugadorActual.pistas;
        
        // Eliminar flechas que ya cumplieron su ciclo
        lista.erase(std::remove_if(lista.begin(), lista.end(),
            [](FlechaPista& f) {
                f.turnosRestantes--; 
                return f.turnosRestantes < 0; 
            }), lista.end());
    }

    // Dibuja las flechas activas
    inline void renderizarPistas(sf::RenderWindow& window, const Jugador& jugadorActual) {
        for (const auto& flecha : jugadorActual.pistas) {
            
            // Matemáticas para dibujar una flecha rotada
            sf::Vector2f diff = flecha.impacto - flecha.origen;
            float longitud = std::sqrt(diff.x*diff.x + diff.y*diff.y);
            float angulo = std::atan2(diff.y, diff.x) * 180.f / 3.14159f;

            // 1. Cuerpo de la flecha (Línea)
            sf::RectangleShape linea({longitud, 3.f});
            linea.setOrigin({0.f, 1.5f});
            linea.setPosition(flecha.origen);
            linea.setRotation(sf::degrees(angulo)); 
            linea.setFillColor(sf::Color::Red);

            // 2. Cabeza de la flecha (Triángulo)
            sf::CircleShape cabeza(8.f, 3);
            cabeza.setOrigin({8.f, 8.f});
            cabeza.setPosition(flecha.impacto);
            cabeza.setRotation(sf::degrees(angulo + 90.f));
            cabeza.setFillColor(sf::Color::Red);

            window.draw(linea);
            window.draw(cabeza);
        }
    }
}