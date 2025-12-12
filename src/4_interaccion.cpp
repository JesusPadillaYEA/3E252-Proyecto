#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

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
    // SFML 3.0: Vector2u se inicializa con {}
    const sf::Vector2u WINDOW_SIZE(1000, 700);
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Radar Tactico (SFML 3.0)");
    window.setFramerateLimit(60);

    // --- RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture tDest, tSub, tMar, tPort;
    bool assetsOk = true;
    if (!tDest.loadFromFile("assets/images/destructor .png")) assetsOk = false;
    if (!tSub.loadFromFile("assets/images/submarino.png")) assetsOk = false;
    if (!tMar.loadFromFile("assets/images/mar.png")) assetsOk = false;
    if (!tPort.loadFromFile("assets/images/portaviones.png")) tPort = tDest; 
    if (!assetsOk) return -1;

    tMar.setRepeated(true);
    sf::Sprite fondo(tMar);
    float offset[2] = {0.f, 0.f};
    fondo.setTextureRect(sf::IntRect({0, 0}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

    // --- JUGADORES ---
    Jugador p1(1, {0.f, 0.f});
    Jugador p2(2, {700.f, 0.f});

    GeneradorFlotas::inicializarFlota(p1, tDest, tPort, tSub);
    GeneradorFlotas::inicializarFlota(p2, tDest, tPort, tSub);

    // --- ZONAS ---
    sf::FloatRect zonaIzquierda({0.f, 0.f}, {500.f, 700.f});
    sf::FloatRect zonaDerecha({500.f, 0.f}, {500.f, 700.f});

    // --- UI ---
    // SFML 3.0: Colores explícitos
    Boton btnAtacar(font, "ATACAR", {200, 50, 50});
    Boton btnMover(font, "MOVER", {50, 150, 50});
    
    // CORRECCIÓN PREVIA: Inicializamos con Color gris y luego seteamos posición
    Boton btnRadar(font, "RADAR", sf::Color(60, 70, 80)); 
    btnRadar.setPosition({850.f, 20.f}); 

    sf::Text txtInfo(font);
    txtInfo.setCharacterSize(20);
    txtInfo.setOutlineThickness(2.f);
    txtInfo.setFillColor(sf::Color::Yellow);
    txtInfo.setPosition({350.f, 20.f});

    // ==========================================
    // >>> RECURSOS ESTÉTICOS DEL RADAR <<<
    // ==========================================
    const sf::Vector2f radarCenter((float)WINDOW_SIZE.x / 2.f, (float)WINDOW_SIZE.y / 2.f);
    const sf::Color neonGreen(0, 255, 100);
    const sf::Color neonRed(255, 50, 50);

    // 1. Fondo Gradiente (VertexArray)
    // SFML 3.0: Inicialización de Vertex con llaves dobles {{pos}, color}
    sf::VertexArray radarBg(sf::PrimitiveType::TriangleStrip, 4);
    radarBg[0] = sf::Vertex{{0.f, 0.f}, sf::Color::Black};
    radarBg[1] = sf::Vertex{{(float)WINDOW_SIZE.x, 0.f}, sf::Color::Black};
    radarBg[2] = sf::Vertex{{0.f, (float)WINDOW_SIZE.y}, sf::Color(10, 25, 40)}; // Gris azulado frío
    radarBg[3] = sf::Vertex{{(float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y}, sf::Color(10, 25, 40)};

    // 2. Elementos HUD (Círculos y Líneas Estáticas)
    std::vector<sf::CircleShape> radarCircles;
    for (int i = 1; i <= 4; ++i) {
        float r = i * 120.f;
        sf::CircleShape c(r);
        c.setOrigin({r, r}); // Centro geométrico
        c.setPosition(radarCenter);
        c.setFillColor(sf::Color::Transparent);
        c.setOutlineThickness(1.f);
        c.setOutlineColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 60)); // Muy transparente
        radarCircles.push_back(c);
    }
    // Cruz central
    sf::RectangleShape axisX({(float)WINDOW_SIZE.x, 1.f});
    axisX.setPosition({0.f, radarCenter.y});
    axisX.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));
    
    sf::RectangleShape axisY({1.f, (float)WINDOW_SIZE.y});
    axisY.setPosition({radarCenter.x, 0.f});
    axisY.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));

    // 3. Línea de Barrido (Scanner)
    float radarSweepAngle = 0.f;
    // Línea principal nítida
    float largoLinea = radarCircles.back().getRadius() + 100.f; // Que cubra todo
    sf::RectangleShape sweepLine({largoLinea, 2.f});
    sweepLine.setOrigin({0.f, 1.f}); 
    sweepLine.setPosition(radarCenter);
    sweepLine.setFillColor(neonGreen);
    
    // "Glow" de la línea (más ancha y transparente detrás)
    sf::RectangleShape sweepGlow({largoLinea, 15.f}); // 15px de ancho para el brillo
    sweepGlow.setOrigin({0.f, 7.5f});
    sweepGlow.setPosition(radarCenter);
    // Gradiente en el brillo sería ideal, pero color plano transparente funciona bien
    sweepGlow.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));
    // ==========================================

    EstadoJuego estado = MENSAJE_P1;
    int sel = -1;
    bool moviendo = false;
    bool apuntando = false;
    bool modoRadar = false;

    // Variables Disparo
    bool cargandoDisparo = false;
    float potenciaActual = 0.f;
    const float VELOCIDAD_CARGA = 5.0f;
    const float MAX_DISTANCIA = 1500.f;

    // Lógica Visual
    bool enZonaEnemiga = false;
    sf::Vector2f origenDisparoReal(0,0); 

    while (window.isOpen()) {
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

            if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
                if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) {
                    estado = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                    sel = -1; moviendo = false; apuntando = false; cargandoDisparo = false;
                    modoRadar = false; 
                    btnMover.resetColor(); btnAtacar.resetColor(); btnRadar.resetColor();
                }
                continue;
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    
                    // --- RADAR TOGGLE ---
                    if (!apuntando && !cargandoDisparo && jugadorActual) {
                        if (btnRadar.esClickeado(mousePos)) {
                            modoRadar = !modoRadar; 
                            if (modoRadar) {
                                btnRadar.setColor(sf::Color::Red);
                                sel = -1; moviendo = false; // Resetear selección al entrar en radar
                            } else {
                                btnRadar.resetColor();
                            }
                        }
                    }

                    if (modoRadar) continue; // Si radar está activo, bloquear clicks en el juego

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

                        if (!btnAtacar.esClickeado(mousePos) && !btnMover.esClickeado(mousePos) && !btnRadar.esClickeado(mousePos) && jugadorActual) {
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
                    modoRadar = false; 
                    btnAtacar.resetColor(); btnMover.resetColor(); btnRadar.resetColor();
                }
            }
        }

        // --- UPDATE ---
        if (!modoRadar) {
            // Animar el agua solo si vemos el mapa normal
            offset[0] += 0.5f; offset[1] += 0.25f;
            if (offset[0] >= (float)WINDOW_SIZE.x) offset[0] = 0; 
            if (offset[1] >= (float)WINDOW_SIZE.y) offset[1] = 0;
            fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));
        } else {
            // Animar Radar
            radarSweepAngle += 2.0f; // Velocidad de giro
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
            
            // >>>>>>>>>>>>>>>>> RENDERIZADO DEL RADAR <<<<<<<<<<<<<<<<<
            if (modoRadar && jugadorEnemigo) {
                // 1. Fondo Gradiente
                window.draw(radarBg);

                // 2. HUD (Círculos y Ejes)
                window.draw(axisX);
                window.draw(axisY);
                for (const auto& circle : radarCircles) {
                    window.draw(circle);
                }

                // 3. Enemigos (Blips Brillantes)
                for (const auto& barco : jugadorEnemigo->getFlota()) {
                    if (!barco.destruido) {
                        sf::FloatRect b = barco.sprite.getGlobalBounds();
                        sf::Vector2f blipPos = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};

                        // Dibujar 3 capas de círculos para simular luz
                        // Capa 3: Halo exterior (muy transparente)
                        sf::CircleShape haloOut(15.f);
                        haloOut.setOrigin({15.f, 15.f});
                        haloOut.setPosition(blipPos);
                        haloOut.setFillColor(sf::Color(neonRed.r, neonRed.g, neonRed.b, 50));
                        window.draw(haloOut);

                        // Capa 2: Halo interior
                        sf::CircleShape haloIn(10.f);
                        haloIn.setOrigin({10.f, 10.f});
                        haloIn.setPosition(blipPos);
                        haloIn.setFillColor(sf::Color(neonRed.r, neonRed.g, neonRed.b, 100));
                        window.draw(haloIn);

                        // Capa 1: Núcleo (Sólido)
                        sf::CircleShape core(5.f);
                        core.setOrigin({5.f, 5.f});
                        core.setPosition(blipPos);
                        core.setFillColor(neonRed);
                        window.draw(core);
                    }
                }

                // 4. Línea de Escaneo (por encima de todo para efecto barrido)
                window.draw(sweepGlow);
                window.draw(sweepLine);

                // UI Radar
                txtInfo.setString("RADAR ACTIVO // BUSCANDO OBJETIVOS...");
                txtInfo.setFillColor(neonGreen);
                // Centrar texto (aprox)
                txtInfo.setPosition({WINDOW_SIZE.x / 2.f - 180.f, 30.f}); 
                window.draw(txtInfo);
                
                btnRadar.dibujar(window);
            }
            // >>>>>>>>>>>>>>>>> RENDERIZADO NORMAL <<<<<<<<<<<<<<<<<
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

                    if (sel != -1 && !jugadorActual->getFlota()[sel].destruido && !cargandoDisparo) {
                        btnAtacar.dibujar(window);
                        btnMover.dibujar(window);
                    }
                }

                if (apuntando && sel != -1) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    float pot = cargandoDisparo ? potenciaActual : 50.f;
                    IndicatorManager::dibujarVectorApuntado(window, origenDisparoReal, mousePos, pot, font);
                }

                if (!apuntando && !cargandoDisparo) {
                    btnRadar.dibujar(window);
                }
            }
        }

        window.display();
    }
    return 0;
}