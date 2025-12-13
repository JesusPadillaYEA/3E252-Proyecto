#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib> // Necesario para ejecutar comandos del sistema (system)

// Función auxiliar para iniciar el juego y cerrar el launcher
void iniciarJuego(sf::RenderWindow& window, sf::Music& musica) {
    musica.stop();
    window.close();

    // Ejecuta el siguiente programa. 
    // "start" en Windows permite que el launcher termine mientras el juego se abre.
    // Si estás en Linux/Mac, quita "start " y usa "./4_interaccion"
    std::system("start ./main.exe"); 
}

int main() {
    // Crear ventana del launcher
    sf::RenderWindow window(sf::VideoMode({1000, 700}), "Battleship - Launcher");
    window.setFramerateLimit(60);
    
    // Cargar la fuente Ring.ttf
    sf::Font fontRing;
    if (!fontRing.openFromFile("assets/fonts/Ring.ttf")) {
        return -1;
    }
    
    // Cargar la música de fondo
    sf::Music musica;
    if (!musica.openFromFile("assets/music/Aguas-de-Fuego.ogg")) {
        // Error silencioso
    } else {
        musica.play();
        musica.setLooping(true); 
    }
    
    // Cargar la imagen de portada
    sf::Texture texturaPortada;
    if (!texturaPortada.loadFromFile("assets/images/portada.jpg")) {
        // Error silencioso
    }
    sf::Sprite portada(texturaPortada);
    portada.setPosition({0.f, 0.f});
    
    // Escalar la portada para que ocupe el área superior (1000x350)
    // Nota: SFML 3 usa vectores para setScale ({x, y})
    sf::Vector2u texSize = texturaPortada.getSize();
    portada.setScale({
        1000.0f / (float)texSize.x,
        350.0f / (float)texSize.y
    });
    
    // ============ CREAR ELEMENTOS DEL LAUNCHER ============
    
    // Crear botón "Iniciar Juego"
    sf::RectangleShape boton({300.f, 80.f});
    boton.setFillColor(sf::Color(220, 20, 60)); // Rojo carmesí
    boton.setPosition({(window.getSize().x - 300.f) / 2.f, 400.f});
    boton.setOutlineThickness(3.f);
    boton.setOutlineColor(sf::Color::White);
    
    // Texto del botón
    sf::Text textoBoton(fontRing); // Constructor directo en SFML 3
    textoBoton.setCharacterSize(40);
    textoBoton.setFillColor(sf::Color::White);
    textoBoton.setString("INICIAR JUEGO");
    
    // Centrar texto en el botón
    sf::FloatRect botonBounds = textoBoton.getLocalBounds();
    sf::Vector2f posBoton = boton.getPosition();
    sf::Vector2f sizeBoton = boton.getSize();
    
    textoBoton.setPosition({
        posBoton.x + (sizeBoton.x - botonBounds.size.x) / 2.f,
        posBoton.y + (sizeBoton.y - botonBounds.size.y) / 2.f - 10.f
    });
    
    // Subtítulo
    sf::Text subtitulo(fontRing);
    subtitulo.setCharacterSize(24);
    subtitulo.setFillColor(sf::Color(150, 0, 20));
    subtitulo.setString("PREPARATE PARA LA BATALLA NAVAL");
    subtitulo.setStyle(sf::Text::Bold);

    sf::FloatRect subtituloBounds = subtitulo.getLocalBounds();
    subtitulo.setPosition({
        (window.getSize().x - subtituloBounds.size.x) / 2.f,
        210.f
    });
    
    // Texto de instrucciones
    sf::Text instrucciones(fontRing);
    instrucciones.setCharacterSize(16);
    instrucciones.setFillColor(sf::Color(200, 200, 200));
    instrucciones.setString("Haz clic en el boton para comenzar\nO presiona ENTER\n\nPresiona ESC para salir");
    instrucciones.setPosition({250.f, 550.f});
    
    // Loop principal
    while (window.isOpen()) {
        // --- SFML 3.0 EVENT POLLING ---
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            // Teclado
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
                if (keyEvent->code == sf::Keyboard::Key::Enter) {
                    iniciarJuego(window, musica);
                    return 0; // Terminar launcher
                }
            }
            
            // Mouse Click
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    if (boton.getGlobalBounds().contains(mousePos)) {
                        iniciarJuego(window, musica);
                        return 0; // Terminar launcher
                    }
                }
            }
        }
        
        // Efecto visual: Hover sobre el botón
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        if (boton.getGlobalBounds().contains(mousePos)) {
            boton.setFillColor(sf::Color(255, 69, 0)); // Naranja más brillante
            textoBoton.setFillColor(sf::Color::Yellow);
        } else {
            boton.setFillColor(sf::Color(220, 20, 60)); // Rojo original
            textoBoton.setFillColor(sf::Color::White);
        }
        
        // Renderizado
        window.clear(sf::Color(25, 50, 100)); // Fondo azul oscuro
        
        window.draw(portada);
        
        sf::RectangleShape fondo2({1000.f, 350.f});
        fondo2.setFillColor(sf::Color(25, 50, 100));
        fondo2.setPosition({0.f, 350.f});
        window.draw(fondo2);
        
        window.draw(subtitulo);
        window.draw(boton);
        window.draw(textoBoton);
        window.draw(instrucciones);
        
        window.display();
    }
    
    return 0;
}