#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "BarcoEntity.hpp"
#include "CollisionManager.hpp"

namespace MovementManager {

    // Añadimos 'mapSize' como parámetro
    inline bool procesarInput(std::vector<BarcoEntity>& flota, int seleccionado, sf::Vector2u mapSize) {
        if (seleccionado == -1) return false;

        float velocidad = 3.0f;
        sf::Vector2f delta(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) delta.x += velocidad;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  delta.x -= velocidad;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    delta.y -= velocidad;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  delta.y += velocidad;

        if (delta.x == 0 && delta.y == 0) return false;

        // Pasamos mapSize a la verificación
        if (!CollisionManager::verificar(flota[seleccionado].sprite, delta, flota, seleccionado, mapSize)) {
            flota[seleccionado].sprite.move(delta);
            return true;
        }
        
        return false;
    }
}