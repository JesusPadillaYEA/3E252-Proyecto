#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "BarcoEntity.hpp"

namespace RenderManager {

    // Añadimos parámetro 'rotacion' (grados)
    inline void renderizarFlota(sf::RenderWindow& window, const std::vector<BarcoEntity>& flota, 
                                int indiceSeleccionado, EstadoJuego estadoActual,
                                bool moviendo, const sf::Color& colorBorde, float rotacion = 0.f) {
        
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

            // Borde selección (también rotado si aplica)
            if ((int)i == indiceSeleccionado) {
                // Nota: Simplificación, el borde no rota aquí para no complicar el código de UI
                // Si rotamos la flota enemiga, normalmente no la estamos seleccionando
                sf::RectangleShape borde(barco.sprite.getGlobalBounds().size);
                borde.setPosition(barco.sprite.getPosition());
                borde.setFillColor(sf::Color::Transparent);
                borde.setOutlineThickness(2.f);
                borde.setOutlineColor(colorBorde);
                window.draw(borde);
            }
        }
    }
}