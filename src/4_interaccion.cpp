#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

// Incluimos nuestros módulos (Asegúrate de tenerlos en la carpeta include/)
#include "EstadoJuego.hpp"
#include "BarcoEntity.hpp"
#include "Boton.hpp"

// --- FUNCIÓN AUXILIAR PARA COLISIONES ---
// Comprueba si el sprite chocaría con algún otro barco de la flota
bool hayColision(sf::Sprite& spriteMoviendose, sf::Vector2f desplazamiento, 
                 const std::vector<BarcoEntity>& flota, int indiceIgnorar) {
    
    // 1. Crear una copia temporal del "rectángulo" (Hitbox) en la futura posición
    sf::FloatRect futuroBounds = spriteMoviendose.getGlobalBounds();
    futuroBounds.position += desplazamiento; // SFML 3: .position en lugar de .left/.top

    // 2. Revisar contra todos los compañeros
    for (size_t i = 0; i < flota.size(); ++i) {
        if ((int)i == indiceIgnorar) continue; // No chocar consigo mismo

        // Verificar intersección manual (AABB) para ser compatible con cualquier versión
        sf::FloatRect otro = flota[i].sprite.getGlobalBounds();
        
        // Lógica AABB: Si NO hay espacio entre ellos, hay colisión
        bool colisionX = futuroBounds.position.x < otro.position.x + otro.size.x &&
                         futuroBounds.position.x + futuroBounds.size.x > otro.position.x;
        bool colisionY = futuroBounds.position.y < otro.position.y + otro.size.y &&
                         futuroBounds.position.y + futuroBounds.size.y > otro.position.y;

        if (colisionX && colisionY) {
            return true; // ¡PUM! Chocó
        }
    }
    return false; // Camino libre
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 700}), "Batalla Naval - Colisiones (SFML 3.0)");
    window.setFramerateLimit(60);

    // --- CARGAR RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture texDestructor, texSubmarino, texMar, texPortaviones;
    if (!texDestructor.loadFromFile("assets/images/destructor .png")) return -1;
    if (!texSubmarino.loadFromFile("assets/images/submarino.png")) return -1;
    if (!texMar.loadFromFile("assets/images/mar.png")) return -1;
    // Usaremos el destructor como "falso portaviones" si no tienes la imagen, o cárgalo si existe
    if (!texPortaviones.loadFromFile("assets/images/portaviones.png")) texPortaviones = texDestructor;

    // --- FONDO ---
    texMar.setRepeated(true);
    sf::Sprite fondo(texMar);
    float offsetX = 0.f, offsetY = 0.f;
    fondo.setTextureRect(sf::IntRect({0, 0}, {1000, 700}));

    // --- FLOTAS (Ahora con 2 barcos cada una) ---
    std::vector<BarcoEntity> flotaP1;
    
    // Barco 1 P1: Destructor
    flotaP1.emplace_back(texDestructor, "Destructor P1");
    flotaP1.back().sprite.setScale({0.3f, 0.3f});
    flotaP1.back().sprite.setPosition({100.f, 200.f});
    
    // Barco 2 P1: Portaviones (Más grande)
    flotaP1.emplace_back(texPortaviones, "Portaviones P1");
    flotaP1.back().sprite.setScale({0.5f, 0.5f});
    flotaP1.back().sprite.setPosition({100.f, 400.f}); // Posición diferente para no chocar al inicio


    std::vector<BarcoEntity> flotaP2;
    
    // Barco 1 P2: Submarino Alpha
    flotaP2.emplace_back(texSubmarino, "Submarino Alpha");
    flotaP2.back().sprite.setScale({0.4f, 0.4f});
    flotaP2.back().sprite.setPosition({700.f, 200.f});
    flotaP2.back().sprite.setColor(sf::Color(200, 200, 255)); // Tinte azulado

    // Barco 2 P2: Submarino Beta
    flotaP2.emplace_back(texSubmarino, "Submarino Beta");
    flotaP2.back().sprite.setScale({0.4f, 0.4f});
    flotaP2.back().sprite.setPosition({700.f, 500.f});
    flotaP2.back().sprite.setColor(sf::Color(200, 200, 255));

    // --- INTERFAZ ---
    sf::Color cRojo(200, 50, 50), cVerde(50, 150, 50), cNaranja(255, 140, 0);
    Boton btnAtacar(font, "ATACAR", cRojo);
    Boton btnMover(font, "MOVER", cVerde);

    sf::Text txtMensaje(font), txtSub(font);
    txtMensaje.setCharacterSize(50); 
    txtMensaje.setOutlineThickness(3.f);
    txtSub.setCharacterSize(20); 
    txtSub.setFillColor(sf::Color::Yellow);
    txtSub.setString("(Cualquier tecla para continuar)");

    // --- VARIABLES DE ESTADO ---
    EstadoJuego estado = MENSAJE_P1;
    int seleccionado = -1;
    bool moviendo = false;

    while (window.isOpen()) {
        std::vector<BarcoEntity>* flotaActiva = (estado == TURNO_P1) ? &flotaP1 : (estado == TURNO_P2 ? &flotaP2 : nullptr);

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            // PANTALLAS DE MENSAJE (Cambio de turno)
            if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
                if (event->is<sf::Event::MouseButtonPressed>() || event->is<sf::Event::KeyPressed>()) {
                    estado = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                }
                continue;
            }

            // INPUT JUEGO
            if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::Escape) {
                    if (moviendo) {
                        // Cancelar movimiento no pasa turno, solo deja de mover
                        moviendo = false;
                        btnMover.resetColor();
                    } else {
                        seleccionado = -1;
                    }
                }
            }

            if (const auto* m = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mPos = window.mapPixelToCoords(m->position);

                    if (seleccionado != -1) {
                        // --- LÓGICA DE BOTONES ---
                        if (btnAtacar.esClickeado(mPos)) {
                            std::cout << ">>> ATAQUE REALIZADO: Turno finalizado." << std::endl;
                            
                            // RESETEAR VARIABLES Y CAMBIAR TURNO
                            moviendo = false;
                            seleccionado = -1;
                            btnMover.resetColor();
                            estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                        } 
                        else if (btnMover.esClickeado(mPos)) {
                            // Toggle movimiento
                            moviendo = !moviendo;
                            if (moviendo) btnMover.setColor(cNaranja);
                            else btnMover.resetColor();
                        }
                    }

                    // --- SELECCIÓN DE BARCOS ---
                    // Solo permitimos seleccionar si no hicimos clic en un botón
                    if (!btnAtacar.esClickeado(mPos) && !btnMover.esClickeado(mPos)) {
                        bool hit = false;
                        if (flotaActiva) {
                            for (size_t i = 0; i < flotaActiva->size(); ++i) {
                                if ((*flotaActiva)[i].sprite.getGlobalBounds().contains(mPos)) {
                                    seleccionado = (int)i;
                                    moviendo = false; // Al cambiar de barco, reseteamos mover
                                    btnMover.resetColor();
                                    hit = true; 
                                    break;
                                }
                            }
                        }
                        if (!hit) seleccionado = -1;
                    }
                }
            }
        }

        // --- UPDATE ---
        offsetX += 0.5f; offsetY += 0.25f;
        if (offsetX >= 1000) offsetX = 0; if (offsetY >= 700) offsetY = 0;
        fondo.setTextureRect(sf::IntRect({(int)offsetX, (int)offsetY}, {1000, 700}));

        // --- MOVIMIENTO CON COLISIÓN ---
        if (moviendo && seleccionado != -1 && flotaActiva) {
            float v = 3.f;
            sf::Vector2f d(0.f,0.f);

            // Detectar dirección
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) d.x += v;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) d.x -= v;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) d.y -= v;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) d.y += v;

            // Solo intentar mover si hay desplazamiento
            if (d.x != 0 || d.y != 0) {
                // Verificar colisión ANTES de mover
                if (!hayColision((*flotaActiva)[seleccionado].sprite, d, *flotaActiva, seleccionado)) {
                    (*flotaActiva)[seleccionado].sprite.move(d);
                } else {
                    // Opcional: Feedback visual de colisión (ej: consola)
                    // std::cout << "Bloqueado!" << std::endl;
                }
            }
        }

        // Posicionar Botones debajo del barco seleccionado
        if (seleccionado != -1 && flotaActiva) {
            auto b = (*flotaActiva)[seleccionado].sprite.getGlobalBounds();
            sf::Vector2f p = (*flotaActiva)[seleccionado].sprite.getPosition();
            float yBtn = p.y + b.size.y + 10.f;
            btnAtacar.setPosition({p.x, yBtn});
            btnMover.setPosition({p.x + 110.f, yBtn});
        }

        // --- DRAW ---
        window.clear();
        window.draw(fondo);

        // Caso 1: Pantallas de Mensaje
        if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
            sf::RectangleShape dim({1000.f, 700.f});
            dim.setFillColor(sf::Color(0,0,0,200));
            window.draw(dim);
            
            txtMensaje.setString(estado == MENSAJE_P1 ? "TURNO JUGADOR 1" : "TURNO JUGADOR 2");
            auto b = txtMensaje.getLocalBounds();
            txtMensaje.setPosition({(1000-b.size.x)/2, (700-b.size.y)/2 - 20});
            auto sb = txtSub.getLocalBounds();
            txtSub.setPosition({(1000-sb.size.x)/2, txtMensaje.getPosition().y + 80});
            
            window.draw(txtMensaje);
            window.draw(txtSub);
        } 
        // Caso 2: Jugando
        else if (flotaActiva) {
            // Dibujar flota activa
            for (size_t i=0; i<flotaActiva->size(); ++i) {
                auto& s = (*flotaActiva)[i].sprite;
                s.setColor((int)i == seleccionado ? sf::Color(255,200,200) : ((estado==TURNO_P2)?sf::Color(200,200,255):sf::Color::White));
                window.draw(s);
            }

            // Dibujar interfaz si hay selección
            if (seleccionado != -1) {
                sf::RectangleShape border;
                auto b = (*flotaActiva)[seleccionado].sprite.getGlobalBounds();
                border.setSize(b.size);
                border.setPosition((*flotaActiva)[seleccionado].sprite.getPosition());
                border.setFillColor(sf::Color::Transparent);
                border.setOutlineThickness(2.f);
                border.setOutlineColor(moviendo ? cNaranja : cVerde);
                window.draw(border);
                
                btnAtacar.dibujar(window);
                btnMover.dibujar(window);
            }
        }
        window.display();
    }
    return 0;
}