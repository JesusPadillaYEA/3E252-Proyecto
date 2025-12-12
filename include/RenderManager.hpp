#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "BarcoEntity.hpp"

namespace RenderManager {

    inline void renderizarFlota(sf::RenderWindow& window, const std::vector<BarcoEntity>& flota, 
                                int indiceSeleccionado, EstadoJuego estadoActual,
                                bool moviendo, const sf::Color& colorBorde, float rotacion = 0.f) {
        
        for (size_t i = 0; i < flota.size(); ++i) {
            const auto& barco = flota[i];
            if (barco.destruido) continue;

            sf::Sprite dibujo = barco.sprite; 
            
            // Rotación visual (si aplica)
            if (rotacion != 0.f) {
                sf::FloatRect bounds = dibujo.getLocalBounds();
                dibujo.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
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

            // --- CORRECCIÓN DE SELECCIÓN ---
            if ((int)i == indiceSeleccionado) {
                // Obtenemos los límites globales REALES (ya consideran rotación, escala y origen)
                sf::FloatRect bounds = barco.sprite.getGlobalBounds();
                
                sf::RectangleShape borde({bounds.size.x, bounds.size.y});
                // CORREGIDO: Usamos la posición de los límites, no la del sprite (que podría ser el centro)
                borde.setPosition({bounds.position.x, bounds.position.y});
                
                borde.setFillColor(sf::Color::Transparent);
                borde.setOutlineThickness(2.f);
                borde.setOutlineColor(colorBorde);
                window.draw(borde);
            }
        }
    }
}