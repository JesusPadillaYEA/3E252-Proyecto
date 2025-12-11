#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// --- MÓDULOS ---
#include "EstadoJuego.hpp"
#include "Boton.hpp"
#include "CollisionManager.hpp"
#include "MovementManager.hpp"
#include "UIManager.hpp"
#include "Jugador.hpp"
#include "GeneradorFlotas.hpp"
#include "RenderManager.hpp" // <--- NUEVO INCLUDE

int main() {
    const sf::Vector2u WINDOW_SIZE(1000, 700);
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Sistema de Vida (SFML 3.0)");
    window.setFramerateLimit(60);

    // --- CARGAR RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture tDest, tSub, tMar, tPort;
    bool assetsOk = true;
    if (!tDest.loadFromFile("assets/images/destructor .png")) assetsOk = false;
    if (!tSub.loadFromFile("assets/images/submarino.png")) assetsOk = false;
    if (!tMar.loadFromFile("assets/images/mar.png")) assetsOk = false;
    if (!tPort.loadFromFile("assets/images/portaviones.png")) tPort = tDest; 

    if (!assetsOk) return -1;

    // --- FONDO ---
    tMar.setRepeated(true);
    sf::Sprite fondo(tMar);
    float offset[2] = {0.f, 0.f};
    fondo.setTextureRect(sf::IntRect({0, 0}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

    // --- JUGADORES ---
    Jugador p1(1, {0.f, 0.f});
    Jugador p2(2, {700.f, 0.f});

    GeneradorFlotas::inicializarFlota(p1, tDest, tPort, tSub);
    GeneradorFlotas::inicializarFlota(p2, tDest, tPort, tSub);

    // --- UI GLOBALES ---
    Boton btnAtacar(font, "ATACAR", {200, 50, 50});
    Boton btnMover(font, "MOVER", {50, 150, 50});
    
    // Actualizamos texto de ayuda para incluir la tecla de prueba
    sf::Text txtAyuda(font);
    txtAyuda.setString("FLECHAS: Mover | K: Destruir (Debug) | ENTER: Confirmar");
    txtAyuda.setCharacterSize(16); txtAyuda.setOutlineThickness(2.f);

    EstadoJuego estado = MENSAJE_P1;
    int sel = -1;
    bool moviendo = false;

    while (window.isOpen()) {
        std::vector<BarcoEntity>* flotaActiva = nullptr;
        if (estado == TURNO_P1) flotaActiva = &p1.getFlota();
        else if (estado == TURNO_P2) flotaActiva = &p2.getFlota();

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
                
                // --- DEBUG: TECLA 'K' PARA MATAR BARCO ---
                if (k->code == sf::Keyboard::Key::K && sel != -1 && flotaActiva) {
                    std::cout << ">>> DESTRUYENDO BARCO: " << (*flotaActiva)[sel].nombre << std::endl;
                    (*flotaActiva)[sel].recibirDano(9999); // Daño masivo
                    sel = -1; // Deseleccionar inmediatamente
                    moviendo = false;
                    btnMover.resetColor();
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
                            // Aquí iría la lógica de disparo real en el futuro
                            std::cout << "Disparo realizado" << std::endl;
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
                            // --- NUEVO: NO SELECCIONAR BARCOS DESTRUIDOS ---
                            if ((*flotaActiva)[i].destruido) continue;

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

        // --- UPDATE ---
        offset[0] += 0.5f; offset[1] += 0.25f;
        if (offset[0] >= (float)WINDOW_SIZE.x) offset[0] = 0; 
        if (offset[1] >= (float)WINDOW_SIZE.y) offset[1] = 0;
        fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

        if (moviendo && sel != -1 && flotaActiva) {
            // Aseguramos que el barco seleccionado siga vivo (por seguridad)
            if (!(*flotaActiva)[sel].destruido) {
                MovementManager::procesarInput(*flotaActiva, sel, WINDOW_SIZE);
            } else {
                moviendo = false; sel = -1; // Si murió mientras se movía, cancelar
            }
        }

        // Posicionar Botones
        if (sel != -1 && flotaActiva && !(*flotaActiva)[sel].destruido) {
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
            // --- USAMOS EL NUEVO RENDER MANAGER ---
            // Él decide qué dibujar (si está vivo) y cómo (si está seleccionado)
            sf::Color colorBorde = moviendo ? sf::Color(255, 140, 0) : sf::Color(50, 150, 50);
            
            RenderManager::renderizarFlota(window, *flotaActiva, sel, estado, moviendo, colorBorde);

            // Dibujar botones solo si hay selección válida
            if (sel != -1 && !(*flotaActiva)[sel].destruido) {
                btnAtacar.dibujar(window);
                btnMover.dibujar(window);
                if (moviendo) window.draw(txtAyuda);
            }
        }
        window.display();
    }
    return 0;
}