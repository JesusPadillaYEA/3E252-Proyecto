#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstdint>

// Headers del proyecto
#include "EstadoJuego.hpp"
#include "Boton.hpp"
#include "CollisionManager.hpp"
#include "MovementManager.hpp"
#include "UIManager.hpp"
#include "Jugador.hpp"
#include "GeneradorFlotas.hpp"
#include "RenderManager.hpp"
#include "AttackManager.hpp"
#include "IndicatorManager.hpp"

// Headers del nuevo sistema modular
#include "GameState.hpp"
#include "ResourceManager.hpp"
#include "AudioManager.hpp"
#include "InputHandler.hpp"
#include "AnimationSystem.hpp"
#include "RenderSystem.hpp"
#include "TutorialScene.hpp"
#include <filesystem>
#include <vector>

// Función auxiliar para ejecutar un programa verificando múltiples rutas posibles
void ejecutarPrograma(const std::vector<std::string>& rutas) {
    for (const auto& ruta : rutas) {
        if (std::filesystem::exists(ruta)) {
            system(("start " + ruta).c_str());
            return;
        }
    }
}

void ejecutarLauncher(sf::RenderWindow& window) {
    window.close();
    // Intenta ejecutar JuegoProyecto.exe en múltiples ubicaciones posibles
    ejecutarPrograma({"./JuegoProyecto.exe", "./bin/JuegoProyecto.exe", "bin\\JuegoProyecto.exe", "JuegoProyecto.exe"});
}

