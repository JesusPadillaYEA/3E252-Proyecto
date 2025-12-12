#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "EstadoJuego.hpp"
#include "BarcoEntity.hpp"
#include "Boton.hpp"
#include "CollisionManager.hpp"
#include "MovementManager.hpp"
#include "UIManager.hpp"

int main() {
    // Definimos el tamaño en una variable para reusarla
    const sf::Vector2u WINDOW_SIZE(1000, 700);
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Limites (SFML 3.0)");
    window.setFramerateLimit(60);

    // --- CARGAR RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture tDest, tSub, tMar, tPort;
    if (!tDest.loadFromFile("assets/images/destructor .png")) return -1;
    if (!tSub.loadFromFile("assets/images/submarino.png")) return -1;
    if (!tMar.loadFromFile("assets/images/mar.png")) return -1;
    if (!tPort.loadFromFile("assets/images/portaviones.png")) tPort = tDest;

    // --- FONDO MAPA COMPLETO ---
    tMar.setRepeated(true);
    sf::Sprite fondo(tMar);
    float offset[2] = {0.f, 0.f};
    
    // Inicializamos el rectángulo del tamaño exacto de la ventana
    fondo.setTextureRect(sf::IntRect({0, 0}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

    // --- FLOTAS ---
    std::vector<BarcoEntity> fP1;
    // En src/4_interaccion.cpp
    // Reduce las escalas (ejemplo: de 0.3 a 0.15)
    fP1.emplace_back(tDest, "Destructor P1"); 
    fP1.back().sprite.setPosition({50.f, 50.f}); 
    fP1.back().sprite.setScale({0.15f, 0.15f}); // <--- MÁS PEQUEÑO

    fP1.emplace_back(tPort, "Portaviones P1"); 
    fP1.back().sprite.setPosition({50.f, 250.f}); // <--- MÁS SEPARADO
    fP1.back().sprite.setScale({0.25f, 0.25f}); // <--- MÁS PEQUEÑO

    std::vector<BarcoEntity> fP2;
    fP2.emplace_back(tSub, "Submarino A"); fP2.back().sprite.setPosition({800.f, 50.f}); fP2.back().sprite.setScale({0.4f, 0.4f}); fP2.back().sprite.setColor({200,200,255});
    fP2.emplace_back(tSub, "Submarino B"); fP2.back().sprite.setPosition({900.f, 300.f}); fP2.back().sprite.setScale({0.4f, 0.4f}); fP2.back().sprite.setColor({200,200,255});

    // --- UI ---
    Boton btnAtacar(font, "ATACAR", {200, 50, 50});
    Boton btnMover(font, "MOVER", {50, 150, 50});
    sf::Text txtAyuda(font);
    txtAyuda.setString("FLECHAS: Mover | ENTER: Confirmar | ESC: Cancelar");
    txtAyuda.setCharacterSize(16); txtAyuda.setOutlineThickness(2.f);

    EstadoJuego estado = MENSAJE_P1;
    int sel = -1;
    bool moviendo = false;

    while (window.isOpen()) {
        auto* flotaActiva = (estado == TURNO_P1) ? &fP1 : (estado == TURNO_P2 ? &fP2 : nullptr);

        while (const auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
                if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) 
                    estado = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                continue;
            }

            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::Escape) {
                    moviendo = false; if (!moviendo) sel = -1; btnMover.resetColor();
                }
                if (k->code == sf::Keyboard::Key::Enter && moviendo) {
                    moviendo = false; sel = -1; btnMover.resetColor();
                    estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                }
            }

            if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    sf::Vector2f pos = window.mapPixelToCoords(m->position);

                    if (sel != -1) {
                        if (btnAtacar.esClickeado(pos)) {
                            moviendo = false; sel = -1; btnMover.resetColor();
                            estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                        } 
                        else if (btnMover.esClickeado(pos)) {
                            if (moviendo) {
                                moviendo = false; sel = -1; btnMover.resetColor();
                                estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                            } else {
                                moviendo = true; btnMover.setColor({255, 140, 0});
                            }
                        }
                    }

                    if (!btnAtacar.esClickeado(pos) && !btnMover.esClickeado(pos) && flotaActiva) {
                        bool hit = false;
                        for (size_t i = 0; i < flotaActiva->size(); ++i) {
                            if ((*flotaActiva)[i].sprite.getGlobalBounds().contains(pos)) {
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
        
        // El mapa siempre cubre toda la ventana (WINDOW_SIZE.x, WINDOW_SIZE.y)
        fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

        if (moviendo && sel != -1 && flotaActiva) {
            // Pasamos WINDOW_SIZE para limites de colision
            MovementManager::procesarInput(*flotaActiva, sel, WINDOW_SIZE);
        }

        if (sel != -1 && flotaActiva) {
            auto bounds = (*flotaActiva)[sel].sprite.getGlobalBounds();
            sf::Vector2f p = (*flotaActiva)[sel].sprite.getPosition();
            float yBtn = p.y + bounds.size.y + 10.f;
            btnAtacar.setPosition({p.x, yBtn});
            btnMover.setPosition({p.x + 110.f, yBtn});
            if (moviendo) {
                float tw = txtAyuda.getLocalBounds().size.x;
                txtAyuda.setPosition({p.x + (bounds.size.x-tw)/2, yBtn + 50.f});
                if (txtAyuda.getPosition().x < 10) txtAyuda.setPosition({10, yBtn + 50});
            }
        }

        // --- DRAW ---
        window.clear();
        window.draw(fondo);

        if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
            UIManager::dibujarTooltipTurno(window, font, estado);
        } 
        else if (flotaActiva) {
            for (size_t i=0; i<flotaActiva->size(); ++i) {
                auto& s = (*flotaActiva)[i].sprite;
                s.setColor((int)i == sel ? sf::Color(255,200,200) : ((estado==TURNO_P2)?sf::Color(200,200,255):sf::Color::White));
                window.draw(s);
            }
            if (sel != -1) {
                sf::RectangleShape borde((*flotaActiva)[sel].sprite.getGlobalBounds().size);
                borde.setPosition((*flotaActiva)[sel].sprite.getPosition());
                borde.setFillColor(sf::Color::Transparent);
                borde.setOutlineThickness(2.f);
                borde.setOutlineColor(moviendo ? sf::Color(255, 140, 0) : sf::Color(50, 150, 50));
                window.draw(borde);
                btnAtacar.dibujar(window);
                btnMover.dibujar(window);
                if (moviendo) window.draw(txtAyuda);
            }
        }
        window.display();
    }
    return 0;
}