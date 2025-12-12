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
    const sf::Vector2u WINDOW_SIZE(1000, 700);
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Radar Dinamico (SFML 3.0)");
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

    // --- ZONAS (SFML 3.0) ---
    sf::FloatRect zonaIzquierda({0.f, 0.f}, {500.f, 700.f});
    sf::FloatRect zonaDerecha({500.f, 0.f}, {500.f, 700.f});

    // --- UI ---
    Boton btnAtacar(font, "ATACAR", {200, 50, 50});
    Boton btnMover(font, "MOVER", {50, 150, 50});
    
    sf::Text txtInfo(font);
    txtInfo.setCharacterSize(20);
    txtInfo.setOutlineThickness(2.f);
    txtInfo.setFillColor(sf::Color::Yellow);
    txtInfo.setPosition({350.f, 20.f});

    EstadoJuego estado = MENSAJE_P1;
    int sel = -1;
    bool moviendo = false;
    bool apuntando = false;
    
    // Variables Disparo
    bool cargandoDisparo = false;
    float potenciaActual = 0.f;
    const float VELOCIDAD_CARGA = 5.0f;
    const float MAX_DISTANCIA = 1500.f;

    while (window.isOpen()) {
        Jugador* jugadorActual = nullptr;
        Jugador* jugadorEnemigo = nullptr;
        sf::FloatRect* zonaObjetivo = nullptr;

        // 1. Configurar Jugadores
        if (estado == TURNO_P1) {
            jugadorActual = &p1; jugadorEnemigo = &p2;
        } else if (estado == TURNO_P2) {
            jugadorActual = &p2; jugadorEnemigo = &p1;
        }

        // 2. Determinar Zona Objetivo Dinámica (Lógica de Lados)
        // Si el barco está a la izquierda (<500), ataca a la derecha. Y viceversa.
        if (sel != -1 && jugadorActual) {
            float barcoX = jugadorActual->getFlota()[sel].sprite.getPosition().x;
            if (barcoX < 500.f) zonaObjetivo = &zonaDerecha;
            else zonaObjetivo = &zonaIzquierda;
        } else {
            // Fallback por seguridad
            zonaObjetivo = (estado == TURNO_P1) ? &zonaDerecha : &zonaIzquierda;
        }

        while (const auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            // CAMBIO DE TURNO
            if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
                if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) {
                    estado = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                    sel = -1; moviendo = false; apuntando = false; cargandoDisparo = false;
                    btnMover.resetColor(); btnAtacar.resetColor();
                }
                continue;
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            // INPUT
            if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    
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

                        if (!btnAtacar.esClickeado(mousePos) && !btnMover.esClickeado(mousePos) && jugadorActual) {
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

            // SOLTAR CLICK (Disparo)
            if (const auto* m = ev->getIf<sf::Event::MouseButtonReleased>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    if (cargandoDisparo && apuntando && sel != -1) {
                        cargandoDisparo = false;

                        auto bounds = jugadorActual->getFlota()[sel].sprite.getGlobalBounds();
                        sf::Vector2f origen = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
                        sf::Vector2f diff = mousePos - origen;
                        float angulo = std::atan2(diff.y, diff.x);
                        
                        sf::Vector2f impacto;
                        impacto.x = origen.x + std::cos(angulo) * potenciaActual;
                        impacto.y = origen.y + std::sin(angulo) * potenciaActual;

                        // Verificar si el impacto cae en la zona VÁLIDA
                        bool enZonaCorrecta = zonaObjetivo && zonaObjetivo->contains(impacto);
                        bool enVentana = (impacto.x >= 0 && impacto.x <= WINDOW_SIZE.x && 
                                          impacto.y >= 0 && impacto.y <= WINDOW_SIZE.y);

                        if (enZonaCorrecta && enVentana) {
                            if (jugadorEnemigo) {
                                AttackManager::procesarDisparo(impacto, origen, *jugadorEnemigo);
                            }
                            IndicatorManager::actualizarTurnos(*jugadorActual);
                            apuntando = false; sel = -1;
                            btnAtacar.resetColor();
                            estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                        } else {
                            std::cout << ">>> DISPARO INVALIDO" << std::endl;
                            potenciaActual = 0.f;
                        }
                    }
                }
            }

            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::Escape) {
                    cargandoDisparo = false; apuntando = false; moviendo = false; sel = -1;
                    btnAtacar.resetColor(); btnMover.resetColor();
                }
            }
        }

        // --- UPDATE ---
        offset[0] += 0.5f; offset[1] += 0.25f;
        if (offset[0] >= (float)WINDOW_SIZE.x) offset[0] = 0; 
        if (offset[1] >= (float)WINDOW_SIZE.y) offset[1] = 0;
        fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

        if (cargandoDisparo) {
            potenciaActual += VELOCIDAD_CARGA;
            if (potenciaActual > MAX_DISTANCIA) potenciaActual = MAX_DISTANCIA;
        }

        if (moviendo && sel != -1 && jugadorActual) {
            auto& barco = jugadorActual->getFlota()[sel];
            if (!barco.destruido) MovementManager::procesarInput(jugadorActual->getFlota(), sel, WINDOW_SIZE);
        }

        // Posicionar Botones
        if (sel != -1 && jugadorActual && !jugadorActual->getFlota()[sel].destruido) {
             auto& barco = jugadorActual->getFlota()[sel];
             auto bounds = barco.sprite.getGlobalBounds();
             sf::Vector2f p = barco.sprite.getPosition();
             float yBtn = p.y + bounds.size.y + 10.f;
             btnAtacar.setPosition({p.x, yBtn});
             btnMover.setPosition({p.x + 110.f, yBtn});
        }

        // --- DRAW ---
        window.clear();
        window.draw(fondo);

        // -- ESTADO VISUAL: ¿MODO ATAQUE TOTAL O NORMAL? --
        bool visualModoAtaque = false;
        sf::Vector2f impactoVisual(0,0);
        sf::Vector2f origenVisual(0,0);

        if (apuntando && sel != -1 && jugadorActual) {
            auto bounds = jugadorActual->getFlota()[sel].sprite.getGlobalBounds();
            sf::Vector2f centro = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            
            // Si el mouse entra en la zona enemiga, activamos el "Modo Visual de Ataque"
            // (Como en la primera versión: pantalla roja, ocultar todo lo propio)
            if (zonaObjetivo && zonaObjetivo->contains(mousePos)) {
                visualModoAtaque = true;
                
                // En modo ataque visual, el origen es la artillería inferior
                origenVisual = {centro.x, (float)WINDOW_SIZE.y};
            } else {
                origenVisual = centro;
            }

            // Calcular impacto
            float pot = cargandoDisparo ? potenciaActual : 50.f;
            sf::Vector2f diff = mousePos - origenVisual;
            float ang = std::atan2(diff.y, diff.x);
            impactoVisual = origenVisual + sf::Vector2f(std::cos(ang)*pot, std::sin(ang)*pot);
        }

        // --- RENDERIZADO ---
        if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
            UIManager::dibujarTooltipTurno(window, font, estado);
        }
        else if (jugadorActual) {
            
            if (visualModoAtaque) {
                // >>> VISUAL ATAQUE TOTAL (Reciclando lógica V1) <<<
                // 1. Filtro Rojo en TODA la pantalla
                sf::RectangleShape overlay(sf::Vector2f((float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y));
                overlay.setFillColor(sf::Color(255, 0, 0, 40)); 
                window.draw(overlay);

                // 2. NO DIBUJAMOS BARCOS PROPIOS (Estamos concentrados en el enemigo)
                // 3. NO DIBUJAMOS BOTONES

                // 4. Texto de Confirmación
                txtInfo.setString("! ZONA ENEMIGA ! SUELTA PARA DISPARAR");
                txtInfo.setFillColor(sf::Color::Red);
                txtInfo.setPosition({impactoVisual.x + 20.f, impactoVisual.y - 40.f});
                window.draw(txtInfo);

            } else {
                // >>> VISUAL NORMAL (Tu flota visible) <<<
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

                // Botones (Solo si no estamos cargando disparo para no estorbar)
                if (sel != -1 && !jugadorActual->getFlota()[sel].destruido && !cargandoDisparo) {
                    btnAtacar.dibujar(window);
                    btnMover.dibujar(window);
                }
            }

            // --- VECTOR DE APUNTADO (Siempre visible si apuntas) ---
            if (apuntando && sel != -1) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                float pot = cargandoDisparo ? potenciaActual : 50.f;
                
                // Usamos el origenVisual que cambia dinámicamente (Barco o Fondo)
                IndicatorManager::dibujarVectorApuntado(window, origenVisual, mousePos, pot, font);
            }
        }
        window.display();
    }
    return 0;
}