#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string> // Necesario para to_string
#include "Jugador.hpp"

namespace IndicatorManager {

    // --- (Funciones de Pistas / Radar se mantienen igual) ---
    inline void agregarPista(Jugador& victima, sf::Vector2f origenDisparo, sf::Vector2f puntoImpacto) {
        FlechaPista nueva = {origenDisparo, puntoImpacto, 1};
        victima.pistas.push_back(nueva);
    }

    inline void actualizarTurnos(Jugador& jugadorActual) {
        auto& lista = jugadorActual.pistas;
        lista.erase(std::remove_if(lista.begin(), lista.end(),
            [](FlechaPista& f) {
                f.turnosRestantes--; 
                return f.turnosRestantes < 0; 
            }), lista.end());
    }

    inline void renderizarPistas(sf::RenderWindow& window, const Jugador& jugadorActual, bool esAtaque) {
        if (jugadorActual.pistas.empty()) return;
        sf::Vector2f winSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
        sf::Vector2f centroRadar = esAtaque ? sf::Vector2f(winSize.x/2.f, winSize.y-50.f) : sf::Vector2f(winSize.x/2.f, 50.f);

        for (const auto& flecha : jugadorActual.pistas) {
            sf::Vector2f posEnemigoVisual = {winSize.x - flecha.origen.x, winSize.y - flecha.origen.y};
            sf::Vector2f vectorDireccion = posEnemigoVisual - centroRadar;
            float angulo = std::atan2(vectorDireccion.y, vectorDireccion.x) * 180.f / 3.14159f;

            float largo = 80.f;
            sf::Color color = esAtaque ? sf::Color(255, 165, 0) : sf::Color(255, 50, 50);

            sf::RectangleShape cuerpo({largo, 4.f});
            cuerpo.setOrigin({0.f, 2.f});
            cuerpo.setPosition(centroRadar);
            cuerpo.setRotation(sf::degrees(angulo));
            cuerpo.setFillColor(color);

            sf::CircleShape cabeza(10.f, 3);
            cabeza.setOrigin({10.f, 10.f});
            float rad = angulo * 3.14159f / 180.f;
            cabeza.setPosition(centroRadar + sf::Vector2f(std::cos(rad)*largo, std::sin(rad)*largo));
            cabeza.setRotation(sf::degrees(angulo + 90.f)); 
            cabeza.setFillColor(color);

            sf::CircleShape pivote(5.f);
            pivote.setOrigin({5.f, 5.f});
            pivote.setPosition(centroRadar);
            pivote.setFillColor(sf::Color::White);

            window.draw(cuerpo);
            window.draw(cabeza);
            window.draw(pivote);
        }
    }

    // --- NUEVO: VISUALIZACIÓN DE CARGA ---
    inline void dibujarVectorApuntado(sf::RenderWindow& window, sf::Vector2f origen, sf::Vector2f mousePos, float potencia, const sf::Font& font) {
        
        // Calcular ángulo hacia el mouse
        sf::Vector2f diff = mousePos - origen;
        float anguloRad = std::atan2(diff.y, diff.x);
        float anguloGrad = anguloRad * 180.f / 3.14159f;

        // 1. Flecha de Dirección (Tamaño constante)
        float largoFlecha = 60.f;
        sf::RectangleShape linea({largoFlecha, 4.f});
        linea.setOrigin({0.f, 2.f});
        linea.setPosition(origen);
        linea.setRotation(sf::degrees(anguloGrad));
        linea.setFillColor(sf::Color(255, 165, 0)); // Naranja

        sf::CircleShape cabeza(10.f, 3);
        cabeza.setOrigin({10.f, 10.f});
        cabeza.setPosition(origen + sf::Vector2f(std::cos(anguloRad)*largoFlecha, std::sin(anguloRad)*largoFlecha));
        cabeza.setRotation(sf::degrees(anguloGrad + 90.f));
        cabeza.setFillColor(sf::Color(255, 69, 0));

        // 2. Mira de Impacto (Se mueve con la potencia)
        // Calculamos dónde caerá el proyectil
        sf::Vector2f impacto = origen + sf::Vector2f(std::cos(anguloRad) * potencia, std::sin(anguloRad) * potencia);

        sf::CircleShape mira(15.f);
        mira.setOrigin({15.f, 15.f});
        mira.setPosition(impacto);
        mira.setFillColor(sf::Color::Transparent);
        mira.setOutlineThickness(2.f);
        mira.setOutlineColor(sf::Color::Red);

        sf::CircleShape punto(3.f);
        punto.setOrigin({3.f, 3.f});
        punto.setPosition(impacto);
        punto.setFillColor(sf::Color::Red);

        // 3. Texto Contador de Distancia
        sf::Text texto(font);
        texto.setString(std::to_string((int)potencia) + "m");
        texto.setCharacterSize(18);
        texto.setFillColor(sf::Color::White);
        texto.setOutlineColor(sf::Color::Black);
        texto.setOutlineThickness(1.f);
        texto.setPosition(impacto + sf::Vector2f(20.f, -20.f));

        window.draw(linea);
        window.draw(cabeza);
        window.draw(mira);
        window.draw(punto);
        window.draw(texto);
    }
}