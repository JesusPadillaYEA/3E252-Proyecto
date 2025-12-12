#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

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
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Vectorial (SFML 3.0)");
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

    Jugador p1(1, {0.f, 0.f});
    Jugador p2(2, {700.f, 0.f});

    GeneradorFlotas::inicializarFlota(p1, tDest, tPort, tSub);
    GeneradorFlotas::inicializarFlota(p2, tDest, tPort, tSub);

    Boton btnAtacar(font, "ATACAR", {200, 50, 50});
    Boton btnMover(font, "MOVER", {50, 150, 50});
    
    sf::Text txtInfo(font);
    txtInfo.setCharacterSize(20);
    txtInfo.setOutlineThickness(2.f);
    txtInfo.setFillColor(sf::Color::Yellow);
    txtInfo.setString("MODO ATAQUE: Sigue la flecha naranja para devolver el fuego");
    txtInfo.setPosition({250.f, 50.f});

    EstadoJuego estado = MENSAJE_P1;
    int sel = -1;
    bool moviendo = false;

    while (window.isOpen()) {
        Jugador* jugadorActual = nullptr;
        Jugador* jugadorEnemigo = nullptr;

        if (estado == TURNO_P1 || estado == ATACANDO_P1) {
            jugadorActual = &p1;
            jugadorEnemigo = &p2;
        } else if (estado == TURNO_P2 || estado == ATACANDO_P2) {
            jugadorActual = &p2;
            jugadorEnemigo = &p1;
        }

        while (const auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
                if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) 
                    estado = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                continue;
            }

            // MODO ATAQUE
            if (estado == ATACANDO_P1 || estado == ATACANDO_P2) {
                if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                    if (m->button == sf::Mouse::Button::Left) {
                        sf::Vector2f targetPos = window.mapPixelToCoords(m->position);
                        sf::Vector2f origenPos = {0,0};
                        
                        if (sel != -1 && jugadorActual) {
                            auto bounds = jugadorActual->getFlota()[sel].sprite.getGlobalBounds();
                            origenPos = {
                                bounds.position.x + bounds.size.x / 2.f,
                                bounds.position.y + bounds.size.y / 2.f
                            };
                        }

                        if (jugadorEnemigo) {
                            AttackManager::procesarDisparo(targetPos, origenPos, *jugadorEnemigo, WINDOW_SIZE);
                        }

                        IndicatorManager::actualizarTurnos(*jugadorActual);
                        estado = (estado == ATACANDO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                        sel = -1; moviendo = false; btnMover.resetColor();
                    }
                }
                if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                    if (k->code == sf::Keyboard::Key::Escape) {
                        estado = (estado == ATACANDO_P1) ? TURNO_P1 : TURNO_P2;
                    }
                }
                continue;
            }

            // MODO NORMAL
            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::Escape) {
                    moviendo = false; sel = -1; btnMover.resetColor();
                }
                if (k->code == sf::Keyboard::Key::Enter && moviendo) {
                    IndicatorManager::actualizarTurnos(*jugadorActual);
                    moviendo = false; sel = -1; btnMover.resetColor();
                    estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                }
            }

            if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    sf::Vector2f pos = window.mapPixelToCoords(m->position);

                    if (sel != -1) {
                        if (btnAtacar.esClickeado(pos)) {
                            moviendo = false; btnMover.resetColor();
                            estado = (estado == TURNO_P1) ? ATACANDO_P1 : ATACANDO_P2;
                        } 
                        else if (btnMover.esClickeado(pos)) {
                            if (moviendo) {
                                IndicatorManager::actualizarTurnos(*jugadorActual);
                                moviendo = false; sel = -1; btnMover.resetColor();
                                estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                            } else {
                                moviendo = true; btnMover.setColor({255, 140, 0});
                            }
                        }
                    }

                    if (!btnAtacar.esClickeado(pos) && !btnMover.esClickeado(pos) && jugadorActual) {
                        bool hit = false;
                        auto& flota = jugadorActual->getFlota();
                        for (size_t i = 0; i < flota.size(); ++i) {
                            if (flota[i].destruido) continue;
                            if (flota[i].sprite.getGlobalBounds().contains(pos)) {
                                sel = (int)i; moviendo = false; btnMover.resetColor(); hit = true; break;
                            }
                        }
                        if (!hit && !moviendo) sel = -1;
                    }
                }
            }
        }

        // --- UPDATE ---
        offset[0] += 0.5f; offset[1] += 0.25f;
        if (offset[0] >= (float)WINDOW_SIZE.x) offset[0] = 0; 
        if (offset[1] >= (float)WINDOW_SIZE.y) offset[1] = 0;
        fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

        if (moviendo && sel != -1 && jugadorActual) {
            auto& barco = jugadorActual->getFlota()[sel];
            if (!barco.destruido) MovementManager::procesarInput(jugadorActual->getFlota(), sel, WINDOW_SIZE);
            else { moviendo = false; sel = -1; }
        }

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

        if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
            UIManager::dibujarTooltipTurno(window, font, estado);
        }
        else if (estado == ATACANDO_P1 || estado == ATACANDO_P2) {
            sf::RectangleShape mira({(float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y});
            mira.setFillColor(sf::Color(255, 0, 0, 30));
            window.draw(mira);
            window.draw(txtInfo);

            // MODO ATAQUE: Dibujar flecha naranja desde abajo
            // Esta flecha apunta exactamente a donde está el enemigo
            if (jugadorActual) IndicatorManager::renderizarPistas(window, *jugadorActual, true);
        }
        else if (jugadorActual) {
            sf::Color cBorde = moviendo ? sf::Color(255, 140, 0) : sf::Color(50, 150, 50);
            RenderManager::renderizarFlota(window, jugadorActual->getFlota(), sel, estado, moviendo, cBorde);

            // MODO DEFENSA: Dibujar flecha roja desde arriba
            IndicatorManager::renderizarPistas(window, *jugadorActual, false);

            if (sel != -1 && !jugadorActual->getFlota()[sel].destruido) {
                btnAtacar.dibujar(window);
                btnMover.dibujar(window);
            }
        }
        window.display();
    }
    return 0;
}