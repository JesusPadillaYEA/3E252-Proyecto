#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

// Incluimos nuestros nuevos archivos modulares
#include "EstadoJuego.hpp"
#include "BarcoEntity.hpp"
#include "Boton.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 700}), "Batalla Naval Modular (SFML 3.0)");
    window.setFramerateLimit(60);

    // --- CARGAR RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture texDestructor, texSubmarino, texMar;
    if (!texDestructor.loadFromFile("assets/images/destructor .png")) return -1;
    if (!texSubmarino.loadFromFile("assets/images/submarino.png")) return -1;
    if (!texMar.loadFromFile("assets/images/mar.png")) return -1;

    // --- FONDO ---
    texMar.setRepeated(true);
    sf::Sprite fondo(texMar);
    float offsetX = 0.f, offsetY = 0.f;
    fondo.setTextureRect(sf::IntRect({0, 0}, {1000, 700}));

    // --- FLOTAS ---
    std::vector<BarcoEntity> flotaP1;
    flotaP1.emplace_back(texDestructor, "Destructor P1");
    flotaP1[0].sprite.setScale({0.3f, 0.3f});
    flotaP1[0].sprite.setPosition({100.f, 200.f});

    std::vector<BarcoEntity> flotaP2;
    flotaP2.emplace_back(texSubmarino, "Submarino P2");
    flotaP2[0].sprite.setScale({0.4f, 0.4f});
    flotaP2[0].sprite.setPosition({700.f, 400.f});
    flotaP2[0].sprite.setColor(sf::Color(200, 200, 255));

    // --- INTERFAZ (Usando la nueva clase Boton) ---
    sf::Color cRojo(200, 50, 50), cVerde(50, 150, 50), cNaranja(255, 140, 0);
    Boton btnAtacar(font, "ATACAR", cRojo);
    Boton btnMover(font, "MOVER", cVerde);

    // Textos de estado
    sf::Text txtMensaje(font), txtSub(font);
    txtMensaje.setCharacterSize(50); 
    txtMensaje.setOutlineThickness(3.f);
    txtSub.setCharacterSize(20); 
    txtSub.setFillColor(sf::Color::Yellow);
    txtSub.setString("(Haz clic para continuar)");

    // --- VARIABLES DE ESTADO ---
    EstadoJuego estado = MENSAJE_P1;
    int seleccionado = -1;
    bool moviendo = false;

    while (window.isOpen()) {
        std::vector<BarcoEntity>* flotaActiva = (estado == TURNO_P1) ? &flotaP1 : (estado == TURNO_P2 ? &flotaP2 : nullptr);

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            // PANTALLAS DE MENSAJE
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
                        moviendo = false;
                        seleccionado = -1;
                        btnMover.resetColor();
                        estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                    } else seleccionado = -1;
                }
            }

            if (const auto* m = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mPos = window.mapPixelToCoords(m->position);

                    if (seleccionado != -1) {
                        if (btnAtacar.esClickeado(mPos)) {
                            std::cout << "Pium!" << std::endl;
                            // Atacar no cambia turno necesariamente, pero puedes decidirlo
                        } 
                        else if (btnMover.esClickeado(mPos)) {
                            if (moviendo) { // Confirmar movimiento -> Fin turno
                                moviendo = false;
                                seleccionado = -1;
                                btnMover.resetColor();
                                estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                            } else {
                                moviendo = true;
                                btnMover.setColor(cNaranja);
                            }
                        }
                    }

                    if (!moviendo) { // Seleccionar barco
                        bool hit = false;
                        if (flotaActiva) {
                            for (size_t i = 0; i < flotaActiva->size(); ++i) {
                                if ((*flotaActiva)[i].sprite.getGlobalBounds().contains(mPos)) {
                                    seleccionado = (int)i;
                                    hit = true; 
                                    break;
                                }
                            }
                        }
                        if (!hit && !btnAtacar.esClickeado(mPos) && !btnMover.esClickeado(mPos)) 
                            seleccionado = -1;
                    }
                }
            }
        }

        // --- UPDATE ---
        offsetX += 0.5f; offsetY += 0.25f;
        if (offsetX >= 1000) offsetX = 0; if (offsetY >= 700) offsetY = 0;
        fondo.setTextureRect(sf::IntRect({(int)offsetX, (int)offsetY}, {1000, 700}));

        if (moviendo && seleccionado != -1 && flotaActiva) {
            float v = 3.f;
            sf::Vector2f d(0.f,0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) d.x += v;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) d.x -= v;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) d.y -= v;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) d.y += v;
            (*flotaActiva)[seleccionado].sprite.move(d);
        }

        // Posicionar Botones
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

        if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
            sf::RectangleShape dim({1000.f, 700.f});
            dim.setFillColor(sf::Color(0,0,0,200));
            window.draw(dim);
            
            txtMensaje.setString(estado == MENSAJE_P1 ? "JUGADOR 1" : "JUGADOR 2");
            auto b = txtMensaje.getLocalBounds();
            txtMensaje.setPosition({(1000-b.size.x)/2, (700-b.size.y)/2 - 20});
            auto sb = txtSub.getLocalBounds();
            txtSub.setPosition({(1000-sb.size.x)/2, txtMensaje.getPosition().y + 80});
            
            window.draw(txtMensaje);
            window.draw(txtSub);
        } else if (flotaActiva) {
            for (size_t i=0; i<flotaActiva->size(); ++i) {
                auto& s = (*flotaActiva)[i].sprite;
                s.setColor((int)i == seleccionado ? sf::Color(255,200,200) : sf::Color::White);
                window.draw(s);
            }
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