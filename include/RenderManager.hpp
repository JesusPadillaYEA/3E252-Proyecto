#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "BarcoEntity.hpp"
#include "Boton.hpp"

namespace RenderManager {

    // Renderiza solo los barcos que NO han sido destruidos
    inline void renderizarFlota(sf::RenderWindow& window, const std::vector<BarcoEntity>& flota, 
                                int indiceSeleccionado, EstadoJuego estadoActual,
                                bool moviendo, const sf::Color& colorBorde) {
        
        for (size_t i = 0; i < flota.size(); ++i) {
            const auto& barco = flota[i];

            // --- LÓGICA DEL GESTOR ---
            // Si el barco no existe (está destruido), lo ignoramos completamente.
            // No se dibuja, es como si se hubiera eliminado del juego.
            if (barco.destruido) continue;

            // Lógica de color (Selección vs Normal)
            // Hacemos una copia temporal del sprite para no modificar el original permanentemente en el render loop
            sf::Sprite dibujo = barco.sprite; 
            
            if ((int)i == indiceSeleccionado) {
                dibujo.setColor(sf::Color(255, 200, 200)); // Rojo claro si está seleccionado
            } else {
                // Mantenemos el color original (importante para diferenciar P1 de P2)
                dibujo.setColor(barco.sprite.getColor());
            }

            window.draw(dibujo);

            // Dibujar borde de selección si corresponde
            if ((int)i == indiceSeleccionado) {
                sf::RectangleShape borde(dibujo.getGlobalBounds().size);
                borde.setPosition(dibujo.getPosition());
                borde.setFillColor(sf::Color::Transparent);
                borde.setOutlineThickness(2.f);
                borde.setOutlineColor(colorBorde);
                window.draw(borde);
            }
        }
    }
}