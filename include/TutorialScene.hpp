#pragma once

#include <SFML/Graphics.hpp>
#include "Jugador.hpp"
#include "TutorialSystem.hpp"
#include "ResourceManager.hpp"
#include "GameState.hpp"
#include "Boton.hpp"
#include "AttackManager.hpp"
#include "MovementManager.hpp"
#include "RenderManager.hpp"
#include "IndicatorManager.hpp"
#include "GeneradorFlotas.hpp"
#include "RenderSystem.hpp"
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
        
        // Estados del tutorial
        bool moviendo = false;
        bool apuntando = false;
        bool cargandoDisparo = false;
        float potenciaActual = 0.f;
        sf::Vector2f posicionApunte = {500.f, 350.f};
        sf::Vector2f origenDisparoReal = {500.f, 700.f};
        
        // Animaciones
        std::vector<AnimationSystem::ExplosionVisual> explosiones;
        sf::Clock uavTimer;
        sf::Clock airstrikeTimer;
        bool lanzandoUAV = false;
        int faseAirstrike = 0;  // 0=inactivo, 1=preparando, 2=volando, 3=esperando click
        sf::Vector2f posUAV;      // Posición actual del UAV
        sf::Vector2f posAvion;    // Posición actual del avión
        float escalaUAV = 0.05f;  // Escala del UAV
        float escalaAvion = 0.05f; // Escala del avión
        sf::CircleShape uavShapeFallback;  // Fallback si no hay textura
        float colObjetivoX = 500.f;  // Columna objetivo para airstrike
        
        // Botones
        Boton btnAtacar;
        Boton btnMover;
        Boton btnRadar;
        Boton btnNotas;
        
        // Estados de UI
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
            
            // Configurar fallback UAV
            uavShapeFallback.setRadius(15.f);
            uavShapeFallback.setFillColor(sf::Color(100, 200, 255));
            posUAV = {500.f, 400.f};
            posAvion = {500.f, 400.f};
            
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
            sf::Vector2f pos = GeneradorFlotas::encontrarPosicionValida(flota, tP, sPort, mapWidth, mapHeight, 0);
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
        
        // Determinar color del borde según estado
        sf::Color colorBorde = sf::Color(50, 150, 50);
        if (scene.moviendo) colorBorde = sf::Color(255, 140, 0);
        if (scene.apuntando) colorBorde = sf::Color::Red;
        
        // Usar RenderManager para dibujar flota del jugador con resaltado
        RenderManager::renderizarFlota(window, scene.jugador.getFlota(), 
                                       scene.barcoSeleccionadoIdx, 
                                       TURNO_P1, scene.moviendo, colorBorde);
        
        // Dibujar barcos enemigos sin resaltado
        for (auto& barco : scene.enemigo.getFlota()) {
            if (!barco.destruido) window.draw(barco.sprite);
        }
        
        // Explosiones animadas
        for (const auto& exp : scene.explosiones) {
            window.draw(exp.sprite);
        }
        
        // UAV si está volando
        if (scene.lanzandoUAV) {
            if (recursos.uavLoaded) {
                sf::Sprite uavSprite(recursos.tUAV);
                uavSprite.setPosition(scene.posUAV);
                uavSprite.setScale({scene.escalaUAV, scene.escalaUAV});
                auto bounds = uavSprite.getLocalBounds();
                uavSprite.setOrigin({bounds.size.x/2.f, bounds.size.y/2.f});
                window.draw(uavSprite);
            } else {
                scene.uavShapeFallback.setPosition(scene.posUAV);
                window.draw(scene.uavShapeFallback);
            }
        }
        
        // Avión de airstrike si está activo
        if (scene.faseAirstrike > 0 && scene.faseAirstrike < 3) {
            if (recursos.avionLoaded) {
                sf::Sprite avionSprite(recursos.tAvionAtaque);
                avionSprite.setPosition(scene.posAvion);
                avionSprite.setScale({scene.escalaAvion, scene.escalaAvion});
                auto bounds = avionSprite.getLocalBounds();
                avionSprite.setOrigin({bounds.size.x/2.f, bounds.size.y/2.f});
                window.draw(avionSprite);
            }
        }
        
        // Flecha de apunte si está apuntando (usando IndicatorManager)
        if (scene.apuntando && scene.barcoSeleccionadoIdx != -1) {
            auto bounds = scene.jugador.getFlota()[scene.barcoSeleccionadoIdx].sprite.getGlobalBounds();
            sf::Vector2f centroBarco = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
            
            float pot = scene.cargandoDisparo ? scene.potenciaActual : 50.f;
            IndicatorManager::dibujarVectorApuntado(window, centroBarco, scene.posicionApunte, pot, recursos.font);
        }
        
        // Dibujar botones (ATACAR y MOVER si hay barco seleccionado)
        if (scene.barcoSeleccionadoIdx != -1 && !scene.jugador.getFlota()[scene.barcoSeleccionadoIdx].destruido &&
            !scene.lanzandoUAV && scene.faseAirstrike == 0) {
            scene.btnAtacar.dibujar(window);
            scene.btnMover.dibujar(window);
        }
        
        // Botones fijos
        if (!scene.lanzandoUAV && scene.faseAirstrike == 0) {
            scene.btnRadar.dibujar(window);
            scene.btnNotas.dibujar(window);
        }
        
        // Mostrar radar si está activo
        if (scene.radarActivo) {
            RenderSystem::RadarVisuals radarVis = RenderSystem::inicializarRadar(windowSize);
            
            // Actualizar memoria del radar con posiciones enemigas
            scene.jugador.memoriaRadar.clear();
            for (const auto& barco : scene.enemigo.getFlota()) {
                if (!barco.destruido) {
                    sf::FloatRect b = barco.sprite.getGlobalBounds();
                    scene.jugador.memoriaRadar.push_back({b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f});
                }
            }
            
            RenderSystem::dibujarRadar(window, recursos.font, radarVis, scene.jugador, scene.btnRadar, windowSize);
        }
        
        // Mostrar notas si están activas
        if (scene.notasActivas) {
            RenderSystem::NotasVisuals notasVis = RenderSystem::inicializarNotas(windowSize);
            RenderSystem::dibujarNotas(window, recursos.font, notasVis, scene.jugador, scene.enemigo, scene.btnNotas, windowSize);
        }
        
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
        int pasoNum = (int)gameState.datosTutorial.pasoActual;
        int totalPasos = 9;
        txtPaso.setString("Paso " + std::to_string(pasoNum) + " de " + std::to_string(totalPasos));
        txtPaso.setCharacterSize(18);
        txtPaso.setFillColor(sf::Color::Yellow);
        txtPaso.setPosition({20.f, 640.f});
        window.draw(txtPaso);
        
        // Menú de pausa si está activo
        if (gameState.juegoPausado) {
            RenderSystem::dibujarMenuPausa(window, recursos.font, gameState, windowSize);
        }
    }
    
    // === PROCESAMIENTO DE EVENTOS ===
    inline void procesarEventosTutorial(const sf::Event* ev, Scene& scene, 
                                        GameState::StateData& gameState,
                                        ResourceManager::Resources& recursos,
                                        sf::Vector2u windowSize) {
        
        // Ignorar eventos si hay animaciones activas
        if (scene.lanzandoUAV || scene.faseAirstrike > 0) return;
        
        // TECLAS
        if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            // ENTER para pasos específicos
            if (k->code == sf::Keyboard::Key::Enter) {
                if (gameState.datosTutorial.pasoActual == TutorialSystem::INICIO) {
                    TutorialSystem::avanzarPaso(gameState.datosTutorial);
                }
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::MENU_PAUSA && 
                         gameState.datosTutorial.menuPausaVisto) {
                    TutorialSystem::avanzarPaso(gameState.datosTutorial);
                }
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::COMPLETADO) {
                    // Fin del tutorial
                }
            }
            
            // ESC para menú de pausa
            if (k->code == sf::Keyboard::Key::Escape) {
                if (gameState.datosTutorial.pasoActual == TutorialSystem::MENU_PAUSA) {
                    gameState.juegoPausado = !gameState.juegoPausado;
                    if (!gameState.juegoPausado && !gameState.datosTutorial.menuPausaVisto) {
                        gameState.datosTutorial.menuPausaVisto = true;
                    }
                }
            }
            
            // FLECHAS para movimiento
            if (gameState.datosTutorial.pasoActual == TutorialSystem::MOVIMIENTO && 
                scene.moviendo && scene.barcoSeleccionadoIdx != -1) {
                MovementManager::procesarInput(scene.jugador.getFlota(), scene.barcoSeleccionadoIdx, windowSize);
            }
        }
        
        // MOUSE
        if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos = sf::Vector2f(m->position);
                
                // PASO: SELECCIONAR_BARCO
                if (gameState.datosTutorial.pasoActual == TutorialSystem::SELECCIONAR_BARCO) {
                    for (size_t i = 0; i < scene.jugador.getFlota().size(); ++i) {
                        if (scene.jugador.getFlota()[i].sprite.getGlobalBounds().contains(mousePos) &&
                            !scene.jugador.getFlota()[i].destruido) {
                            scene.barcoSeleccionadoIdx = i;
                            gameState.datosTutorial.barcoSeleccionado = true;
                            TutorialSystem::avanzarPaso(gameState.datosTutorial);
                            return;
                        }
                    }
                }
                
                // PASO: MOVIMIENTO - Toggle
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::MOVIMIENTO) {
                    if (scene.barcoSeleccionadoIdx != -1 && scene.btnMover.esClickeado(mousePos)) {
                        recursos.sButton.play();
                        scene.moviendo = !scene.moviendo;
                        if (scene.moviendo) {
                            scene.btnMover.setColor({255, 140, 0});
                        } else {
                            scene.btnMover.resetColor();
                            gameState.datosTutorial.movimientoToggled = true;
                            TutorialSystem::avanzarPaso(gameState.datosTutorial);
                        }
                        return;
                    }
                }
                
                // PASO: ATAQUE (unificado)
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::ATAQUE) {
                    // Click en ATACAR para entrar en modo apuntar
                    if (!scene.apuntando && scene.barcoSeleccionadoIdx != -1 && 
                        scene.btnAtacar.esClickeado(mousePos)) {
                        recursos.sButton.play();
                        scene.apuntando = true;
                        scene.btnAtacar.setColor({255, 69, 0});
                        return;
                    }
                    
                    // Iniciar carga si ya está apuntando
                    if (scene.apuntando) {
                        scene.cargandoDisparo = true;
                        scene.potenciaActual = 0.f;
                        // Calcular origen del disparo
                        auto bounds = scene.jugador.getFlota()[scene.barcoSeleccionadoIdx].sprite.getGlobalBounds();
                        sf::Vector2f centroBarco = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
                        scene.origenDisparoReal = {centroBarco.x, (float)windowSize.y};
                    }
                }
                
                // PASO: RADAR
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::RADAR) {
                    if (scene.btnRadar.esClickeado(mousePos)) {
                        recursos.sButton.play();
                        
                        if (!scene.lanzandoUAV && !scene.radarActivo) {
                            // Iniciar lanzamiento UAV
                            scene.lanzandoUAV = true;
                            recursos.sUAV.play();
                            scene.uavTimer.restart();
                            
                            // Inicializar posición del UAV desde el portaviones
                            bool encontradoPortaviones = false;
                            for (const auto& barco : scene.jugador.getFlota()) {
                                if (barco.nombre.find("Portaviones") != std::string::npos && !barco.destruido) {
                                    auto bounds = barco.sprite.getGlobalBounds();
                                    scene.posUAV = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
                                    encontradoPortaviones = true;
                                    break;
                                }
                            }
                            if (!encontradoPortaviones) {
                                scene.posUAV = {250.f, 400.f};  // Lado izquierdo como fallback
                            }
                            scene.escalaUAV = 0.05f;
                        } else if (scene.radarActivo) {
                            // Cerrar radar
                            scene.radarActivo = false;
                            scene.btnRadar.resetColor();
                            recursos.sRadar.stop();
                            gameState.datosTutorial.radarCerrado = true;
                            TutorialSystem::avanzarPaso(gameState.datosTutorial);
                        }
                        return;
                    }
                }
                
                // PASO: NOTAS_VER
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::NOTAS_VER) {
                    if (scene.btnNotas.esClickeado(mousePos)) {
                        recursos.sButton.play();
                        
                        if (!scene.notasActivas) {
                            scene.notasActivas = true;
                            scene.btnNotas.setColor(sf::Color::White);
                            recursos.sNotas.play();
                        } else {
                            scene.notasActivas = false;
                            scene.btnNotas.resetColor();
                            gameState.datosTutorial.notasCerradas = true;
                            TutorialSystem::avanzarPaso(gameState.datosTutorial);
                        }
                        return;
                    }
                }
                
                // PASO: PORTAVIONES_CAMBIO
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
                
                // PASO: AIRSTRIKE - Auto-avanza después de animación
                else if (gameState.datosTutorial.pasoActual == TutorialSystem::AIRSTRIKE) {
                    if (scene.faseAirstrike == 0 && scene.barcoSeleccionadoIdx != -1) {
                        auto& barco = scene.jugador.getFlota()[scene.barcoSeleccionadoIdx];
                        if (barco.nombre.find("Portaviones") != std::string::npos && 
                            scene.btnAtacar.esClickeado(mousePos)) {
                            recursos.sButton.play();
                            recursos.sAirStrike.play();
                            scene.faseAirstrike = 1;
                            scene.airstrikeTimer.restart();
                            
                            auto bounds = barco.sprite.getGlobalBounds();
                            scene.posAvion = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
                            scene.escalaAvion = 0.05f;
                            scene.colObjetivoX = 750.f;  // Centro de zona enemiga
                            
                            scene.btnAtacar.setColor(sf::Color::Red);
                            return;
                        }
                    }
                    // La animación avanza automáticamente en actualizarTutorial
                }
            }
        }
        
        // MOUSE MOVE - Actualizar apunte
        if (const auto* m = ev->getIf<sf::Event::MouseMoved>()) {
            scene.posicionApunte = sf::Vector2f(m->position);
        }
        
        
        // MOUSE RELEASED - Disparar (paso ATAQUE)
        if (const auto* m = ev->getIf<sf::Event::MouseButtonReleased>()) {
            if (m->button == sf::Mouse::Button::Left && scene.cargandoDisparo && scene.apuntando) {
                if (gameState.datosTutorial.pasoActual == TutorialSystem::ATAQUE) {
                    sf::Vector2f mousePos = sf::Vector2f(m->position);
                    
                    // Procesar disparo
                    bool acertado = AttackManager::procesarDisparo(mousePos,
                        scene.origenDisparoReal, scene.enemigo, scene.bajasReportadas);
                    
                    if (acertado) recursos.sShotDone.play();
                    else recursos.sShotFail.play();
                    
                    // Crear explosión animada
                    if (recursos.animOk && !recursos.animExplosionTex.empty()) {
                        AnimationSystem::ExplosionVisual ex(recursos.animExplosionTex[0]);
                        ex.sprite.setPosition(mousePos);
                        scene.explosiones.push_back(ex);
                    }
                    
                    scene.cargandoDisparo = false;
                    scene.apuntando = false;
                    scene.btnAtacar.resetColor();
                    gameState.datosTutorial.ataqueCompletado = true;
                    TutorialSystem::avanzarPaso(gameState.datosTutorial);
                }
            }
        }
    }
    
    
    // === ACTUALIZACIÓN DEL TUTORIAL ===
    inline void actualizarTutorial(Scene& scene, GameState::StateData& gameState, ResourceManager::Resources& recursos) {
        // Actualizar posición de botones
        scene.actualizarPosicionBotones();
        
        // Actualizar carga de disparo
        if (scene.cargandoDisparo) {
            scene.potenciaActual += 5.f;
            if (scene.potenciaActual > 1500.f) scene.potenciaActual = 1500.f;
        }
        
        // Actualizar explosiones animadas
        float deltaTime = 1.f / 60.f;  // Aproximado
        AnimationSystem::actualizarExplosiones(scene.explosiones, recursos.animExplosionTex, deltaTime, recursos.animOk);
        
        // Animación UAV
        if (scene.lanzandoUAV) {
            if (scene.uavTimer.getElapsedTime().asMilliseconds() > 500) {
                if (recursos.uavLoaded) {
                    scene.posUAV.y -= 3.0f;
                    if (scene.escalaUAV < 0.15f) scene.escalaUAV += 0.0005f;
                    
                    if (scene.posUAV.y < -50.f) {
                        scene.lanzandoUAV = false;
                        scene.radarActivo = true;
                        scene.btnRadar.setColor(sf::Color::Red);
                        recursos.sRadar.play();
                        recursos.sUAV.stop();
                    }
                } else {
                    scene.posUAV.y -= 3.0f;
                    if (scene.posUAV.y < -50.f) {
                        scene.lanzandoUAV = false;
                        scene.radarActivo = true;
                        scene.btnRadar.setColor(sf::Color::Red);
                        recursos.sRadar.play();
                    }
                }
            }
        }
        
        // Animación Airstrike
        if (scene.faseAirstrike > 0) {
            if (scene.faseAirstrike == 1) {
                // Fase de preparación (parpadeo - 1 segundo)
                if (scene.airstrikeTimer.getElapsedTime().asSeconds() > 1.0f) {
                    scene.faseAirstrike = 2;
                } else if (recursos.avionLoaded) {
                    // Efecto de parpadeo
                    scene.escalaAvion = 0.05f + (std::sin(scene.airstrikeTimer.getElapsedTime().asMilliseconds() * 0.01f) * 0.002f);
                }
            }
            else if (scene.faseAirstrike == 2) {
                // Avión despegando y volando
                scene.posAvion.y -= 4.0f;
                if (scene.escalaAvion < 0.25f) scene.escalaAvion += 0.001f;
                
                bool avionFuera = (scene.posAvion.y < -100.f);
                bool sonidoTerminado = (recursos.sAirStrike.getStatus() == sf::Sound::Status::Stopped);
                
                if (avionFuera || sonidoTerminado) {
                    // Ejecutar ataque automáticamente
                    std::vector<sf::Vector2f> bajasAereas;
                    AttackManager::procesarAtaqueAereo(scene.colObjetivoX, scene.enemigo, bajasAereas);
                    
                    // Crear explosiones animadas
                    if (recursos.animOk && !recursos.animExplosionTex.empty()) {
                        for (const auto& pos : bajasAereas) {
                            AnimationSystem::ExplosionVisual ex(recursos.animExplosionTex[0]);
                            ex.sprite.setPosition(pos);
                            scene.explosiones.push_back(ex);
                        }
                    }
                    
                    scene.faseAirstrike = 0;
                    scene.btnAtacar.resetColor();
                    gameState.datosTutorial.airstrikeCompletado = true;
                    TutorialSystem::avanzarPaso(gameState.datosTutorial);
                }
            }
        }
    }
}
