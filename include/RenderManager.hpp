#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "BarcoEntity.hpp"

namespace RenderManager {

    // Se agrega el parámetro 'debugHitbox' al final (por defecto false)
    inline void renderizarFlota(sf::RenderWindow& window, const std::vector<BarcoEntity>& flota, 
                                int indiceSeleccionado, EstadoJuego estadoActual,
                                bool moviendo, const sf::Color& colorBorde, float rotacion = 0.f,
                                bool debugHitbox = false) { 
        
        for (size_t i = 0; i < flota.size(); ++i) {
            const auto& barco = flota[i];
            if (barco.destruido) continue;

            // Copia temporal para dibujar
            sf::Sprite dibujo = barco.sprite; 
            
            // --- APLICAR ROTACIÓN (MODO ESPEJO) ---
            if (rotacion != 0.f) {
                // Rotamos alrededor del centro del sprite
                sf::FloatRect bounds = dibujo.getLocalBounds();
                dibujo.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
                
                // Ajustamos posición para compensar el origen
                dibujo.move({bounds.size.x * dibujo.getScale().x / 2.f, bounds.size.y * dibujo.getScale().y / 2.f});
                
                dibujo.setRotation(sf::degrees(rotacion));
            }

            // Colores
            if ((int)i == indiceSeleccionado) {
                dibujo.setColor(sf::Color(255, 200, 200));
            } else {
                dibujo.setColor(barco.sprite.getColor());
            }

            window.draw(dibujo);

            // Borde selección
            if ((int)i == indiceSeleccionado) {
                sf::RectangleShape borde(barco.sprite.getGlobalBounds().size);
                borde.setPosition(barco.sprite.getPosition());
                borde.setFillColor(sf::Color::Transparent);
                borde.setOutlineThickness(2.f);
                borde.setOutlineColor(colorBorde);
                window.draw(borde);
            }

            // --- DEBUG HITBOX (SOLUCIÓN BUG) ---
            if (debugHitbox) {
                sf::FloatRect bounds = dibujo.getGlobalBounds();
                
                // 1. Rectángulo de la hitbox
                sf::RectangleShape rectDebug(bounds.size);
                rectDebug.setPosition(bounds.position);
                rectDebug.setFillColor(sf::Color::Transparent);
                rectDebug.setOutlineThickness(1.f);
                rectDebug.setOutlineColor(sf::Color::Magenta); 
                
                // 2. Una X para verificar el centro y alineación
                // CORRECCIÓN SFML 3.0: Usamos llaves {} para inicializar sf::Vertex
                sf::Vertex lineas[] = {
                    sf::Vertex{bounds.position, sf::Color::Magenta},
                    sf::Vertex{bounds.position + bounds.size, sf::Color::Magenta},
                    sf::Vertex{sf::Vector2f(bounds.position.x + bounds.size.x, bounds.position.y), sf::Color::Magenta},
                    sf::Vertex{sf::Vector2f(bounds.position.x, bounds.position.y + bounds.size.y), sf::Color::Magenta}
                };

                window.draw(rectDebug);
                window.draw(lineas, 4, sf::PrimitiveType::Lines);
            }
        }
    }
}