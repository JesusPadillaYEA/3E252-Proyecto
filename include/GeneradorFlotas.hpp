#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Jugador.hpp"
#include "BarcoEntity.hpp"

namespace GeneradorFlotas {

    // Función auxiliar para encontrar una posición válida sin colisiones
    inline sf::Vector2f encontrarPosicionValida(const std::vector<BarcoEntity>& flota, 
                                                 const sf::Texture& textura,
                                                 const sf::Vector2f& escala,
                                                 float mapWidth, float mapHeight,
                                                 int maxIntentosId) {
        int intentos = 0;
        const int MAX_INTENTOS = 50;
        
        while (intentos < MAX_INTENTOS) {
            // Generar posición aleatoria en toda la pantalla
            float x = static_cast<float>(std::rand() % (int)mapWidth);
            float y = static_cast<float>(std::rand() % (int)mapHeight);
            
            // Crear sprite temporal para verificar colisión y límites
            sf::Sprite tempSprite(textura);
            tempSprite.setScale(escala);
            tempSprite.setPosition({x, y});
            
            // Obtener bounds del nuevo sprite
            sf::FloatRect newBounds = tempSprite.getGlobalBounds();
            
            // Verificar si sale del mapa
            if (newBounds.position.x < 0.f || 
                newBounds.position.y < 0.f ||
                newBounds.position.x + newBounds.size.x > mapWidth ||
                newBounds.position.y + newBounds.size.y > mapHeight) {
                intentos++;
                continue;
            }
            
            // Verificar si colisiona con algún barco existente
            bool colisiona = false;
            for (const auto& barco : flota) {
                sf::FloatRect barcoBounds = barco.sprite.getGlobalBounds();
                
                // Expandir el área de colisión un poco para dejar espacio
                barcoBounds.position.x -= 20.f;
                barcoBounds.position.y -= 20.f;
                barcoBounds.size.x += 40.f;
                barcoBounds.size.y += 40.f;
                
                if (newBounds.findIntersection(barcoBounds)) {
                    colisiona = true;
                    break;
                }
            }
            
            // Si no colisiona y está dentro del mapa, retornar la posición
            if (!colisiona) {
                return {x, y};
            }
            
            intentos++;
        }
        
        // Si no encontramos posición después de MAX_INTENTOS, retornar posición por defecto (centro)
        return {mapWidth / 2.f, mapHeight / 2.f};
    }

    inline void inicializarFlota(Jugador& jugador, 
                                 const sf::Texture& tDestructor, 
                                 const sf::Texture& tPortaviones,
                                 const sf::Texture& tSubmarino) {
        
        auto& flota = jugador.grid.flota;
        flota.clear();

        // Escalas fijas
        sf::Vector2f sDest = {0.3f, 0.3f};
        sf::Vector2f sPort = {0.5f, 0.5f};
        sf::Vector2f sSub  = {0.4f, 0.4f};

        if (jugador.id == 1) {
            // --- JUGADOR 1 ---
            // Generar en toda la pantalla (0 a 1000 en X, 0 a 700 en Y)
            
            // 1. Portaviones
            flota.emplace_back(tPortaviones, "Portaviones P1");
            flota.back().sprite.setScale(sPort);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tPortaviones, sPort, 1000.f, 700.f, 0));

            // 2. Submarino Alpha
            flota.emplace_back(tSubmarino, "Submarino Alpha P1");
            flota.back().sprite.setScale(sSub);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tSubmarino, sSub, 1000.f, 700.f, 1));

            // 3. Submarino Beta
            flota.emplace_back(tSubmarino, "Submarino Beta P1");
            flota.back().sprite.setScale(sSub);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tSubmarino, sSub, 1000.f, 700.f, 2));

            // 4. Destructor A
            flota.emplace_back(tDestructor, "Destructor A P1");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tDestructor, sDest, 1000.f, 700.f, 3));

            // 5. Destructor B
            flota.emplace_back(tDestructor, "Destructor B P1");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tDestructor, sDest, 1000.f, 700.f, 4));

            // 6. Destructor C
            flota.emplace_back(tDestructor, "Destructor C P1");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tDestructor, sDest, 1000.f, 700.f, 5));
        } 
        else {
            // --- JUGADOR 2 ---
            // Generar en toda la pantalla (0 a 1000 en X, 0 a 700 en Y)
            
            // 1. Portaviones
            flota.emplace_back(tPortaviones, "Portaviones P2");
            flota.back().sprite.setScale(sPort);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tPortaviones, sPort, 1000.f, 700.f, 0));

            // 2. Submarino Alpha
            flota.emplace_back(tSubmarino, "Submarino Alpha P2");
            flota.back().sprite.setScale(sSub);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tSubmarino, sSub, 1000.f, 700.f, 1));

            // 3. Submarino Beta
            flota.emplace_back(tSubmarino, "Submarino Beta P2");
            flota.back().sprite.setScale(sSub);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tSubmarino, sSub, 1000.f, 700.f, 2));

            // 4. Destructor A
            flota.emplace_back(tDestructor, "Destructor A P2");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tDestructor, sDest, 1000.f, 700.f, 3));

            // 5. Destructor B
            flota.emplace_back(tDestructor, "Destructor B P2");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tDestructor, sDest, 1000.f, 700.f, 4));

            // 6. Destructor C
            flota.emplace_back(tDestructor, "Destructor C P2");
            flota.back().sprite.setScale(sDest);
            flota.back().sprite.setPosition(encontrarPosicionValida(flota, tDestructor, sDest, 1000.f, 700.f, 5));

            // Color distintivo para P2
            for (auto& barco : flota) {
                barco.sprite.setColor({200, 200, 255});
            }
        }
    }
}