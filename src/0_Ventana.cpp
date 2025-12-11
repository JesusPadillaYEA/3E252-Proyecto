#include <SFML/Graphics.hpp>
#include "GameWindow.hpp"
#include <iostream>

int main() {
    // Crear una ventana de 800x600 píxeles
    GameWindow gameWindow(800, 600, "Juego Naval - Ventana Principal");
    
    // Crear algunos elementos gráficos de prueba
    
    // Rectángulo azul (simular agua)
    sf::RectangleShape water(sf::Vector2f(800.0f, 600.0f));
    water.setFillColor(sf::Color(30, 144, 255)); // Azul océano
    water.setPosition(0, 0);
    
    // Círculo rojo (simular un barco)
    sf::CircleShape barco(30.0f);
    barco.setFillColor(sf::Color::Red);
    barco.setPosition(300, 200);
    
    // Triángulo verde (simular un avión)
    sf::CircleShape avion(20.0f, 3); // Triángulo
    avion.setFillColor(sf::Color::Green);
    avion.setPosition(400, 100);
    
    // Texto de bienvenida
    sf::Font font;
    // Nota: Para usar fuentes, debes cargar un archivo .ttf
    // if (!font.loadFromFile("assets/fonts/arial.ttf")) {
    //     std::cerr << "Error al cargar la fuente" << std::endl;
    // }
    
    sf::Text title;
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::White);
    title.setString("JUEGO NAVAL");
    title.setPosition(250, 20);
    // title.setFont(font);
    
    std::cout << "Ventana creada: 800x600" << std::endl;
    std::cout << "Presiona ESC o cierra la ventana para salir" << std::endl;
    
    // Loop principal
    while (gameWindow.isOpen()) {
        sf::Event event;
        while (gameWindow.pollEvent(event)) {
            // Cerrar ventana con ESC o botón X
            if (event.type == sf::Event::Closed) {
                gameWindow.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    gameWindow.close();
                }
            }
            
            // Mover el barco con las flechas
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Right) {
                    barco.move(10.0f, 0.0f);
                }
                if (event.key.code == sf::Keyboard::Left) {
                    barco.move(-10.0f, 0.0f);
                }
                if (event.key.code == sf::Keyboard::Up) {
                    barco.move(0.0f, -10.0f);
                }
                if (event.key.code == sf::Keyboard::Down) {
                    barco.move(0.0f, 10.0f);
                }
            }
        }
        
        // Limpiar la ventana
        gameWindow.clear();
        
        // Dibujar los elementos
        gameWindow.draw(water);
        gameWindow.draw(barco);
        gameWindow.draw(avion);
        // gameWindow.draw(title); // Descomentar si cargas la fuente
        
        // Mostrar la ventana
        gameWindow.display();
    }
    
    return 0;
}
