#pragma once

#include <SFML/Graphics.hpp>
#include "Jugador.hpp"
#include "TutorialSystem.hpp"
#include "ResourceManager.hpp"
#include "GameState.hpp"
#include "Boton.hpp"
#include "AttackManager.hpp"
#include "MovementManager.hpp"
#include <vector>

namespace TutorialScene {
    
    struct Scene {
        // Fondo
        sf::Sprite fondo;

        // Jugador y enemigos
        Jugador jugador;
        Jugador enemigo;
        
        // Tipos de barco disponibles
        sf::Texture *tDestroyer, *tPortaviones, *tSubmarine;
        
        // Índice del barco actual (jugador)
        int barcoSeleccionadoIdx = -1;
        
        // Estados para el juego
        bool moviendo = false;
        bool apuntando = false;
        bool cargandoDisparo = false;
        float potenciaActual = 0.f;
        sf::Vector2f posicionApunte = {500.f, 350.f};
        sf::Vector2f origenDisparoReal = {500.f, 700.f};
        
        // Animaciones
        std::vector<sf::CircleShape> explosiones;
        sf::Clock uavTimer;
        sf::Clock airstrikeTimer;
        bool lanzandoUAV = false;
        int faseAirstrike = 0;  // 0=inactivo, 1=preparando, 2=volando
        sf::Vector2f posAvion;
        
        // Botones
        Boton btnAtacar;
        Boton btnMover;
        Boton btnRadar;
        Boton btnNotas;
        
        // Estados deUI
        bool radarActivo = false;
        bool notasActivas = false;
        
        // Vector de bajas reportadas (para explosiones)
        std::vector<sf::Vector2f> bajasReportadas;
        
        Scene(sf::Texture& tD, sf::Texture& tP, sf::Texture& tS, sf::Texture& tMar,
              const sf::Font& font)
            : fondo(tMar), jugador(1, {0.f, 0.f}), enemigo(2, {700.f, 0.f}),
              tDestroyer(&tD), tPortaviones(&tP), tSubmarine(&tS),
              btnAtacar(font, "ATACAR", {200, 50, 50}),
              btnMover(font, "MOVER", {50, 150, 50}),
              btnRadar(font, "RADAR", sf::Color(60, 70, 80)),
              btnNotas(font, "NOTAS", sf::Color(139, 100, 60)) {
            
            // Configurar fondo
            fondo.setTexture(tMar);
            
            // Inicializar flotas separadas para tutorial (mitades del mapa)
            inicializarFlotaTutorial(jugador, tD, tP, tS);
            inicializarFlotaTutorial(enemigo, tD, tP, tS);
            
            // Posicionar botones fijos (RADAR y NOTAS)
            btnRadar.setPosition({850.f, 20.f});
            btnNotas.setPosition({730.f, 20.f});
        }
        
        // Actualizar posición de botones al lado del barco seleccionado
        void actualizarPosicionBotones() {
            if (barcoSeleccionadoIdx != -1 && !jugador.getFlota()[barcoSeleccionadoIdx].destruido) {
                auto& barco = jugador.getFlota()[barcoSeleccionadoIdx];
                auto bounds = barco.sprite.getGlobalBounds();
                float yBtn = barco.sprite.getPosition().y + bounds.size.y + 10.f;
                btnAtacar.setPosition({barco.sprite.getPosition().x, yBtn});
                btnMover.setPosition({barco.sprite.getPosition().x + 110.f, yBtn});
            }
        }
        