int main(int argc, char* argv[]) {
    const sf::Vector2u WINDOW_SIZE(1000, 700);
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Bitacora (SFML 3.0)");
    window.setFramerateLimit(60);

    // === VERIFICAR ARGUMENTOS (TUTORIAL) ===
    bool modoTutorial = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--tutorial") {
            modoTutorial = true;
            break;
        }
    }

    // === CARGAR RECURSOS ===
    ResourceManager::Resources recursos = ResourceManager::cargarRecursos();
    if (!recursos.assetsOk) return -1;

    // === ESTADO DEL JUEGO ===
    GameState::StateData gameState;
    gameState.enTutorial = modoTutorial;
    if (modoTutorial) TutorialSystem::resetearTutorial(gameState.datosTutorial);
    AudioManager::AudioState audioState;

    // === ESCENA DEL TUTORIAL (si aplica) ===
    TutorialScene::Scene* scenaTutorial = nullptr;
    if (modoTutorial) {
        scenaTutorial = new TutorialScene::Scene(recursos.tDest, recursos.tPort, recursos.tSub, recursos.tMar, recursos.font);
    }

    // === VARIABLES PARA ANIMACIONES ===
    std::vector<AnimationSystem::ExplosionVisual> explosionesActivas;
    std::vector<AnimationSystem::FuegoVisual> fuegosActivos;
    std::vector<AnimationSystem::Particula> sistemaParticulas;

    // === VISUALES DEL RADAR Y NOTAS ===
    RenderSystem::RadarVisuals radarVisuals = RenderSystem::inicializarRadar(WINDOW_SIZE);
    RenderSystem::NotasVisuals notasVisuals = RenderSystem::inicializarNotas(WINDOW_SIZE);

    // === FONDO ANIMADO ===
    sf::Sprite fondo(recursos.tMar);
    float offset[2] = {0.f, 0.f};
    fondo.setTextureRect(sf::IntRect({0, 0}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

    // === SPRITE DEL UAV ===
    sf::Sprite uavSprite(recursos.tUAV);
    sf::CircleShape uavShapeFallback(5.f, 3);
    if (recursos.uavLoaded) {
        sf::FloatRect uavBounds = uavSprite.getLocalBounds();
        uavSprite.setOrigin({uavBounds.size.x / 2.f, uavBounds.size.y / 2.f});
    } else {
        uavShapeFallback.setFillColor(sf::Color::Cyan);
        uavShapeFallback.setOrigin({5.f, 5.f});
    }

    // === SPRITE DEL AVIÓN DE ATAQUE ===
    sf::Sprite sAvionAtaque(recursos.tAvionAtaque);
    if (recursos.avionLoaded) {
        sf::FloatRect bounds = sAvionAtaque.getLocalBounds();
        sAvionAtaque.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    }

    // === INICIAR MÚSICA ===
    recursos.musicaFondo.play();

    // === BOTONES ===
    Boton btnAtacar(recursos.font, "ATACAR", {200, 50, 50});
    Boton btnMover(recursos.font, "MOVER", {50, 150, 50});
    Boton btnRadar(recursos.font, "RADAR", sf::Color(60, 70, 80));
    btnRadar.setPosition({850.f, 20.f});
    Boton btnNotas(recursos.font, "NOTAS", sf::Color(139, 100, 60));
    btnNotas.setPosition({730.f, 20.f});

    sf::Text txtInfo(recursos.font);
    txtInfo.setCharacterSize(20);
    txtInfo.setOutlineThickness(2.f);
    txtInfo.setFillColor(sf::Color::Yellow);
    txtInfo.setPosition({350.f, 20.f});

    // === ZONAS DE ATAQUE ===
    InputHandler::MapZones zonas;

    // === JUGADORES ===
    Jugador p1(1, {0.f, 0.f});
    Jugador p2(2, {700.f, 0.f});

    GeneradorFlotas::inicializarFlota(p1, recursos.tDest, recursos.tPort, recursos.tSub);
    GeneradorFlotas::inicializarFlota(p2, recursos.tDest, recursos.tPort, recursos.tSub);

    // === TIMERS ===
    sf::Clock uavTimer;
    sf::Clock timerAtaqueAereo;
    sf::Clock dtClock;

    // === VARIABLES PARA ATAQUES ===
    sf::Vector2f posAvionAtaque;
    float colObjetivoX = 0.f;
    const float velocidadAvion = 4.0f;
    const float VELOCIDAD_CARGA = 5.0f;
    const float MAX_DISTANCIA = 1500.f;

    // === CICLO PRINCIPAL ===
    // === LOOP DEL TUTORIAL (TOTALMENTE INDEPENDIENTE) ===
    if (gameState.enTutorial && scenaTutorial) {
        while (window.isOpen()) {
            while (const auto ev = window.pollEvent()) {
                if (ev->is<sf::Event::Closed>()) {
                    window.close();
                    break;
                }
                
                TutorialScene::procesarEventosTutorial(&(*ev), *scenaTutorial, gameState, recursos, WINDOW_SIZE);
                
                // Detectar si completó el tutorial con ENTER
                if (gameState.datosTutorial.pasoActual == TutorialSystem::COMPLETADO) {
                    if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                        if (k->code == sf::Keyboard::Key::Enter) {
                            window.close();
                            break;
                        }
                    }
                }
            }
            
            // Actualizar lógica del tutorial (timers, auto-avances, animaciones)
            TutorialScene::actualizarTutorial(*scenaTutorial, gameState, recursos);
            
            // Renderizar escena de tutorial
            TutorialScene::renderizarEscena(window, *scenaTutorial, recursos, gameState, WINDOW_SIZE);
            window.display();
        }
        
        // Tutorial completado, volver al programa principal
        if (scenaTutorial) delete scenaTutorial;
        ejecutarPrograma({"./JuegoProyecto.exe", "./bin/JuegoProyecto.exe", "bin\\JuegoProyecto.exe", "JuegoProyecto.exe"});
        return 0;
    }

    // === LOOP DEL JUEGO PRINCIPAL ===
    while (window.isOpen()) {
        float deltaTime = dtClock.restart().asSeconds();

        // Actualizar referencias de jugadores
        Jugador* jugadorActual = nullptr;
        Jugador* jugadorEnemigo = nullptr;

        if (gameState.estado == TURNO_P1) {
            jugadorActual = &p1;
            jugadorEnemigo = &p2;
        } else if (gameState.estado == TURNO_P2) {
            jugadorActual = &p2;
            jugadorEnemigo = &p1;
        }

        // === PROCESAMIENTO DE EVENTOS ===
        while (const auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (gameState.juegoPausado) {
                    // Procesar cualquier tecla del menú
                    InputHandler::procesarMenuPausa(k, gameState, recursos, recursos.musicaFondo);
                } else if (k->code == sf::Keyboard::Key::Escape) {
                    if (gameState.idGanador != 0) {
                        ejecutarLauncher(window);
                        return 0;
                    }
                    InputHandler::procesarEscape(gameState, recursos, btnAtacar, btnMover, btnRadar, btnNotas);
                }
            }

            // Ignorar clicks si pausa o animación en curso
            if (gameState.juegoPausado) continue;
            if (gameState.lanzandoUAV || gameState.lanzandoUAVRefuerzo || gameState.faseAtaqueAereo > 0 ||
                gameState.faseDisparoNormal > 0 || gameState.idGanador != 0 || gameState.ganadorDetectado != 0) {
                continue;
            }

            // === PANTALLA DE MENSAJES (TRANSICIÓN DE TURNO) ===
            if (gameState.estado == MENSAJE_P1 || gameState.estado == MENSAJE_P2) {
                if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) {
                    EstadoJuego siguienteEstado = (gameState.estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                    Jugador* siguienteJugador = (siguienteEstado == TURNO_P1) ? &p1 : &p2;

                    // Limpiar fuegos del turno anterior
                    fuegosActivos.clear();

                    // Generar fuego si hay zona bombardeada
                    if (siguienteJugador->columnaFuegoX > 0.f) {
                        int cantidadFuego = 40;
                        float alturaTramo = (float)WINDOW_SIZE.y / cantidadFuego;
                        
                        for (int i = 0; i < cantidadFuego; ++i) {
                            float yBase = i * alturaTramo;
                            float variacionY = static_cast<float>(std::rand() % (int)alturaTramo);
                            float yFinal = yBase + variacionY;
                            float offsetX = (std::rand() % 50) - 25.f;
                            
                            sf::Vector2f pos = {siguienteJugador->columnaFuegoX + offsetX, yFinal};
                            AnimationSystem::FuegoVisual fuego(recursos.animFuegoTex[0], pos);
                            fuego.sprite.setTexture(recursos.animFuegoTex[fuego.currentFrame]);
                            fuegosActivos.push_back(fuego);
                        }
                    }

                    // Procesar explosiones pendientes
                    if (!siguienteJugador->explosionesPendientes.empty()) {
                        recursos.sDestruccion.play();
                        
                        for (const auto& pos : siguienteJugador->explosionesPendientes) {
                            AnimationSystem::ExplosionVisual ex(recursos.animExplosionTex[0]);
                            sf::FloatRect bounds = ex.sprite.getLocalBounds();
                            ex.sprite.setOrigin({bounds.size.x/2.f, bounds.size.y/2.f});
                            ex.sprite.setPosition(pos);
                            ex.sprite.setScale({1.0f, 1.0f});
                            explosionesActivas.push_back(ex);
                        }
                        siguienteJugador->explosionesPendientes.clear();
                    }

                    // Verificar victoria
                    if (GameState::verificarVictoria(*siguienteJugador)) {
                        gameState.ganadorDetectado = (siguienteJugador->id == 1) ? 2 : 1;
                        gameState.timerVictoria = 4.0f;
                    }

                    // Reducir cooldowns
                    if (siguienteJugador->cooldownRadar > 0) siguienteJugador->cooldownRadar--;
                    if (siguienteJugador->cooldownAtaqueAereo > 0) siguienteJugador->cooldownAtaqueAereo--;

                    // Limpiar franja roja del jugador anterior
                    Jugador* jugadorAnterior = (siguienteEstado == TURNO_P1) ? &p2 : &p1;
                    jugadorAnterior->columnaFuegoX = -1.f;

                    // Manejar refuerzo de UAV pendiente
                    if (siguienteJugador->radarRefuerzoPendiente) {
                        gameState.lanzandoUAVRefuerzo = true;
                        recursos.sUAV.play();
                        siguienteJugador->radarRefuerzoPendiente = false;
                        siguienteJugador->cooldownRadar = 2;
                        
                        sf::Vector2f posInicio = {(float)WINDOW_SIZE.x / 2.f, (float)WINDOW_SIZE.y + 100.f};
                        if (recursos.uavLoaded) {
                            uavSprite.setPosition(posInicio);
                            uavSprite.setRotation(sf::degrees(0));
                            uavSprite.setScale({0.15f, 0.15f});
                        } else {
                            uavShapeFallback.setPosition(posInicio);
                        }
                    }

                    gameState.estado = siguienteEstado;
                    gameState.sel = -1;
                    gameState.moviendo = false;
                    gameState.apuntando = false;
                    gameState.cargandoDisparo = false;
                    gameState.modoRadar = false;
                    gameState.modoNotas = false;
                    gameState.lanzandoUAV = false;
                    // NO resetear lanzandoUAVRefuerzo aquí - se maneja en la lógica de animación
                    gameState.errorTimer = 0.f;
                    btnMover.resetColor();
                    btnAtacar.resetColor();
                    btnRadar.resetColor();
                    btnNotas.resetColor();
                    continue;
                }
                continue;
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    
                    if (!gameState.apuntando && !gameState.cargandoDisparo && jugadorActual) {
                        // Botón RADAR
                        if (btnRadar.esClickeado(mousePos) && !gameState.modoNotas) {
                            recursos.sButton.play();
                            if (!gameState.modoRadar) {
                                if (jugadorActual->radarRefuerzoPendiente) {
                                    gameState.msgError = "SOLICITUD EN PROCESO. REFUERZOS LLEGAN SIGUIENTE TURNO.";
                                    gameState.errorTimer = 2.0f;
                                } else if (jugadorActual->cooldownRadar > 0) {
                                    gameState.msgError = "SISTEMA ENFRIANDOSE. ESPERA " + std::to_string(jugadorActual->cooldownRadar) + " TURNO(S).";
                                    recursos.sError.play();
                                    gameState.errorTimer = 2.0f;
                                } else {
                                    bool carrierFound = false;
                                    sf::Vector2f posInicio;
                                    for (const auto& barco : jugadorActual->getFlota()) {
                                        if (barco.nombre.find("Portaviones") != std::string::npos && !barco.destruido) {
                                            sf::FloatRect b = barco.sprite.getGlobalBounds();
                                            posInicio = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};
                                            carrierFound = true;
                                            break;
                                        }
                                    }

                                    if (carrierFound) {
                                        if (recursos.uavLoaded) {
                                            uavSprite.setPosition(posInicio);
                                            uavSprite.setRotation(sf::degrees(0));
                                            uavSprite.setScale({0.05f, 0.05f});
                                        } else {
                                            uavShapeFallback.setPosition(posInicio);
                                        }
                                        gameState.lanzandoUAV = true;
                                        recursos.sUAV.play();
                                        uavTimer.restart();
                                        jugadorActual->cooldownRadar = 2;
                                        btnRadar.setColor(sf::Color::Red);
                                        gameState.sel = -1;
                                        gameState.moviendo = false;
                                    } else {
                                        jugadorActual->radarRefuerzoPendiente = true;
                                        gameState.msgError = "PORTAVIONES CAIDO. REFUERZOS LLEGAN EL PROXIMO TURNO.";
                                        gameState.errorTimer = 3.0f;
                                    }
                                }
                            } else {
                                gameState.modoRadar = false;
                                btnRadar.resetColor();
                                recursos.sRadar.stop();
                            }
                        }
                        // Botón NOTAS
                        else if (btnNotas.esClickeado(mousePos) && !gameState.modoRadar) {
                            recursos.sButton.play();
                            recursos.sNotas.play();
                            gameState.modoNotas = !gameState.modoNotas;
                            if (gameState.modoNotas) {
                                btnNotas.setColor(sf::Color::White);
                                gameState.sel = -1;
                                gameState.moviendo = false;
                            } else {
                                btnNotas.resetColor();
                            }
                        }
                    }

                    if (gameState.modoRadar || gameState.modoNotas) continue;

                    // MODO NORMAL
                    if (gameState.apuntando && gameState.sel != -1) {
                        gameState.cargandoDisparo = true;
                        gameState.potenciaActual = 0.f;
                    }

                    if (!gameState.apuntando && !gameState.cargandoDisparo) {
                        if (gameState.sel != -1) {
                            // Botón ATACAR
                            if (btnAtacar.esClickeado(mousePos)) {
                                recursos.sButton.play();
                                bool esPortaviones = (jugadorActual->getFlota()[gameState.sel].nombre.find("Portaviones") != std::string::npos);
                                
                                if (esPortaviones) {
                                    if (jugadorActual->cooldownAtaqueAereo > 0) {
                                        gameState.msgError = "ESCUADRON REPOSTANDO. ESPERA " + std::to_string(jugadorActual->cooldownAtaqueAereo) + " TURNO(S).";
                                        recursos.sError.play();
                                        gameState.errorTimer = 2.0f;
                                    } else {
                                        gameState.faseAtaqueAereo = 1;
                                        timerAtaqueAereo.restart();
                                        recursos.sAirStrike.play();
                                        
                                        auto bounds = jugadorActual->getFlota()[gameState.sel].sprite.getGlobalBounds();
                                        sf::Vector2f centro = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
                                        posAvionAtaque = centro;
                                        colObjetivoX = centro.x;
                                        
                                        if (recursos.avionLoaded) {
                                            sAvionAtaque.setPosition(posAvionAtaque);
                                            sAvionAtaque.setScale({0.05f, 0.05f});
                                            sAvionAtaque.setRotation(sf::degrees(0));
                                        }
                                        
                                        jugadorActual->cooldownAtaqueAereo = 5;
                                        btnAtacar.setColor(sf::Color::Red);
                                        gameState.sel = -1;
                                    }
                                } else {
                                    gameState.apuntando = true;
                                    gameState.moviendo = false;
                                    btnAtacar.setColor({255, 69, 0});
                                    btnMover.resetColor();
                                }
                            }
                            // Botón MOVER
                            else if (btnMover.esClickeado(mousePos)) {
                                recursos.sButton.play();
                                gameState.moviendo = !gameState.moviendo;
                                if (gameState.moviendo) {
                                    btnMover.setColor({255, 140, 0});
                                } else {
                                    btnMover.resetColor();
                                }
                            }
                        }

                        // Seleccionar barco
                        InputHandler::procesarSeleccionBarco(jugadorActual, mousePos, btnAtacar, btnMover, btnRadar, btnNotas, gameState);
                    }
                }
            }

            if (const auto* m = ev->getIf<sf::Event::MouseButtonReleased>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    if (gameState.cargandoDisparo && gameState.apuntando && gameState.sel != -1) {
                        gameState.cargandoDisparo = false;
                        sf::Vector2f diff = mousePos - gameState.origenDisparoReal;
                        float angulo = std::atan2(diff.y, diff.x);
                        sf::Vector2f impacto;
                        impacto.x = gameState.origenDisparoReal.x + std::cos(angulo) * gameState.potenciaActual;
                        impacto.y = gameState.origenDisparoReal.y + std::sin(angulo) * gameState.potenciaActual;

                        if (jugadorEnemigo) {
                            std::vector<sf::Vector2f> bajas;
                            bool acerto = AttackManager::procesarDisparo(impacto, gameState.origenDisparoReal, *jugadorEnemigo, bajas);
                            
                            if (!bajas.empty()) {
                                for (const auto& pos : bajas) {
                                    jugadorEnemigo->explosionesPendientes.push_back(pos);
                                }
                            }
                            
                            if (acerto) {
                                recursos.sShotDone.play();
                                audioState.sonidoDisparoActual = &recursos.sShotDone;
                            } else {
                                recursos.sShotFail.play();
                                audioState.sonidoDisparoActual = &recursos.sShotFail;
                            }
                            
                            gameState.faseDisparoNormal = 1;
                        }
                        IndicatorManager::actualizarTurnos(*jugadorActual);
                        gameState.apuntando = false;
                        gameState.sel = -1;
                        btnAtacar.resetColor();
                    }
                }
            }
        }

        // === UPDATE ===
        
        // Actualizar error timer
        if (gameState.errorTimer > 0.f) gameState.errorTimer -= deltaTime;

        // Animación UAV
        if (gameState.lanzandoUAV) {
            if (uavTimer.getElapsedTime().asMilliseconds() > 500) {
                if (recursos.uavLoaded) {
                    uavSprite.move({0.f, -3.0f});
                    sf::Vector2f scale = uavSprite.getScale();
                    if (scale.x < 0.15f) uavSprite.setScale({scale.x + 0.0005f, scale.y + 0.0005f});
                    if (uavSprite.getPosition().y < -50.f) {
                        gameState.lanzandoUAV = false;
                        gameState.modoRadar = true;
                        btnRadar.setColor(sf::Color::Red);
                        recursos.sRadar.play();
                        recursos.sUAV.stop();
                    }
                } else {
                    uavShapeFallback.move({0.f, -3.0f});
                    if (uavShapeFallback.getPosition().y < -50.f) {
                        gameState.lanzandoUAV = false;
                        gameState.modoRadar = true;
                        btnRadar.setColor(sf::Color::Red);
                    }
                }
            }
        } else if (gameState.lanzandoUAVRefuerzo) {
            if (recursos.uavLoaded) {
                uavSprite.move({0.f, -5.0f});
                if (uavSprite.getPosition().y < -50.f) {
                    gameState.lanzandoUAVRefuerzo = false;
                    gameState.modoRadar = true;
                    btnRadar.setColor(sf::Color::Red);
                    recursos.sRadar.play();
                    recursos.sUAV.stop();
                }
            } else {
                uavShapeFallback.move({0.f, -5.0f});
                if (uavShapeFallback.getPosition().y < -50.f) {
                    gameState.lanzandoUAVRefuerzo = false;
                    gameState.modoRadar = true;
                    btnRadar.setColor(sf::Color::Red);
                    recursos.sRadar.play();
                    recursos.sUAV.stop();
                }
            }
        }

        // Animación Air Strike
        if (gameState.faseAtaqueAereo > 0) {
            if (gameState.faseAtaqueAereo == 1) {
                if (timerAtaqueAereo.getElapsedTime().asSeconds() > 1.0f) {
                    gameState.faseAtaqueAereo = 2;
                } else {
                    float scale = 0.05f + (std::sin(timerAtaqueAereo.getElapsedTime().asMilliseconds() * 0.01f) * 0.002f);
                    sAvionAtaque.setScale({scale, scale});
                }
            } else if (gameState.faseAtaqueAereo == 2) {
                sAvionAtaque.move({0.f, -velocidadAvion});
                
                sf::Vector2f scale = sAvionAtaque.getScale();
                if (scale.x < 0.25f) sAvionAtaque.setScale({scale.x + 0.001f, scale.y + 0.001f});

                bool avionFuera = (sAvionAtaque.getPosition().y < -100.f);
                bool sonidoTerminado = (recursos.sAirStrike.getStatus() == sf::Sound::Status::Stopped);

                if (avionFuera && sonidoTerminado) {
                    if (jugadorEnemigo) {
                        std::vector<sf::Vector2f> bajasAereas;
                        AttackManager::procesarAtaqueAereo(colObjetivoX, *jugadorEnemigo, bajasAereas);
                        
                        if (!bajasAereas.empty()) {
                            for (const auto& pos : bajasAereas) {
                                jugadorEnemigo->explosionesPendientes.push_back(pos);
                            }
                        }
                    }
                    
                    gameState.faseAtaqueAereo = 0;
                    IndicatorManager::actualizarTurnos(*jugadorActual);
                    gameState.estado = (gameState.estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                }
            }
        } else if (gameState.faseDisparoNormal == 1) {
            if (audioState.sonidoDisparoActual && audioState.sonidoDisparoActual->getStatus() == sf::Sound::Status::Stopped) {
                gameState.faseDisparoNormal = 0;
                audioState.sonidoDisparoActual = nullptr;
                IndicatorManager::actualizarTurnos(*jugadorActual);
                gameState.estado = (gameState.estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
            }
        } else if (!gameState.modoRadar && !gameState.modoNotas) {
            offset[0] += 0.5f;
            offset[1] += 0.25f;
            if (offset[0] >= (float)WINDOW_SIZE.x) offset[0] = 0;
            if (offset[1] >= (float)WINDOW_SIZE.y) offset[1] = 0;
            fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));
        } else if (gameState.modoRadar && jugadorActual && jugadorEnemigo) {
            // Recopilar datos del radar
            jugadorActual->memoriaRadar.clear();
            for (const auto& barco : jugadorEnemigo->getFlota()) {
                if (!barco.destruido) {
                    sf::FloatRect b = barco.sprite.getGlobalBounds();
                    sf::Vector2f blipPos = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};
                    jugadorActual->memoriaRadar.push_back(blipPos);
                }
            }
            RenderSystem::actualizarRadar(radarVisuals, deltaTime);
        }

        if (gameState.cargandoDisparo) {
            gameState.potenciaActual += VELOCIDAD_CARGA;
            if (gameState.potenciaActual > MAX_DISTANCIA) gameState.potenciaActual = MAX_DISTANCIA;
        }

        // Actualizar timer de victoria
        GameState::actualizarVictoria(gameState, deltaTime);

        // Actualizar animaciones
        AnimationSystem::actualizarExplosiones(explosionesActivas, recursos.animExplosionTex, deltaTime, recursos.animOk);
        AnimationSystem::actualizarFuegos(fuegosActivos, recursos.animFuegoTex, deltaTime);

        if (gameState.idGanador != 0) {
            AnimationSystem::generarExplosion(sistemaParticulas, WINDOW_SIZE);
            AnimationSystem::actualizarParticulas(sistemaParticulas, deltaTime);
        }

        // Actualizar posición de barco seleccionado
        if (gameState.moviendo && gameState.sel != -1 && jugadorActual) {
            auto& barco = jugadorActual->getFlota()[gameState.sel];
            if (!barco.destruido) {
                MovementManager::procesarInput(jugadorActual->getFlota(), gameState.sel, WINDOW_SIZE);
            }
        }

        // Posicionar botones de acción
        if (gameState.sel != -1 && jugadorActual && !jugadorActual->getFlota()[gameState.sel].destruido) {
            auto& barco = jugadorActual->getFlota()[gameState.sel];
            auto bounds = barco.sprite.getGlobalBounds();
            sf::Vector2f p = barco.sprite.getPosition();
            float yBtn = p.y + bounds.size.y + 10.f;
            btnAtacar.setPosition({p.x, yBtn});
            btnMover.setPosition({p.x + 110.f, yBtn});
        }

        // Calcular posición de disparo
        sf::Vector2f impactoVisual(0, 0);
        if (gameState.apuntando && gameState.sel != -1 && jugadorActual) {
            auto bounds = jugadorActual->getFlota()[gameState.sel].sprite.getGlobalBounds();
            sf::Vector2f centroBarco = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            
            sf::FloatRect* zonaObjetivo = InputHandler::obtenerZonaObjetivo(gameState, jugadorActual, zonas);
            
            if (zonaObjetivo && zonaObjetivo->contains(mousePos)) {
                gameState.enZonaEnemiga = true;
                gameState.origenDisparoReal = {centroBarco.x, (float)WINDOW_SIZE.y};
            } else {
                gameState.enZonaEnemiga = false;
                gameState.origenDisparoReal = centroBarco;
            }

            float pot = gameState.cargandoDisparo ? gameState.potenciaActual : 50.f;
            sf::Vector2f diff = mousePos - gameState.origenDisparoReal;
            float ang = std::atan2(diff.y, diff.x);
            impactoVisual = gameState.origenDisparoReal + sf::Vector2f(std::cos(ang)*pot, std::sin(ang)*pot);
        }

        // === DRAW ===
        window.clear();

        if (gameState.estado == MENSAJE_P1 || gameState.estado == MENSAJE_P2) {
            window.draw(fondo);
            UIManager::dibujarTooltipTurno(window, recursos.font, gameState.estado);
        } else if (jugadorActual) {
            // Modo Radar
            if (gameState.modoRadar && jugadorEnemigo) {
                RenderSystem::dibujarRadar(window, recursos.font, radarVisuals, *jugadorActual, btnRadar, WINDOW_SIZE);
            }
            // Modo Notas
            else if (gameState.modoNotas) {
                RenderSystem::dibujarNotas(window, recursos.font, notasVisuals, *jugadorActual, *jugadorEnemigo, btnNotas, WINDOW_SIZE);
            }
            // Modo Normal
            else {
                window.draw(fondo);

                // Dibujar franja de fuego
                if (jugadorActual->columnaFuegoX > 0.f) {
                    txtInfo.setString("! ALERTA ! ZONA BOMBARDEADA");
                    txtInfo.setFillColor(sf::Color::Red);
                    txtInfo.setPosition({jugadorActual->columnaFuegoX + 40.f, 100.f});
                    window.draw(txtInfo);
                    for (const auto& fuego : fuegosActivos) {
                        window.draw(fuego.sprite);
                    }
                }

                if (gameState.enZonaEnemiga && gameState.apuntando) {
                    sf::RectangleShape overlay(sf::Vector2f((float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y));
                    overlay.setFillColor(sf::Color(255, 0, 0, 40));
                    window.draw(overlay);
                    txtInfo.setString("! ZONA ENEMIGA ! SUELTA PARA DISPARAR");
                    txtInfo.setFillColor(sf::Color::Red);
                    float textoY = impactoVisual.y - 40.f;
                    if (textoY < 10) textoY = impactoVisual.y + 20.f;
                    txtInfo.setPosition({impactoVisual.x + 20.f, textoY});
                    window.draw(txtInfo);
                } else {
                    sf::Color cBorde = gameState.moviendo ? sf::Color(255, 140, 0) : sf::Color(50, 150, 50);
                    if (gameState.apuntando) cBorde = sf::Color::Red;
                    
                    RenderManager::renderizarFlota(window, jugadorActual->getFlota(), gameState.sel, gameState.estado, gameState.moviendo, cBorde);

                    // Dibujar explosiones
                    for (const auto& ex : explosionesActivas) {
                        window.draw(ex.sprite);
                    }

                    if (gameState.apuntando) {
                        txtInfo.setString("Arrastra hacia territorio enemigo...");
                        txtInfo.setFillColor(sf::Color::Yellow);
                        txtInfo.setPosition({300.f, 20.f});
                        window.draw(txtInfo);
                    }

                    // Botones de acción
                    if (gameState.sel != -1 && !jugadorActual->getFlota()[gameState.sel].destruido && 
                        !gameState.cargandoDisparo && !gameState.lanzandoUAV && !gameState.lanzandoUAVRefuerzo && 
                        gameState.faseAtaqueAereo == 0) {
                        btnAtacar.dibujar(window);
                        btnMover.dibujar(window);
                    }
                }

                if (gameState.apuntando && gameState.sel != -1) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    float pot = gameState.cargandoDisparo ? gameState.potenciaActual : 50.f;
                    IndicatorManager::dibujarVectorApuntado(window, gameState.origenDisparoReal, mousePos, pot, recursos.font);
                }

                // Animación UAV
                if (gameState.lanzandoUAV) {
                    if (uavTimer.getElapsedTime().asMilliseconds() <= 500) {
                        txtInfo.setString("PREPARANDO DESPEGUE...");
                        txtInfo.setFillColor(sf::Color::White);
                        sf::Vector2f pos = recursos.uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                        txtInfo.setPosition({pos.x - 80.f, pos.y - 50.f});
                        window.draw(txtInfo);
                    } else {
                        txtInfo.setString("UAV EN RUTA...");
                        txtInfo.setFillColor(sf::Color::Cyan);
                        sf::Vector2f pos = recursos.uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                        txtInfo.setPosition({pos.x + 20.f, pos.y});
                        window.draw(txtInfo);
                    }
                    if (recursos.uavLoaded) window.draw(uavSprite);
                    else window.draw(uavShapeFallback);
                }

                if (gameState.lanzandoUAVRefuerzo) {
                    txtInfo.setString("REFUERZOS LLEGANDO...");
                    txtInfo.setFillColor(sf::Color::Green);
                    sf::Vector2f pos = recursos.uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                    txtInfo.setPosition({pos.x + 20.f, pos.y});
                    window.draw(txtInfo);
                    if (recursos.uavLoaded) window.draw(uavSprite);
                    else window.draw(uavShapeFallback);
                }

                // Animación Air Strike
                if (gameState.faseAtaqueAereo > 0 && recursos.avionLoaded) {
                    window.draw(sAvionAtaque);
                    if (gameState.faseAtaqueAereo == 1) {
                        txtInfo.setString("MOTORES EN MARCHA...");
                        txtInfo.setFillColor(sf::Color::Yellow);
                        txtInfo.setPosition({sAvionAtaque.getPosition().x + 20.f, sAvionAtaque.getPosition().y});
                        window.draw(txtInfo);
                    }
                }

                if (gameState.errorTimer > 0.f) {
                    txtInfo.setString(gameState.msgError);
                    txtInfo.setFillColor(sf::Color::Red);
                    txtInfo.setPosition({150.f, 300.f});
                    window.draw(txtInfo);
                }

                if (!gameState.apuntando && !gameState.cargandoDisparo && !gameState.lanzandoUAV && 
                    !gameState.lanzandoUAVRefuerzo && gameState.faseAtaqueAereo == 0) {
                    btnRadar.dibujar(window);
                    btnNotas.dibujar(window);
                }
            }
        }

        // Menú de pausa
        if (gameState.juegoPausado) {
            RenderSystem::dibujarMenuPausa(window, recursos.font, gameState, WINDOW_SIZE);
        }

        // Pantalla de victoria
        if (gameState.idGanador != 0) {
            RenderSystem::dibujarVictoria(window, recursos.font, gameState.idGanador, sistemaParticulas, WINDOW_SIZE);
        }

        // Salir a launcher
        if (gameState.salirAlLauncher) {
            window.close();
            // Intenta ejecutar JuegoProyecto.exe en múltiples ubicaciones posibles
            ejecutarPrograma({"./JuegoProyecto.exe", "./bin/JuegoProyecto.exe", "bin\\JuegoProyecto.exe", "JuegoProyecto.exe"});
            return 0;
        }

        window.display();
    }

    if (scenaTutorial) delete scenaTutorial;
    return 0;
}
