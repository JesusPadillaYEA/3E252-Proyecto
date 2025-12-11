#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "BarcoEntity.hpp"
#include "CollisionManager.hpp"

namespace MovementManager {

    // Retorna true si hubo movimiento efectivo
    inline bool procesarInput(std::vector<BarcoEntity>& flota, int seleccionado) {
        if (seleccionado == -1) return false;

        float velocidad = 3.0f;
        sf::Vector2f delta(0.f, 0.f);

        // Lectura de Teclado
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) delta.x += velocidad;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  delta.x -= velocidad;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    delta.y -= velocidad;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  delta.y += velocidad;

        // Si no se presionó nada, salimos
        if (delta.x == 0 && delta.y == 0) return false;

        // Verificar Colisión antes de mover
        if (!CollisionManager::verificar(flota[seleccionado].sprite, delta, flota, seleccionado)) {
            flota[seleccionado].sprite.move(delta);
            return true;
        }
        
        return false; // Hubo intención pero colisionó
    }
}