    private:
        // Inicializar flotas separadas para tutorial
        void inicializarFlotaTutorial(Jugador& j, const sf::Texture& tD, const sf::Texture& tP, const sf::Texture& tS) {
            auto& flota = j.grid.flota;
            flota.clear();

            sf::Vector2f sDest = {0.3f, 0.3f};
            sf::Vector2f sPort = {0.5f, 0.5f};
            sf::Vector2f sSub  = {0.4f, 0.4f};

            float mapWidth = 500.f;  // Solo mitad del mapa
            float mapHeight = 700.f;
            float offsetX = (j.id == 1) ? 0.f : 500.f;  // J1: 0-500, J2: 500-1000

            // Portaviones
            flota.emplace_back(tP, j.id == 1 ? "Portaviones P1" : "Portaviones P2");
            flota.back().sprite.setScale(sPort);
            sf::Vector2f pos = GeneradorFlotas::encontrarPos icionValida(flota, tP, sPort, mapWidth, mapHeight, 0);
            flota.back().sprite.setPosition({pos.x + offsetX, pos.y});

            // Submarinos
            for (int i = 0; i < 2; ++i) {
                std::string nombre = j.id == 1 ? "Submarino Alpha P1" : "Submarino Alpha P2";
                if (i == 1) nombre = j.id == 1 ? "Submarino Beta P1" : "Submarino Beta P2";
                flota.emplace_back(tS, nombre);
                flota.back().sprite.setScale(sSub);
                pos = GeneradorFlotas::encontrarPosicionValida(flota, tS, sSub, mapWidth, mapHeight, i+1);
                flota.back().sprite.setPosition({pos.x + offsetX, pos.y});
            }

            // Destructores
            for (int i = 0; i < 3; ++i) {
                std::string nombre = j.id == 1 ? "Destructor A P1" : "Destructor A P2";
                if (i == 1) nombre = j.id == 1 ? "Destructor B P1" : "Destructor B P2";
                if (i == 2) nombre = j.id == 1 ? "Destructor C P1" : "Destructor C P2";
                flota.emplace_back(tD, nombre);
                flota.back().sprite.setScale(sDest);
                pos = GeneradorFlotas::encontrarPosicionValida(flota, tD, sDest, mapWidth, mapHeight, i+3);
                flota.back().sprite.setPosition({pos.x + offsetX, pos.y});
            }

            // Color distintivo para P2
            if (j.id == 2) {
                for (auto& barco : flota) {
                    barco.sprite.setColor({200, 200, 255});
                }
            }
        }
    };
    
    // === RENDERIZACIÓN ===
    inline void renderizarEscena(sf::RenderWindow& window, Scene& scene, 
                                  ResourceManager::Resources& recursos,
                                  GameState::StateData& gameState,
                                  sf::Vector2u windowSize) {
        
        // Fondo
        window.draw(scene.fondo);
        
        // Dibujar barcos del jugador
        for (auto& barco : scene.jugador.getFlota()) {
            window.draw(barco.sprite);
        }
        
        // Dibujar barcos enemigos
        for (auto& barco : scene.enemigo.getFlota()) {
            window.draw(barco.sprite);
        }
        
        // Explosiones
        for (auto& exp : scene.explosiones) {
            window.draw(exp);
        }
        
        // Línea de apunte si está apuntando
        if (scene.apuntando && scene.barcoSeleccionadoIdx != -1) {
            sf::Vector2f pos1 = scene.jugador.getFlota()[scene.barcoSeleccionadoIdx].sprite.getPosition();
            sf::Vertex line[] = {
                sf::Vertex{{pos1.x, pos1.y}, sf::Color::Cyan},
                sf::Vertex{{scene.posicionApunte.x, scene.posicionApunte.y}, sf::Color::Cyan}
            };
            window.draw(line, 2, sf::PrimitiveType::LineStrip);
        }
        
        // Dibujar botones
        scene.btnAtacar.dibujar(window);
        scene.btnMover.dibujar(window);
        scene.btnRadar.dibujar(window);
        scene.btnNotas.dibujar(window);
        
        // === INSTRUCCIÓN ===
        std::string instruccion = TutorialSystem::obtenerInstruccion(gameState.datosTutorial.pasoActual);
        
        sf::Text txtInstruccion(recursos.font);
        txtInstruccion.setString(instruccion);
        txtInstruccion.setCharacterSize(20);
        txtInstruccion.setFillColor(sf::Color::White);
        txtInstruccion.setOutlineThickness(2.f);
        txtInstruccion.setOutlineColor(sf::Color::Black);
        
        // Posicionar en la parte inferior
        sf::FloatRect bounds = txtInstruccion.getLocalBounds();
        txtInstruccion.setPosition({
            ((float)windowSize.x - bounds.size.x) / 2.f,
            (float)windowSize.y - 140.f
        });
        
        // Fondo semi-transparente
        sf::RectangleShape fondoInstruccion({(float)windowSize.x, 140.f});
        fondoInstruccion.setFillColor(sf::Color(0, 0, 0, 200));
        fondoInstruccion.setPosition({0.f, (float)(windowSize.y - 140)});
        window.draw(fondoInstruccion);
        window.draw(txtInstruccion);
        
        // Mostrar paso actual
        sf::Text txtPaso(recursos.font);
        int pasoNum = (int)gameState.datosTutorial.pasoActual + 1;
        int totalPasos = 12;
        txtPaso.setString("Paso " + std::to_string(pasoNum) + " de " + std::to_string(totalPasos));
        txtPaso.setCharacterSize(18);
        txtPaso.setFillColor(sf::Color::Yellow);
        txtPaso.setPosition({20.f, 640.f});
        window.draw(txtPaso);
    }
    
