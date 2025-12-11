#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// --- MÓDULOS DEL PROYECTO ---
#include "EstadoJuego.hpp"
#include "BarcoEntity.hpp"
#include "Boton.hpp"
#include "CollisionManager.hpp" // Nuevo
#include "MovementManager.hpp"  // Nuevo
#include "UIManager.hpp"        // Nuevo

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 700}), "Batalla Naval Modular (SFML 3.0)");
    window.setFramerateLimit(60);

    // --- CARGAR RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture tDest, tSub, tMar, tPort;
    if (!tDest.loadFromFile("assets/images/destructor .png")) return -1;
    if (!tSub.loadFromFile("assets/images/submarino.png")) return -1;
    if (!tMar.loadFromFile("assets/images/mar.png")) return -1;
    if (!tPort.loadFromFile("assets/images/portaviones.png")) tPort = tDest;

    tMar.setRepeated(true);
    sf::Sprite fondo(tMar);
    float offset[2] = {0.f, 0.f};
    fondo.setTextureRect(sf::IntRect({0, 0}, {1000, 700}));

    // --- DEFINICIÓN DE FLOTAS ---
    std::vector<BarcoEntity> fP1;
    fP1.emplace_back(tDest, "Destructor P1"); fP1.back().sprite.setPosition({100.f, 200.f}); fP1.back().sprite.setScale({0.3f, 0.3f});
    fP1.emplace_back(tPort, "Portaviones P1"); fP1.back().sprite.setPosition({100.f, 400.f}); fP1.back().sprite.setScale({0.5f, 0.5f});

    std::vector<BarcoEntity> fP2;
    fP2.emplace_back(tSub, "Submarino A"); fP2.back().sprite.setPosition({700.f, 200.f}); fP2.back().sprite.setScale({0.4f, 0.4f}); fP2.back().sprite.setColor({200,200,255});
    fP2.emplace_back(tSub, "Submarino B"); fP2.back().sprite.setPosition({700.f, 500.f}); fP2.back().sprite.setScale({0.4f, 0.4f}); fP2.back().sprite.setColor({200,200,255});

    // --- UI GLOBALES ---
    Boton btnAtacar(font, "ATACAR", {200, 50, 50});
    Boton btnMover(font, "MOVER", {50, 150, 50});
    sf::Text txtAyuda(font);
    txtAyuda.setString("FLECHAS: Mover | ENTER: Confirmar | ESC: Cancelar");
    txtAyuda.setCharacterSize(16); txtAyuda.setOutlineThickness(2.f);

    EstadoJuego estado = MENSAJE_P1;
    int sel = -1;       // Índice seleccionado
    bool moviendo = false;

    // === BUCLE PRINCIPAL ===
    while (window.isOpen()) {
        // Puntero a la flota actual (switch rápido)
        auto* flotaActiva = (estado == TURNO_P1) ? &fP1 : (estado == TURNO_P2 ? &fP2 : nullptr);

        // --- 1. GESTIÓN DE EVENTOS ---
        while (const auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            // Mensajes de Turno (Click para continuar)
            if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
                if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) 
                    estado = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                continue;
            }

            // Teclado (Lógica de control)
            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::Escape) {
                    moviendo = false; 
                    if (!moviendo) sel = -1; 
                    btnMover.resetColor();
                }
                if (k->code == sf::Keyboard::Key::Enter && moviendo) {
                    moviendo = false; sel = -1; btnMover.resetColor();
                    estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1; // Pasar turno
                }
            }

            // Mouse (Clicks)
            if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    sf::Vector2f pos = window.mapPixelToCoords(m->position);

                    // Click en Botones
                    if (sel != -1) {
                        if (btnAtacar.esClickeado(pos)) {
                            moviendo = false; sel = -1; btnMover.resetColor();
                            estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                        } 
                        else if (btnMover.esClickeado(pos)) {
                            if (moviendo) { // Confirmar
                                moviendo = false; sel = -1; btnMover.resetColor();
                                estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                            } else { // Activar
                                moviendo = true; btnMover.setColor({255, 140, 0});
                            }
                        }
                    }

                    // Click en Barcos (Selección)
                    if (!btnAtacar.esClickeado(pos) && !btnMover.esClickeado(pos) && flotaActiva) {
                        bool hit = false;
                        for (size_t i = 0; i < flotaActiva->size(); ++i) {
                            if ((*flotaActiva)[i].sprite.getGlobalBounds().contains(pos)) {
                                sel = (int)i; moviendo = false; btnMover.resetColor(); hit = true;
                                break;
                            }
                        }
                        if (!hit && !moviendo) sel = -1;
                    }
                }
            }
        }

        // --- 2. UPDATE (Lógica Continua) ---
        offset[0] += 0.5f; offset[1] += 0.25f;
        if (offset[0] >= 1000) offset[0] = 0; if (offset[1] >= 700) offset[1] = 0;
        fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {1000, 700}));

        // Delegamos el movimiento al módulo MovementManager
        if (moviendo && sel != -1 && flotaActiva) {
            MovementManager::procesarInput(*flotaActiva, sel);
        }

        // Actualizar posición de botones UI
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

        // --- 3. DRAW (Renderizado) ---
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