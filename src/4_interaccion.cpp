#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string> // Necesario para to_string

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

int main() {
    const sf::Vector2u WINDOW_SIZE(1000, 700);
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Cooldown y Notas (SFML 3.0)");
    window.setFramerateLimit(60);

    // --- RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture tDest, tSub, tMar, tPort, tUAV;
    bool assetsOk = true;
    if (!tDest.loadFromFile("assets/images/destructor .png")) assetsOk = false;
    if (!tSub.loadFromFile("assets/images/submarino.png")) assetsOk = false;
    if (!tMar.loadFromFile("assets/images/mar.png")) assetsOk = false;
    if (!tPort.loadFromFile("assets/images/portaviones.png")) tPort = tDest; 
    
    bool uavLoaded = tUAV.loadFromFile("assets/images/UAV.png");
    if (!assetsOk) return -1;

    tMar.setRepeated(true);
    sf::Sprite fondo(tMar);
    float offset[2] = {0.f, 0.f};
    fondo.setTextureRect(sf::IntRect({0, 0}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

    // --- SPRITE DEL UAV ---
    sf::Sprite uavSprite(tUAV); 
    sf::CircleShape uavShapeFallback(5.f, 3);
    if (uavLoaded) {
        sf::FloatRect uavBounds = uavSprite.getLocalBounds();
        uavSprite.setOrigin({uavBounds.size.x / 2.f, uavBounds.size.y / 2.f});
    } else {
        uavShapeFallback.setFillColor(sf::Color::Cyan);
        uavShapeFallback.setOrigin({5.f, 5.f});
    }

    // --- JUGADORES ---
    Jugador p1(1, {0.f, 0.f});
    Jugador p2(2, {700.f, 0.f});

    GeneradorFlotas::inicializarFlota(p1, tDest, tPort, tSub);
    GeneradorFlotas::inicializarFlota(p2, tDest, tPort, tSub);

    // --- ZONAS ---
    sf::FloatRect zonaIzquierda({0.f, 0.f}, {500.f, 700.f});
    sf::FloatRect zonaDerecha({500.f, 0.f}, {500.f, 700.f});

    // --- UI ---
    Boton btnAtacar(font, "ATACAR", {200, 50, 50});
    Boton btnMover(font, "MOVER", {50, 150, 50});
    
    Boton btnRadar(font, "RADAR", sf::Color(60, 70, 80)); 
    btnRadar.setPosition({850.f, 20.f}); 

    Boton btnNotas(font, "NOTAS", sf::Color(139, 100, 60)); 
    btnNotas.setPosition({730.f, 20.f}); 

    sf::Text txtInfo(font);
    txtInfo.setCharacterSize(20);
    txtInfo.setOutlineThickness(2.f);
    txtInfo.setFillColor(sf::Color::Yellow);
    txtInfo.setPosition({350.f, 20.f});

    // ==========================================
    // >>> RECURSOS VISUALES <<<
    // ==========================================
    const sf::Vector2f radarCenter((float)WINDOW_SIZE.x / 2.f, (float)WINDOW_SIZE.y / 2.f);
    const sf::Color neonGreen(0, 255, 100);
    const sf::Color neonRed(255, 50, 50);

    sf::VertexArray radarBg(sf::PrimitiveType::TriangleStrip, 4);
    radarBg[0] = sf::Vertex{{0.f, 0.f}, sf::Color::Black};
    radarBg[1] = sf::Vertex{{(float)WINDOW_SIZE.x, 0.f}, sf::Color::Black};
    radarBg[2] = sf::Vertex{{0.f, (float)WINDOW_SIZE.y}, sf::Color(10, 25, 40)}; 
    radarBg[3] = sf::Vertex{{(float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y}, sf::Color(10, 25, 40)};

    std::vector<sf::CircleShape> radarCircles;
    for (int i = 1; i <= 4; ++i) {
        float r = i * 120.f; 
        sf::CircleShape c(r);
        c.setOrigin({r, r});
        c.setPosition(radarCenter);
        c.setFillColor(sf::Color::Transparent);
        c.setOutlineThickness(1.f);
        c.setOutlineColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 60));
        radarCircles.push_back(c);
    }
    
    sf::RectangleShape axisX({(float)WINDOW_SIZE.x, 1.f});
    axisX.setPosition({0.f, radarCenter.y});
    axisX.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));
    
    sf::RectangleShape axisY({1.f, (float)WINDOW_SIZE.y});
    axisY.setPosition({radarCenter.x, 0.f});
    axisY.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));

    float radarSweepAngle = 0.f;
    float largoLinea = radarCircles.back().getRadius() + 100.f;
    sf::RectangleShape sweepLine({largoLinea, 2.f});
    sweepLine.setOrigin({0.f, 1.f}); 
    sweepLine.setPosition(radarCenter);
    sweepLine.setFillColor(neonGreen);
    
    sf::RectangleShape sweepGlow({largoLinea, 15.f});
    sweepGlow.setOrigin({0.f, 7.5f});
    sweepGlow.setPosition(radarCenter);
    sweepGlow.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));

    sf::RectangleShape fondoNotas((sf::Vector2f)WINDOW_SIZE);
    fondoNotas.setFillColor(sf::Color(210, 195, 140)); 
    // ==========================================

    EstadoJuego estado = MENSAJE_P1;
    int sel = -1;
    bool moviendo = false;
    bool apuntando = false;
    
    // --- LÓGICA DE RADAR Y UAV ---
    bool modoRadar = false;
    bool modoNotas = false; 
    bool lanzandoUAV = false;
    bool lanzandoUAVRefuerzo = false; 
    float errorTimer = 0.f;
    sf::String msgError = "";
    
    sf::Clock uavTimer;

    bool cargandoDisparo = false;
    float potenciaActual = 0.f;
    const float VELOCIDAD_CARGA = 5.0f;
    const float MAX_DISTANCIA = 1500.f;

    bool enZonaEnemiga = false;
    sf::Vector2f origenDisparoReal(0,0); 

    sf::Clock dtClock;

    while (window.isOpen()) {
        float deltaTime = dtClock.restart().asSeconds();
        if (errorTimer > 0.f) errorTimer -= deltaTime;

        Jugador* jugadorActual = nullptr;
        Jugador* jugadorEnemigo = nullptr;
        sf::FloatRect* zonaObjetivo = nullptr;

        if (estado == TURNO_P1) {
            jugadorActual = &p1; jugadorEnemigo = &p2;
        } else if (estado == TURNO_P2) {
            jugadorActual = &p2; jugadorEnemigo = &p1;
        }

        if (sel != -1 && jugadorActual) {
            float barcoX = jugadorActual->getFlota()[sel].sprite.getPosition().x;
            if (barcoX < 500.f) zonaObjetivo = &zonaDerecha;
            else zonaObjetivo = &zonaIzquierda;
        } else {
            zonaObjetivo = (estado == TURNO_P1) ? &zonaDerecha : &zonaIzquierda;
        }

        while (const auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            // >>> CAMBIO DE TURNO Y GESTIÓN DE REFUERZOS <<<
            if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
                if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) {
                    EstadoJuego siguienteEstado = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                    Jugador* siguienteJugador = (siguienteEstado == TURNO_P1) ? &p1 : &p2;

                    // 1. Reducir Cooldown del siguiente jugador
                    if (siguienteJugador->cooldownRadar > 0) {
                        siguienteJugador->cooldownRadar--;
                    }

                    // 2. Verificar si llegan refuerzos
                    if (siguienteJugador->radarRefuerzoPendiente) {
                        lanzandoUAVRefuerzo = true;
                        siguienteJugador->radarRefuerzoPendiente = false; 
                        
                        // REGLA: "el enfriamiento se toma desde que aparece"
                        // Al llegar el refuerzo, activamos el cooldown de 2 turnos inmediatamente
                        siguienteJugador->cooldownRadar = 2; 
                        
                        // Configurar inicio
                        sf::Vector2f posInicio = { (float)WINDOW_SIZE.x / 2.f, (float)WINDOW_SIZE.y + 100.f };
                        if (uavLoaded) {
                            uavSprite.setPosition(posInicio);
                            uavSprite.setRotation(sf::degrees(0));
                            uavSprite.setScale({0.15f, 0.15f});
                        } else {
                            uavShapeFallback.setPosition(posInicio);
                        }
                    }

                    estado = siguienteEstado;
                    sel = -1; moviendo = false; apuntando = false; cargandoDisparo = false;
                    modoRadar = false; modoNotas = false; lanzandoUAV = false; 
                    errorTimer = 0.f;
                    btnMover.resetColor(); btnAtacar.resetColor(); btnRadar.resetColor(); btnNotas.resetColor();
                }
                continue;
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (lanzandoUAV || lanzandoUAVRefuerzo) continue;

            if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    
                    if (!apuntando && !cargandoDisparo && jugadorActual) {
                        
                        // --- BOTÓN RADAR ---
                        if (btnRadar.esClickeado(mousePos) && !modoNotas) {
                            if (!modoRadar) {
                                // REGLA 1: Verificar si ya hay una solicitud pendiente
                                if (jugadorActual->radarRefuerzoPendiente) {
                                    msgError = "SOLICITUD EN PROCESO. REFUERZOS LLEGAN SIGUIENTE TURNO.";
                                    errorTimer = 2.0f;
                                }
                                // REGLA 2: Verificar Cooldown
                                else if (jugadorActual->cooldownRadar > 0) {
                                    msgError = "SISTEMA ENFRIANDOSE. ESPERA " + std::to_string(jugadorActual->cooldownRadar) + " TURNO(S).";
                                    errorTimer = 2.0f;
                                }
                                else {
                                    // REGLA 3: Buscar Portaviones
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
                                        // CASO A: Despegue Normal (Inicia cooldown ahora)
                                        if (uavLoaded) {
                                            uavSprite.setPosition(posInicio);
                                            uavSprite.setRotation(sf::degrees(0));
                                            uavSprite.setScale({0.05f, 0.05f}); 
                                        } else {
                                            uavShapeFallback.setPosition(posInicio);
                                        }
                                        lanzandoUAV = true;
                                        uavTimer.restart();
                                        
                                        // Aplicar cooldown de 2 turnos
                                        jugadorActual->cooldownRadar = 2; 
                                        
                                        btnRadar.setColor(sf::Color::Red);
                                        sel = -1; moviendo = false; 
                                    } 
                                    else {
                                        // CASO B: Sin Portaviones (Delayed)
                                        // No aplicamos cooldown aquí, se aplica cuando llegue el refuerzo
                                        jugadorActual->radarRefuerzoPendiente = true;
                                        
                                        msgError = "PORTAVIONES CAIDO. REFUERZOS LLEGAN EL PROXIMO TURNO.";
                                        errorTimer = 3.0f;
                                    }
                                }
                            } 
                            else {
                                modoRadar = false;
                                btnRadar.resetColor();
                            }
                        }

                        // --- BOTÓN NOTAS ---
                        else if (btnNotas.esClickeado(mousePos) && !modoRadar) {
                            modoNotas = !modoNotas;
                            if (modoNotas) {
                                btnNotas.setColor(sf::Color::White); 
                                sel = -1; moviendo = false; 
                            } else {
                                btnNotas.resetColor();
                            }
                        }
                    }

                    if (modoRadar || modoNotas) continue; 

                    // --- JUEGO NORMAL ---
                    if (apuntando && sel != -1) {
                        cargandoDisparo = true;
                        potenciaActual = 0.f;
                    }
                    
                    if (!apuntando && !cargandoDisparo) {
                        if (sel != -1) {
                            if (btnAtacar.esClickeado(mousePos)) {
                                apuntando = true; moviendo = false;
                                btnAtacar.setColor({255, 69, 0}); btnMover.resetColor();
                            } 
                            else if (btnMover.esClickeado(mousePos)) {
                                if (moviendo) {
                                    IndicatorManager::actualizarTurnos(*jugadorActual);
                                    moviendo = false; sel = -1; btnMover.resetColor();
                                    estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                                } else {
                                    moviendo = true; btnMover.setColor({255, 140, 0});
                                }
                            }
                        }

                        if (!btnAtacar.esClickeado(mousePos) && !btnMover.esClickeado(mousePos) && 
                            !btnRadar.esClickeado(mousePos) && !btnNotas.esClickeado(mousePos) && jugadorActual) {
                            bool hit = false;
                            auto& flota = jugadorActual->getFlota();
                            for (size_t i = 0; i < flota.size(); ++i) {
                                if (flota[i].destruido) continue;
                                if (flota[i].sprite.getGlobalBounds().contains(mousePos)) {
                                    sel = (int)i; hit = true; 
                                    moviendo = false; apuntando = false; 
                                    btnMover.resetColor(); btnAtacar.resetColor();
                                    break;
                                }
                            }
                            if (!hit && !moviendo) sel = -1;
                        }
                    }
                }
            }

            if (const auto* m = ev->getIf<sf::Event::MouseButtonReleased>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    if (cargandoDisparo && apuntando && sel != -1) {
                        cargandoDisparo = false;
                        sf::Vector2f diff = mousePos - origenDisparoReal;
                        float angulo = std::atan2(diff.y, diff.x);
                        sf::Vector2f impacto;
                        impacto.x = origenDisparoReal.x + std::cos(angulo) * potenciaActual;
                        impacto.y = origenDisparoReal.y + std::sin(angulo) * potenciaActual;

                        if (jugadorEnemigo) {
                            AttackManager::procesarDisparo(impacto, origenDisparoReal, *jugadorEnemigo);
                        }
                        IndicatorManager::actualizarTurnos(*jugadorActual);
                        apuntando = false; sel = -1;
                        btnAtacar.resetColor();
                        estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                    }
                }
            }

            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::Escape) {
                    cargandoDisparo = false; apuntando = false; moviendo = false; sel = -1; 
                    modoRadar = false; modoNotas = false;
                    lanzandoUAV = false; lanzandoUAVRefuerzo = false;
                    btnAtacar.resetColor(); btnMover.resetColor(); btnRadar.resetColor(); btnNotas.resetColor();
                }
            }
        }

        // --- UPDATE ---
        
        if (lanzandoUAV) {
            if (uavTimer.getElapsedTime().asMilliseconds() > 500) {
                if (uavLoaded) {
                    uavSprite.move({0.f, -3.0f}); 
                    sf::Vector2f scale = uavSprite.getScale();
                    if (scale.x < 0.15f) uavSprite.setScale({scale.x + 0.0005f, scale.y + 0.0005f});
                    if (uavSprite.getPosition().y < -50.f) {
                        lanzandoUAV = false; modoRadar = true; btnRadar.setColor(sf::Color::Red);
                    }
                } else {
                    uavShapeFallback.move({0.f, -3.0f});
                    if (uavShapeFallback.getPosition().y < -50.f) {
                        lanzandoUAV = false; modoRadar = true; btnRadar.setColor(sf::Color::Red);
                    }
                }
            }
        }
        else if (lanzandoUAVRefuerzo) {
            if (uavLoaded) {
                uavSprite.move({0.f, -5.0f}); 
                if (uavSprite.getPosition().y < -50.f) {
                    lanzandoUAVRefuerzo = false; modoRadar = true; btnRadar.setColor(sf::Color::Red);
                }
            } else {
                uavShapeFallback.move({0.f, -5.0f});
                if (uavShapeFallback.getPosition().y < -50.f) {
                    lanzandoUAVRefuerzo = false; modoRadar = true; btnRadar.setColor(sf::Color::Red);
                }
            }
        }
        else if (!modoRadar && !modoNotas) {
            offset[0] += 0.5f; offset[1] += 0.25f;
            if (offset[0] >= (float)WINDOW_SIZE.x) offset[0] = 0; 
            if (offset[1] >= (float)WINDOW_SIZE.y) offset[1] = 0;
            fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));
        } 
        else if (modoRadar) {
            radarSweepAngle += 2.0f;
            if (radarSweepAngle >= 360.f) radarSweepAngle -= 360.f;
            sweepLine.setRotation(sf::degrees(radarSweepAngle));
            sweepGlow.setRotation(sf::degrees(radarSweepAngle));
        }

        if (cargandoDisparo) {
            potenciaActual += VELOCIDAD_CARGA;
            if (potenciaActual > MAX_DISTANCIA) potenciaActual = MAX_DISTANCIA;
        }

        if (moviendo && sel != -1 && jugadorActual) {
            auto& barco = jugadorActual->getFlota()[sel];
            if (!barco.destruido) MovementManager::procesarInput(jugadorActual->getFlota(), sel, WINDOW_SIZE);
        }

        if (sel != -1 && jugadorActual && !jugadorActual->getFlota()[sel].destruido) {
             auto& barco = jugadorActual->getFlota()[sel];
             auto bounds = barco.sprite.getGlobalBounds();
             sf::Vector2f p = barco.sprite.getPosition();
             float yBtn = p.y + bounds.size.y + 10.f;
             btnAtacar.setPosition({p.x, yBtn});
             btnMover.setPosition({p.x + 110.f, yBtn});
        }

        sf::Vector2f impactoVisual(0,0);
        if (apuntando && sel != -1 && jugadorActual) {
            auto bounds = jugadorActual->getFlota()[sel].sprite.getGlobalBounds();
            sf::Vector2f centroBarco = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            
            if (zonaObjetivo && zonaObjetivo->contains(mousePos)) {
                enZonaEnemiga = true;
                origenDisparoReal = {centroBarco.x, (float)WINDOW_SIZE.y};
            } else {
                enZonaEnemiga = false;
                origenDisparoReal = centroBarco;
            }

            float pot = cargandoDisparo ? potenciaActual : 50.f;
            sf::Vector2f diff = mousePos - origenDisparoReal;
            float ang = std::atan2(diff.y, diff.x);
            impactoVisual = origenDisparoReal + sf::Vector2f(std::cos(ang)*pot, std::sin(ang)*pot);
        }

        // --- DRAW ---
        window.clear();

        if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
            window.draw(fondo); 
            UIManager::dibujarTooltipTurno(window, font, estado);
        }
        else if (jugadorActual) {
            
            // >>> MODO RADAR <<<
            if (modoRadar && jugadorEnemigo) {
                // GUARDAR DATOS EN MEMORIA
                jugadorActual->memoriaRadar.clear(); 
                for (const auto& barco : jugadorEnemigo->getFlota()) {
                    if (!barco.destruido) {
                        sf::FloatRect b = barco.sprite.getGlobalBounds();
                        sf::Vector2f blipPos = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};
                        jugadorActual->memoriaRadar.push_back(blipPos);
                    }
                }

                window.draw(radarBg);
                window.draw(axisX); window.draw(axisY);
                for (const auto& circle : radarCircles) window.draw(circle);

                for (const auto& pos : jugadorActual->memoriaRadar) {
                    sf::CircleShape haloOut(15.f); haloOut.setOrigin({15.f, 15.f}); haloOut.setPosition(pos);
                    haloOut.setFillColor(sf::Color(neonRed.r, neonRed.g, neonRed.b, 50));
                    window.draw(haloOut);

                    sf::CircleShape core(5.f); core.setOrigin({5.f, 5.f}); core.setPosition(pos);
                    core.setFillColor(neonRed);
                    window.draw(core);
                }

                window.draw(sweepGlow);
                window.draw(sweepLine);

                txtInfo.setString("ESCANEANDO... GUARDANDO COORDENADAS EN BITACORA");
                txtInfo.setFillColor(neonGreen);
                txtInfo.setPosition({WINDOW_SIZE.x / 2.f - 200.f, 50.f}); 
                window.draw(txtInfo);
                
                btnRadar.dibujar(window);
            }
            // >>> MODO NOTAS <<<
            else if (modoNotas) {
                window.draw(fondoNotas); 

                // Puntos guardados
                for (const auto& pos : jugadorActual->memoriaRadar) {
                    sf::CircleShape punto(6.f);
                    punto.setOrigin({6.f, 6.f});
                    punto.setPosition(pos);
                    punto.setFillColor(sf::Color(180, 0, 0, 150)); 
                    window.draw(punto);
                }

                // Cruces de barcos destruidos
                if (jugadorEnemigo) {
                    for (const auto& barco : jugadorEnemigo->getFlota()) {
                        if (barco.destruido) {
                            sf::FloatRect b = barco.sprite.getGlobalBounds();
                            sf::Vector2f centro = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};
                            
                            sf::RectangleShape linea1({60.f, 8.f});
                            linea1.setOrigin({30.f, 4.f});
                            linea1.setPosition(centro);
                            linea1.setFillColor(sf::Color::Red);
                            linea1.setRotation(sf::degrees(45));

                            sf::RectangleShape linea2({60.f, 8.f});
                            linea2.setOrigin({30.f, 4.f});
                            linea2.setPosition(centro);
                            linea2.setFillColor(sf::Color::Red);
                            linea2.setRotation(sf::degrees(-45));

                            window.draw(linea1);
                            window.draw(linea2);
                        }
                    }
                }

                txtInfo.setString("-- BITACORA TACTICA --");
                txtInfo.setFillColor(sf::Color::Black);
                txtInfo.setPosition({350.f, 30.f});
                window.draw(txtInfo);

                btnNotas.dibujar(window);
            }
            // >>> MODO NORMAL <<<
            else {
                window.draw(fondo); 

                if (enZonaEnemiga && apuntando) {
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
                    sf::Color cBorde = moviendo ? sf::Color(255, 140, 0) : sf::Color(50, 150, 50);
                    if (apuntando) cBorde = sf::Color::Red;
                    
                    RenderManager::renderizarFlota(window, jugadorActual->getFlota(), sel, estado, moviendo, cBorde);
                    IndicatorManager::renderizarPistas(window, *jugadorActual, false);

                    if (apuntando) {
                        txtInfo.setString("Arrastra hacia territorio enemigo...");
                        txtInfo.setFillColor(sf::Color::Yellow);
                        txtInfo.setPosition({300.f, 20.f});
                        window.draw(txtInfo);
                    }

                    if (sel != -1 && !jugadorActual->getFlota()[sel].destruido && !cargandoDisparo && !lanzandoUAV && !lanzandoUAVRefuerzo) {
                        btnAtacar.dibujar(window);
                        btnMover.dibujar(window);
                    }
                }

                if (apuntando && sel != -1) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    float pot = cargandoDisparo ? potenciaActual : 50.f;
                    IndicatorManager::dibujarVectorApuntado(window, origenDisparoReal, mousePos, pot, font);
                }

                if (lanzandoUAV) {
                    if (uavTimer.getElapsedTime().asMilliseconds() <= 500) {
                        txtInfo.setString("PREPARANDO DESPEGUE...");
                        txtInfo.setFillColor(sf::Color::White);
                        sf::Vector2f pos = uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                        txtInfo.setPosition({pos.x - 80.f, pos.y - 50.f});
                        window.draw(txtInfo);
                    } else {
                        txtInfo.setString("UAV EN RUTA...");
                        txtInfo.setFillColor(sf::Color::Cyan);
                        sf::Vector2f pos = uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                        txtInfo.setPosition({pos.x + 20.f, pos.y});
                        window.draw(txtInfo);
                    }
                    if (uavLoaded) window.draw(uavSprite); else window.draw(uavShapeFallback);
                }

                if (lanzandoUAVRefuerzo) {
                    txtInfo.setString("REFUERZOS LLEGANDO...");
                    txtInfo.setFillColor(sf::Color::Green);
                    sf::Vector2f pos = uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                    txtInfo.setPosition({pos.x + 20.f, pos.y});
                    window.draw(txtInfo);
                    if (uavLoaded) window.draw(uavSprite); else window.draw(uavShapeFallback);
                }

                if (errorTimer > 0.f) {
                    txtInfo.setString(msgError);
                    txtInfo.setFillColor(sf::Color::Red);
                    txtInfo.setPosition({150.f, 300.f});
                    window.draw(txtInfo);
                }

                if (!apuntando && !cargandoDisparo && !lanzandoUAV && !lanzandoUAVRefuerzo) {
                    btnRadar.dibujar(window);
                    btnNotas.dibujar(window); 
                }
            }
        }

        window.display();
    }
    return 0;
}