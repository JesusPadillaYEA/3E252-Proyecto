#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

// Estructura para representar un barco
struct BarcoEntity {
    sf::Sprite sprite;
    std::string nombre;
    // Constructor obligatorio para SFML 3 (Sprite no puede estar vacío)
    BarcoEntity(const sf::Texture& texture, std::string name) 
        : sprite(texture), nombre(name) {
    }
};

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
    // 1. Crear ventana
    sf::RenderWindow window(sf::VideoMode({1000, 700}), "Movimiento Táctico - SFML 3.0");
    window.setFramerateLimit(60);

    // 2. Cargar Recursos
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) {
        std::cerr << "Error cargando fuente Ring.ttf" << std::endl;
        std::cout << "Presiona ENTER para salir..." << std::endl;
        std::cin.get(); 
        return -1;
    }

    sf::Texture texDestructor, texSubmarino, texMar;
    bool cargaOk = true;
    if (!texDestructor.loadFromFile("assets/images/destructor .png")) cargaOk = false;
    if (!texSubmarino.loadFromFile("assets/images/submarino.png")) cargaOk = false;
    if (!texMar.loadFromFile("assets/images/mar.png")) cargaOk = false;

    if (!cargaOk) {
        std::cerr << "Error cargando imagenes." << std::endl;
        std::cin.get();
        return -1;
    }

    // --- FONDO ANIMADO ---
    texMar.setRepeated(true);
    sf::Sprite fondo(texMar);
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    fondo.setTextureRect(sf::IntRect({0, 0}, {1000, 700}));

    // 3. Crear Flota
    std::vector<BarcoEntity> flota;

    BarcoEntity b1(texDestructor, "Destructor Alpha");
    b1.sprite.setScale({0.3f, 0.3f});
    b1.sprite.setPosition({150.f, 200.f});
    flota.push_back(b1);

    BarcoEntity b2(texSubmarino, "Submarino Bravo");
    b2.sprite.setScale({0.4f, 0.4f});
    b2.sprite.setPosition({600.f, 400.f});
    flota.push_back(b2);

    // 4. Configurar Botones (UI)
    sf::RectangleShape btnAtacar, btnMover;
    sf::Text txtAtacar(font), txtMover(font);

    sf::Color colorRojo = sf::Color(200, 50, 50);
    sf::Color colorVerde = sf::Color(50, 150, 50);
    sf::Color colorActivo = sf::Color(255, 165, 0); // Naranja para modo activo

    configurarBoton(btnAtacar, txtAtacar, font, "ATACAR", colorRojo);
    configurarBoton(btnMover, txtMover, font, "MOVER", colorVerde);

    // --- VARIABLES DE ESTADO ---
    int indiceSeleccionado = -1;
    bool moviendoBarco = false; // Indica si estamos en modo "mover con flechas"

    // Loop Principal
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Manejo del Teclado (Eventos de una sola vez)
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Escape) {
                    // ESC cancela movimiento o selección
                    if (moviendoBarco) {
                        moviendoBarco = false;
                        std::cout << "Movimiento cancelado." << std::endl;
                        btnMover.setFillColor(colorVerde); // Restaurar color
                    } else {
                        indiceSeleccionado = -1; // Deseleccionar
                    }
                }
            }

            // Manejo del Mouse
            if (const auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(mouseBtn->position);

                    // 1. Verificar clic en botones (SOLO si hay barco seleccionado)
                    bool clicEnInterfaz = false;
                    if (indiceSeleccionado != -1) {
                        if (btnAtacar.getGlobalBounds().contains(mousePos)) {
                            std::cout << ">>> " << flota[indiceSeleccionado].nombre << " disparo sus cañones!" << std::endl;
                            clicEnInterfaz = true;
                            // Al atacar, dejamos de mover
                            moviendoBarco = false;
                            btnMover.setFillColor(colorVerde);
                        } 
                        else if (btnMover.getGlobalBounds().contains(mousePos)) {
                            // TOGGLE MOVIMIENTO
                            moviendoBarco = !moviendoBarco;
                            if (moviendoBarco) {
                                std::cout << ">>> MODO MOVIMIENTO ACTIVADO: Usa las flechas." << std::endl;
                                btnMover.setFillColor(colorActivo); // Cambiar color a naranja
                            } else {
                                std::cout << ">>> Modo movimiento desactivado." << std::endl;
                                btnMover.setFillColor(colorVerde);
                            }
                            clicEnInterfaz = true;
                        }
                    }

                    // 2. Si no toqué botones, verificar selección de barcos
                    if (!clicEnInterfaz) {
                        bool clicEnBarco = false;
                        for (size_t i = 0; i < flota.size(); i++) {
                            if (flota[i].sprite.getGlobalBounds().contains(mousePos)) {
                                indiceSeleccionado = (int)i;
                                clicEnBarco = true;
                                moviendoBarco = false; // Resetear movimiento al cambiar barco
                                btnMover.setFillColor(colorVerde);
                                std::cout << "Seleccionado: " << flota[i].nombre << std::endl;
                                break;
                            }
                        }
                        // Si hice clic en el mar (y no estoy moviendo activamente), deseleccionar
                        if (!clicEnBarco && !moviendoBarco) {
                            indiceSeleccionado = -1;
                        }
                    }
                }
            }
        }

        // --- LÓGICA DE JUEGO (FRAME A FRAME) ---

        // 1. Mover Barco con Teclado
        if (indiceSeleccionado != -1 && moviendoBarco) {
            float velocidad = 3.0f;
            sf::Vector2f movimiento(0.f, 0.f);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) movimiento.x += velocidad;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  movimiento.x -= velocidad;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    movimiento.y -= velocidad;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  movimiento.y += velocidad;

            // Mover el sprite
            flota[indiceSeleccionado].sprite.move(movimiento);
        }

        // 2. Actualizar Posición de los Botones (Deben seguir al barco)
        if (indiceSeleccionado != -1) {
            auto bounds = flota[indiceSeleccionado].sprite.getGlobalBounds();
            sf::Vector2f pos = flota[indiceSeleccionado].sprite.getPosition();
            
            // Calculamos posición debajo del barco
            float bx = pos.x;
            float by = pos.y + bounds.size.y + 10.f;

            // Aplicamos posición
            btnAtacar.setPosition({bx, by});
            btnMover.setPosition({bx + 110.f, by});

            // Centrar textos en botones
            txtAtacar.setPosition({bx + 15.f, by + 8.f});
            txtMover.setPosition({bx + 110.f + 20.f, by + 8.f});
        }

        // 3. Animar Fondo
        offsetX += 0.5f;
        offsetY += 0.25f;
        if (offsetX >= (float)texMar.getSize().x) offsetX = 0.0f;
        if (offsetY >= (float)texMar.getSize().y) offsetY = 0.0f;
        fondo.setTextureRect(sf::IntRect({(int)offsetX, (int)offsetY}, {1000, 700}));


        // --- DIBUJAR ---
        window.clear();
        window.draw(fondo);

        // Dibujar barcos
        for (size_t i = 0; i < flota.size(); i++) {
            // Efecto visual de selección
            if ((int)i == indiceSeleccionado) {
                flota[i].sprite.setColor(sf::Color(255, 200, 200)); // Rojizo
            } else {
                flota[i].sprite.setColor(sf::Color::White);
            }
            window.draw(flota[i].sprite);
        }

        // Dibujar UI (Solo si hay selección)
        if (indiceSeleccionado != -1) {
            // Marco verde alrededor del barco
            sf::RectangleShape marco;
            auto bounds = flota[indiceSeleccionado].sprite.getGlobalBounds();
            marco.setSize(bounds.size);
            marco.setPosition(flota[indiceSeleccionado].sprite.getPosition());
            marco.setFillColor(sf::Color::Transparent);
            
            // Si está moviéndose, marco naranja, si no, verde
            if (moviendoBarco) marco.setOutlineColor(colorActivo);
            else marco.setOutlineColor(sf::Color::Green);
            
            marco.setOutlineThickness(2.f);
            window.draw(marco);

            // Botones
            window.draw(btnAtacar);
            window.draw(txtAtacar);
            window.draw(btnMover);
            window.draw(txtMover);

            // Texto de ayuda si se está moviendo
            if (moviendoBarco) {
                sf::Text ayuda(font);
                ayuda.setString("Usa las FLECHAS para mover\nPresiona ESC para terminar");
                ayuda.setCharacterSize(14);
                ayuda.setFillColor(sf::Color::White);
                ayuda.setOutlineColor(sf::Color::Black);
                ayuda.setOutlineThickness(1.f);
                ayuda.setPosition({btnMover.getPosition().x, btnMover.getPosition().y + 45.f});
                window.draw(ayuda);
            }
        }

        window.display();
    }
    return 0;
}