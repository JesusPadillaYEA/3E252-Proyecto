#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "EstadoJuego.hpp"

namespace UIManager {

    inline void dibujarTooltipTurno(sf::RenderWindow& window, sf::Font& font, EstadoJuego estado) {
        sf::Vector2f winSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
        sf::Vector2f tooltipSize(500.f, 150.f);
        sf::Vector2f pos = {(winSize.x - tooltipSize.x) / 2.f, (winSize.y - tooltipSize.y) / 2.f};

        // Fondo
        sf::RectangleShape bg(tooltipSize);
        bg.setPosition(pos);
        bg.setFillColor(sf::Color(30, 30, 40, 220));
        bg.setOutlineThickness(3.f);
        bg.setOutlineColor((estado == MENSAJE_P1) ? sf::Color(100, 255, 100, 200) : sf::Color(100, 100, 255, 200));

        // Texto Principal
        sf::Text titulo(font);
        titulo.setString((estado == MENSAJE_P1) ? "TURNO JUGADOR 1" : "TURNO JUGADOR 2");
        titulo.setCharacterSize(40);
        titulo.setFillColor(sf::Color::White);
        
        auto bTit = titulo.getLocalBounds();
        titulo.setPosition({pos.x + (tooltipSize.x - bTit.size.x) / 2.f, pos.y + 30.f});

        // Subtexto
        sf::Text sub(font);
        sub.setString("Haz click para comenzar");
        sub.setCharacterSize(18);
        sub.setFillColor(sf::Color(200, 200, 200));
        
        auto bSub = sub.getLocalBounds();
        sub.setPosition({pos.x + (tooltipSize.x - bSub.size.x) / 2.f, pos.y + 90.f});

        window.draw(bg);
        window.draw(titulo);
        window.draw(sub);
    }
}