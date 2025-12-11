#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "BarcoEntity.hpp"

namespace CollisionManager {

    inline bool verificar(sf::Sprite& spriteMoviendose, sf::Vector2f desplazamiento, 
                          const std::vector<BarcoEntity>& flota, int indiceIgnorar, 
                          sf::Vector2u mapSize) {
        
        sf::FloatRect futuro = spriteMoviendose.getGlobalBounds();
        futuro.position += desplazamiento;

        // 1. Limites del Mapa (Igual que antes)
        if (futuro.position.x < 0.f || futuro.position.y < 0.f) return true; 
        if (futuro.position.x + futuro.size.x > (float)mapSize.x) return true;
        if (futuro.position.y + futuro.size.y > (float)mapSize.y) return true;

        // 2. Colisión con otros barcos
        for (size_t i = 0; i < flota.size(); ++i) {
            if ((int)i == indiceIgnorar) continue;

            // --- NUEVA LÓGICA ---
            // Si el otro barco está destruido, no colisionamos (pasamos por encima)
            if (flota[i].destruido) continue;

            sf::FloatRect otro = flota[i].sprite.getGlobalBounds();
            
            bool chocaX = futuro.position.x < otro.position.x + otro.size.x &&
                          futuro.position.x + futuro.size.x > otro.position.x;
            bool chocaY = futuro.position.y < otro.position.y + otro.size.y &&
                          futuro.position.y + futuro.size.y > otro.position.y;

            if (chocaX && chocaY) return true;
        }

        return false;
    }
}