    // === PROCESAMIENTO DE EVENTOS ===
    inline void procesarEventosTutorial(const sf::Event* ev, Scene& scene, 
                                        GameState::StateData& gameState,
                                        ResourceManager::Resources& recursos,
                                        sf::Vector2u windowSize) {
        
        // TECLAS
        if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            // MOVIMIENTO con flechas
            if (gameState.datosTutorial.pasoActual == TutorialSystem::MOVIMIENTO && 
                scene.barcoSeleccionadoIdx != -1 && scene.moviendo) {
                
                sf::Vector2f delta(0, 0);
                if (k->code == sf::Keyboard::Key::Up)    delta.y = -5.f;
                if (k->code == sf::Keyboard::Key::Down)  delta.y = 5.f;
                if (k->code == sf::Keyboard::Key::Left)  delta.x = -5.f;
                if (k->code == sf::Keyboard::Key::Right) delta.x = 5.f;
                
                if (delta.x != 0 || delta.y != 0) {
                    bool moved = MovementManager::procesarInput(scene.jugador.getFlota(),
                                                                 scene.barcoSeleccionadoIdx,
                                                                 windowSize);
                    if (moved) {
                        gameState.datosTutorial.barcoMovido = true;
                        // Auto-avanzar cuando se mueve el barco
                        TutorialSystem::avanzarPaso(gameState.datosTutorial);
                    }
                }
            }
            
            // ENTER para avanzar en pasos específicos
            if (k->code == sf::Keyboard::Key::Enter) {
                if (gameState.datosTutorial.pasoActual == TutorialSystem::INICIO) {
                    TutorialSystem::avanzarPaso(gameState.datosTutorial);
                }
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::COMPLETADO) {
                    // Fin del tutorial
                }
            }
        }
        
        // MOUSE
        if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos = sf::Vector2f(m->position);
                
                // PASO 1: SELECCIONAR BARCO
                if (gameState.datosTutorial.pasoActual == TutorialSystem::SELECCIONAR_BARCO) {
                    for (size_t i = 0; i < scene.jugador.getFlota().size(); ++i) {
                        if (scene.jugador.getFlota()[i].sprite.getGlobalBounds().contains(mousePos)) {
                            scene.barcoSeleccionadoIdx = i;
                            gameState.datosTutorial.barcoSeleccionado = true;
                            TutorialSystem::avanzarPaso(gameState.datosTutorial);
                            return;
                        }
                    }
                }
                
                // PASO 9: PORTAVIONES_CAMBIO - Detectar selección de portaviones
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::PORTAVIONES_CAMBIO) {
                    for (size_t i = 0; i < scene.jugador.getFlota().size(); ++i) {
                        auto& barco = scene.jugador.getFlota()[i];
                        if (barco.sprite.getGlobalBounds().contains(mousePos) && !barco.destruido) {
                            if (barco.nombre.find("Portaviones") != std::string::npos) {
                                scene.barcoSeleccionadoIdx = i;
                                gameState.datosTutorial.portavionesActivo = true;
                                TutorialSystem::avanzarPaso(gameState.datosTutorial);
                                return;
                            }
                        }
                    }
                }
                
                // PASO 2: MOVIMIENTO - Activar modo movimiento
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::MOVIMIENTO) {
                    if (scene.barcoSeleccionadoIdx != -1) {
                        if (scene.btnMover.esClickeado(mousePos)) {
                            recursos.sButton.play();
                            scene.moviendo = !scene.moviendo;
                            if (scene.moviendo) {
                                scene.btnMover.setColor({255, 140, 0});
                            } else {
                                scene.btnMover.resetColor();
                            }
                        }
                    }
                }
                
                // PASO 3: ATACAR - Click en botón ATACAR
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::ATAQUE_SELECTOR) {
                    if (scene.barcoSeleccionadoIdx != -1 && scene.btnAtacar.esClickeado(mousePos)) {
                        recursos.sButton.play();
                        scene.apuntando = true;
                        scene.btnAtacar.setColor({255, 69, 0});
                        gameState.datosTutorial.ataqueSeleccionado = true;
                        TutorialSystem::avanzarPaso(gameState.datosTutorial);
                        return;
                    }
                }
                
                // PASO 4: APUNTAR - Mover mouse hacia enemigos
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::ATAQUE_APUNTAR) {
                    if (scene.apuntando) {
                        // Detectar si el apunte es hacia la zona enemiga
                        sf::FloatRect zonaEnemiga({600.f, 50.f}, {400.f, 600.f});
                        if (zonaEnemiga.contains(mousePos)) {
                            gameState.datosTutorial.disparoApuntado = true;
                            TutorialSystem::avanzarPaso(gameState.datosTutorial);
                            return;
                        }
                    }
                }
                
                // PASO 5: CARGAR DISPARO
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::ATAQUE_CARGAR) {
                    if (scene.apuntando) {
                        scene.cargandoDisparo = true;
                        gameState.datosTutorial.disparioCargado = true;
                        TutorialSystem::avanzarPaso(gameState.datosTutorial);
                        return;
                    }
                }
            }
            else if (m->button == sf::Mouse::Button::Right) {
                // CANCELAR
                if (scene.apuntando || scene.cargandoDisparo) {
                    scene.apuntando = false;
                    scene.cargandoDisparo = false;
                    scene.potenciaActual = 0.f;
                    scene.btnAtacar.resetColor();
                }
            }
        }
        
        // MOUSE MOVE - Actualizar posición de apunte
        if (const auto* m = ev->getIf<sf::Event::MouseMoved>()) {
            scene.posicionApunte = sf::Vector2f(m->position);
        }
        
        // MOUSE BUTTON RELEASED - Disparar
        if (const auto* m = ev->getIf<sf::Event::MouseButtonReleased>()) {
            if (m->button == sf::Mouse::Button::Left && scene.cargandoDisparo) {
                // PASO 6: DISPARAR
                if (gameState.datosTutorial.pasoActual == TutorialSystem::ATAQUE_DISPARAR) {
                    sf::Vector2f mousePos = sf::Vector2f(m->position);
                    
                    // Procesar disparo
                    bool acertado = AttackManager::procesarDisparo(mousePos,
                        scene.jugador.getFlota()[scene.barcoSeleccionadoIdx].sprite.getPosition(),
                        scene.enemigo,
                        scene.bajasReportadas);
                    
                    if (acertado) {
                        recursos.sShotDone.play();
                    } else {
                        recursos.sShotFail.play();
                    }
                    
                    // Crear explosión visual
                    sf::CircleShape explosion(15.f);
                    explosion.setFillColor(acertado ? sf::Color::Red : sf::Color::Yellow);
                    explosion.setPosition({mousePos.x - 15.f, mousePos.y - 15.f});
                    scene.explosiones.push_back(explosion);
                    
                    gameState.datosTutorial.disparoRealizado = true;
                    scene.cargandoDisparo = false;
                    scene.apuntando = false;
                    scene.btnAtacar.resetColor();
                    
                    TutorialSystem::avanzarPaso(gameState.datosTutorial);
                    return;
                }
            }
        }
        
        // RADAR - PASO 7
        if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos = sf::Vector2f(m->position);
                
                if (gameState.datosTutorial.pasoActual == TutorialSystem::RADAR_LANCIAR) {
                    if (scene.btnRadar.esClickeado(mousePos)) {
                        recursos.sButton.play();
                        recursos.sRadar.play();
                        scene.radarActivo = !scene.radarActivo;
                        if (scene.radarActivo) {
                            scene.btnRadar.setColor(sf::Color::Green);
                            gameState.datosTutorial.radarLanzado = true;
                            TutorialSystem::avanzarPaso(gameState.datosTutorial);
                        }
                        return;
                    }
                }
                
                // NOTAS - PASO 8
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::NOTAS_VER) {
                    if (scene.btnNotas.esClickeado(mousePos)) {
                        recursos.sButton.play();
                        recursos.sNotas.play();
                        scene.notasActivas = true;
                        scene.btnNotas.setColor(sf::Color::White);
                        gameState.datosTutorial.notasVistas = true;
                        scene.timerNotas.restart();
                        return;
                    }
                }
                
                // AIR STRIKE - PASO 10-11
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::AIRSTRIKE_SELECTOR) {
                    if (scene.barcoSeleccionadoIdx != -1) {
                        auto& barco = scene.jugador.getFlota()[scene.barcoSeleccionadoIdx];
                        if (barco.nombre.find("Portaviones") != std::string::npos && 
                            scene.btnAtacar.esClickeado(mousePos)) {
                            
                            recursos.sButton.play();
                            recursos.sAirStrike.play();
                            gameState.datosTutorial.airStrikeSeleccionado = true;
                            scene.btnAtacar.setColor(sf::Color::Red);
                            TutorialSystem::avanzarPaso(gameState.datosTutorial);
                            return;
                        }
                    }
                }
                
                // AIR STRIKE DISPARO
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::AIRSTRIKE_DISPARO) {
                    sf::FloatRect zonaEnemiga({600.f, 50.f}, {400.f, 600.f});
                    if (zonaEnemiga.contains(mousePos)) {
                        // Procesar ataque aéreo
                        AttackManager::procesarAtaqueAereo(mousePos.x, scene.enemigo, scene.bajasReportadas);
                        recursos.sAirStrike.play();
                        
                        // Crear explosión más grande
                        sf::CircleShape explosion(25.f);
                        explosion.setFillColor(sf::Color(255, 165, 0));
                        explosion.setPosition({mousePos.x - 25.f, mousePos.y - 25.f});
                        scene.explosiones.push_back(explosion);
                        
                        gameState.datosTutorial.airStrikeRealizado = true;
                        scene.btnAtacar.resetColor();
                        
                        TutorialSystem::avanzarPaso(gameState.datosTutorial);
                        return;
                    }
                }
            }
        }
    }
    
    // === ACTUALIZACIÓN DEL TUTORIAL ===
    inline void actualizarTutorial(Scene& scene, GameState::StateData& gameState) {
        // Auto-avance para NOTAS_VER después de 2 segundos
        if (gameState.datosTutorial.pasoActual == TutorialSystem::NOTAS_VER && 
            scene.notasActivas && 
            scene.timerNotas.getElapsedTime().asSeconds() > 2.0f) {
            scene.notasActivas = false;
            scene.btnNotas.resetColor();
            TutorialSystem::avanzarPaso(gameState.datosTutorial);
        }
    }
}

