#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct BarcoEntity {
    sf::Sprite sprite;
    std::string nombre;

    // Constructor obligatorio para SFML 3.0
    BarcoEntity(const sf::Texture& texture, std::string name) 
        : sprite(texture), nombre(name) {
    }
};