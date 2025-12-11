#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

// --- ESTADOS DEL JUEGO ---
enum EstadoJuego {
    MENSAJE_P1, // Muestra "Turno Jugador 1" (oculta todo)
    TURNO_P1,   // Jugador 1 ve sus barcos y juega
    MENSAJE_P2, // Muestra "Turno Jugador 2" (oculta todo)
    TURNO_P2    // Jugador 2 ve sus barcos y juega
};

// Estructura de Barco
struct BarcoEntity {
    sf::Sprite sprite;
    std::string nombre;
    BarcoEntity(const sf::Texture& texture, std::string name) 
        : sprite(texture), nombre(name) {}
};

// Configuración de botones
void configurarBoton(sf::RectangleShape& boton, sf::Text& texto, const sf::Font& fuente, 
                    std::string str, sf::Color colorBase) {
    boton.setSize({100.f, 40.f});
    boton.setFillColor(colorBase);
    boton.setOutlineThickness(2.f);
    boton.setOutlineColor(sf::Color::White);

    texto.setFont(fuente);
    texto.setString(str);
    texto.setCharacterSize(18);
    texto.setFillColor(sf::Color::White);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 700}), "Batalla Naval - Turnos (SFML 3.0)");
    window.setFramerateLimit(60);

    // --- CARGAR RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture texDestructor, texSubmarino, texMar;
    if (!texDestructor.loadFromFile("assets/images/destructor .png")) return -1;
    if (!texSubmarino.loadFromFile("assets/images/submarino.png")) return -1;
    if (!texMar.loadFromFile("assets/images/mar.png")) return -1;

    // --- FONDO ANIMADO ---
    texMar.setRepeated(true);
    sf::Sprite fondo(texMar);
    float offsetX = 0.0f, offsetY = 0.0f;
    fondo.setTextureRect(sf::IntRect({0, 0}, {1000, 700}));

    // --- CREAR FLOTAS SEPARADAS ---
    std::vector<BarcoEntity> flotaP1;
    std::vector<BarcoEntity> flotaP2;

    // Flota P1 (Izquierda - Destructores)
    BarcoEntity b1(texDestructor, "Destructor P1");
    b1.sprite.setScale({0.3f, 0.3f});
    b1.sprite.setPosition({100.f, 200.f});
    flotaP1.push_back(b1);

    // Flota P2 (Derecha - Submarinos)
    // Usamos setColor para diferenciarlos un poco (ej: un tono más oscuro)
    BarcoEntity b2(texSubmarino, "Submarino P2");
    b2.sprite.setScale({0.4f, 0.4f});
    b2.sprite.setPosition({700.f, 400.f}); 
    b2.sprite.setColor(sf::Color(200, 200, 255)); 
    flotaP2.push_back(b2);

    // --- INTERFAZ ---
    sf::RectangleShape btnAtacar, btnMover;
    sf::Text txtAtacar(font), txtMover(font);
    sf::Color colRojo(200, 50, 50), colVerde(50, 150, 50), colNaranja(255, 140, 0);

    configurarBoton(btnAtacar, txtAtacar, font, "ATACAR", colRojo);
    configurarBoton(btnMover, txtMover, font, "MOVER", colVerde);

    // Texto para los mensajes de turno
    sf::Text textoMensaje(font);
    textoMensaje.setCharacterSize(50);
    textoMensaje.setFillColor(sf::Color::White);
    textoMensaje.setOutlineColor(sf::Color::Black);
    textoMensaje.setOutlineThickness(3.f);

    sf::Text subMensaje(font);
    subMensaje.setCharacterSize(20);
    subMensaje.setString("(Haz clic para continuar)");
    subMensaje.setFillColor(sf::Color::Yellow);

    // --- VARIABLES DE ESTADO ---
    EstadoJuego estadoActual = MENSAJE_P1;
    int indiceSeleccionado = -1;
    bool moviendoBarco = false;

    while (window.isOpen()) {
        // Obtenemos referencia a la flota activa para no duplicar código
        std::vector<BarcoEntity>* flotaActiva = nullptr;
        if (estadoActual == TURNO_P1) flotaActiva = &flotaP1;
        if (estadoActual == TURNO_P2) flotaActiva = &flotaP2;

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            // --- LÓGICA: PANTALLAS DE MENSAJE ---
            if (estadoActual == MENSAJE_P1 || estadoActual == MENSAJE_P2) {
                // Cualquier clic o tecla avanza al juego
                if (event->is<sf::Event::MouseButtonPressed>() || event->is<sf::Event::KeyPressed>()) {
                    if (estadoActual == MENSAJE_P1) estadoActual = TURNO_P1;
                    else estadoActual = TURNO_P2;
                }
                continue; // Saltamos el resto de lógica si estamos en mensaje
            }

            // --- LÓGICA: JUEGO ACTIVO (P1 o P2) ---
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Escape) {
                    if (moviendoBarco) {
                        // TERMINAR MOVIMIENTO -> CAMBIO DE TURNO
                        moviendoBarco = false;
                        indiceSeleccionado = -1;
                        btnMover.setFillColor(colVerde);
                        // Cambiar al mensaje del OTRO jugador
                        estadoActual = (estadoActual == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                    } else {
                        indiceSeleccionado = -1;
                    }
                }
            }

            if (const auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(mouseBtn->position);
                    bool clicInterfaz = false;

                    // 1. Clic en Botones
                    if (indiceSeleccionado != -1) {
                        if (btnAtacar.getGlobalBounds().contains(mousePos)) {
                            std::cout << "Pium Pium!" << std::endl;
                            clicInterfaz = true;
                        } 
                        else if (btnMover.getGlobalBounds().contains(mousePos)) {
                            clicInterfaz = true;
                            
                            // Si ya estaba moviendo y hago clic de nuevo -> TERMINO MOVIMIENTO
                            if (moviendoBarco) {
                                moviendoBarco = false;
                                indiceSeleccionado = -1;
                                btnMover.setFillColor(colVerde);
                                // CAMBIO DE TURNO
                                estadoActual = (estadoActual == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                            } else {
                                // Iniciar movimiento
                                moviendoBarco = true;
                                btnMover.setFillColor(colNaranja);
                            }
                        }
                    }

                    // 2. Selección de Barcos (Solo de la flota activa)
                    if (!clicInterfaz && !moviendoBarco) {
                        bool tocoBarco = false;
                        for (size_t i = 0; i < flotaActiva->size(); i++) {
                            if ((*flotaActiva)[i].sprite.getGlobalBounds().contains(mousePos)) {
                                indiceSeleccionado = (int)i;
                                tocoBarco = true;
                                break;
                            }
                        }
                        if (!tocoBarco) indiceSeleccionado = -1;
                    }
                }
            }
        }

        // --- UPDATE ---
        // Animar fondo
        offsetX += 0.5f; offsetY += 0.25f;
        if (offsetX >= (float)texMar.getSize().x) offsetX = 0;
        if (offsetY >= (float)texMar.getSize().y) offsetY = 0;
        fondo.setTextureRect(sf::IntRect({(int)offsetX, (int)offsetY}, {1000, 700}));

        // Mover barco con flechas
        if (moviendoBarco && indiceSeleccionado != -1 && flotaActiva) {
            sf::Vector2f move(0.f, 0.f);
            float vel = 3.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) move.x += vel;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  move.x -= vel;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    move.y -= vel;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  move.y += vel;
            (*flotaActiva)[indiceSeleccionado].sprite.move(move);
        }

        // Posicionar botones
        if (indiceSeleccionado != -1 && flotaActiva) {
            auto bounds = (*flotaActiva)[indiceSeleccionado].sprite.getGlobalBounds();
            sf::Vector2f pos = (*flotaActiva)[indiceSeleccionado].sprite.getPosition();
            float bx = pos.x;
            float by = pos.y + bounds.size.y + 10.f;

            btnAtacar.setPosition({bx, by});
            btnMover.setPosition({bx + 110.f, by});
            txtAtacar.setPosition({bx + 15.f, by + 8.f});
            txtMover.setPosition({bx + 110.f + 20.f, by + 8.f});
        }

        // --- DIBUJAR ---
        window.clear();
        window.draw(fondo);

        // Caso 1: Pantallas de Mensaje
        if (estadoActual == MENSAJE_P1 || estadoActual == MENSAJE_P2) {
            // Fondo oscuro semitransparente
            sf::RectangleShape overlay({1000.f, 700.f});
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);

            std::string msg = (estadoActual == MENSAJE_P1) ? "JUGADOR 1" : "JUGADOR 2";
            textoMensaje.setString(msg);
            
            // Centrar texto
            auto bounds = textoMensaje.getLocalBounds();
            textoMensaje.setPosition({(1000.f - bounds.size.x)/2.f, (700.f - bounds.size.y)/2.f - 20.f});
            
            auto subBounds = subMensaje.getLocalBounds();
            subMensaje.setPosition({(1000.f - subBounds.size.x)/2.f, textoMensaje.getPosition().y + 80.f});

            window.draw(textoMensaje);
            window.draw(subMensaje);
        }
        // Caso 2: Juego (Dibujar solo flota activa)
        else if (flotaActiva) {
            // Dibujar barcos
            for (size_t i = 0; i < flotaActiva->size(); i++) {
                if ((int)i == indiceSeleccionado) (*flotaActiva)[i].sprite.setColor(sf::Color(255, 200, 200));
                else (*flotaActiva)[i].sprite.setColor(sf::Color::White);
                window.draw((*flotaActiva)[i].sprite);
            }

            // Dibujar UI si hay selección
            if (indiceSeleccionado != -1) {
                sf::RectangleShape marco;
                auto bounds = (*flotaActiva)[indiceSeleccionado].sprite.getGlobalBounds();
                marco.setSize(bounds.size);
                marco.setPosition((*flotaActiva)[indiceSeleccionado].sprite.getPosition());
                marco.setFillColor(sf::Color::Transparent);
                marco.setOutlineColor(moviendoBarco ? colNaranja : colVerde);
                marco.setOutlineThickness(2.f);
                window.draw(marco);

                window.draw(btnAtacar);
                window.draw(txtAtacar);
                window.draw(btnMover);
                window.draw(txtMover);
            }
        }

        window.display();
    }
    return 0;
}