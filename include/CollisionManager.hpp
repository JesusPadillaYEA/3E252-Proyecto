#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "BarcoEntity.hpp"

// Usamos namespace para organizar mejor
namespace CollisionManager {

    // Función inline para verificar colisiones futuras
    inline bool verificar(sf::Sprite& spriteMoviendose, sf::Vector2f desplazamiento, 
                          const std::vector<BarcoEntity>& flota, int indiceIgnorar) {
        
        // Calcular dónde estará el barco
        sf::FloatRect futuro = spriteMoviendose.getGlobalBounds();
        futuro.position += desplazamiento; // Sintaxis SFML 3.0

        // Comparar contra todos los demás barcos
        for (size_t i = 0; i < flota.size(); ++i) {
            if ((int)i == indiceIgnorar) continue;

            sf::FloatRect otro = flota[i].sprite.getGlobalBounds();
            
            // Lógica AABB (Axis-Aligned Bounding Box)
            bool chocaX = futuro.position.x < otro.position.x + otro.size.x &&
                          futuro.position.x + futuro.size.x > otro.position.x;
            bool chocaY = futuro.position.y < otro.position.y + otro.size.y &&
                          futuro.position.y + futuro.size.y > otro.position.y;

            if (chocaX && chocaY) return true;
        }
        return false;
    }
}