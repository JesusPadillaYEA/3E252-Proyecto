#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream> // Necesario para imprimir errores
#include "BarcoEntity.hpp"

namespace CollisionManager {

    inline bool verificar(sf::Sprite& spriteMoviendose, sf::Vector2f desplazamiento, 
                          const std::vector<BarcoEntity>& flota, int indiceIgnorar, 
                          sf::Vector2u mapSize) {
        
        // 1. Calcular rectángulo futuro
        sf::FloatRect futuro = spriteMoviendose.getGlobalBounds();
        futuro.position += desplazamiento;

        // --- DEBUG: Ver tamaño del barco ---
        // Descomenta esto si quieres ver el tamaño real de tus barcos
        // std::cout << "Barco Tamano: " << futuro.size.x << "x" << futuro.size.y << std::endl;

        // 2. CHEQUEO DE LÍMITES DEL MAPA
        if (futuro.position.x < 0.f) {
            std::cout << "[BLOQUEADO] Choca izquierda" << std::endl;
            return true; 
        }
        if (futuro.position.y < 0.f) {
            std::cout << "[BLOQUEADO] Choca arriba" << std::endl;
            return true; 
        }
        if (futuro.position.x + futuro.size.x > (float)mapSize.x) {
            std::cout << "[BLOQUEADO] Choca derecha (Limite mapa)" << std::endl;
            return true;
        }
        if (futuro.position.y + futuro.size.y > (float)mapSize.y) {
            std::cout << "[BLOQUEADO] Choca abajo (Limite mapa)" << std::endl;
            return true;
        }

        // 3. CHEQUEO CON OTROS BARCOS
        for (size_t i = 0; i < flota.size(); ++i) {
            if ((int)i == indiceIgnorar) continue;

            sf::FloatRect otro = flota[i].sprite.getGlobalBounds();
            
            bool chocaX = futuro.position.x < otro.position.x + otro.size.x &&
                          futuro.position.x + futuro.size.x > otro.position.x;
            bool chocaY = futuro.position.y < otro.position.y + otro.size.y &&
                          futuro.position.y + futuro.size.y > otro.position.y;

            if (chocaX && chocaY) {
                std::cout << "[BLOQUEADO] Choca con barco aliado: " << flota[i].nombre << std::endl;
                return true;
            }
        }

        return false; // Camino libre
    }
}