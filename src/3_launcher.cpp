#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

int main() {
    // Crear ventana del launcher
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Battleship - Launcher");
    window.setFramerateLimit(60);
    
    // Cargar la fuente Ring.ttf
    sf::Font fontRing;
    if (!fontRing.loadFromFile("assets/fonts/Ring.ttf")) {
        std::cerr << "Error al cargar la fuente Ring.ttf" << std::endl;
        return -1;
    }
    
    // Cargar la música de fondo
    sf::Music musica;
    if (!musica.openFromFile("assets/music/Aguas-de-Fuego.ogg")) {
        std::cerr << "Error al cargar la música Aguas-de-Fuego.ogg" << std::endl;
    } else {
        musica.play();
        musica.setLoop(true); // Repetir la música infinitamente
    }
    
    // Cargar la imagen de portada
    sf::Texture texturaPortada;
    if (!texturaPortada.loadFromFile("assets/images/portada.jpg")) {
        std::cerr << "Error al cargar portada.jpg" << std::endl;
    }
    sf::Sprite portada(texturaPortada);
    portada.setPosition(0, 0);
    // Escalar la portada para que ocupe el área azul claro (1000x350)
    portada.setScale(
        1000.0f / texturaPortada.getSize().x,
        350.0f / texturaPortada.getSize().y
    );
    
    // ============ CREAR ELEMENTOS DEL LAUNCHER ============
    
    // Crear botón "Iniciar Juego"
    sf::RectangleShape boton(sf::Vector2f(300, 80));
    boton.setFillColor(sf::Color(220, 20, 60)); // Rojo carmesí
    boton.setPosition((window.getSize().x - 300) / 2, 400);
    boton.setOutlineThickness(3);
    boton.setOutlineColor(sf::Color::White);
    
    // Texto del botón
    sf::Text textoBoton;
    textoBoton.setFont(fontRing);
    textoBoton.setCharacterSize(40);
    textoBoton.setFillColor(sf::Color::White);
    textoBoton.setString("INICIAR JUEGO");
    
    // Centrar texto en el botón
    sf::FloatRect botonBounds = textoBoton.getLocalBounds();
    textoBoton.setPosition(
        boton.getPosition().x + (boton.getSize().x - botonBounds.width) / 2,
        boton.getPosition().y + (boton.getSize().y - botonBounds.height) / 2 - 10
    );
    
    // Subtítulo
    sf::Text subtitulo;
    subtitulo.setFont(fontRing);
    subtitulo.setCharacterSize(24);
    subtitulo.setFillColor(sf::Color(150, 0, 20));
    subtitulo.setString("PREPARATE PARA LA BATALLA NAVAL");
    subtitulo.setStyle(sf::Text::Bold);

    sf::FloatRect subtituloBounds = subtitulo.getLocalBounds();
    subtitulo.setPosition(
        (window.getSize().x - subtituloBounds.width) / 2,
        210
    );
    
    // Texto de instrucciones
    sf::Text instrucciones;
    instrucciones.setFont(fontRing);
    instrucciones.setCharacterSize(16);
    instrucciones.setFillColor(sf::Color(200, 200, 200));
    instrucciones.setString("Haz clic en el boton para comenzar\nO presiona ENTER\n\nPresiona ESC para salir");
    instrucciones.setPosition(250, 550);
    
    std::cout << "Launcher iniciado. Reproduciendo música..." << std::endl;
    
    // Loop principal del launcher
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // Cerrar con ESC
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                // Iniciar juego con ENTER
                if (event.key.code == sf::Keyboard::Return) {
                    std::cout << "¡Iniciando juego!" << std::endl;
                    window.close();
                }
            }
            
            // Detectar clic en el botón
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosf(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                
                if (boton.getGlobalBounds().contains(mousePosf)) {
                    std::cout << "¡Iniciando juego!" << std::endl;
                    window.close();
                }
            }
        }
        
        // Efecto visual: cambiar color del botón cuando el ratón está encima
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosf(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        
        if (boton.getGlobalBounds().contains(mousePosf)) {
            boton.setFillColor(sf::Color(255, 69, 0)); // Naranja más brillante
            textoBoton.setFillColor(sf::Color::Yellow);
        } else {
            boton.setFillColor(sf::Color(220, 20, 60)); // Rojo original
            textoBoton.setFillColor(sf::Color::White);
        }
        
        // Limpiar ventana con color de fondo (azul océano oscuro)
        window.clear(sf::Color(25, 50, 100));
        
        // Dibujar la portada en el área superior
        window.draw(portada);
        
        // Dibujar fondo oscuro en la parte inferior
        sf::RectangleShape fondo2(sf::Vector2f(1000, 350));
        fondo2.setFillColor(sf::Color(25, 50, 100));
        fondo2.setPosition(0, 350);
        window.draw(fondo2);
        
        // Dibujar elementos
        window.draw(subtitulo);
        window.draw(boton);
        window.draw(textoBoton);
        window.draw(instrucciones);
        
        // Mostrar ventana
        window.display();
    }
    
    // Detener la música al cerrar
    musica.stop();
    std::cout << "Launcher cerrado." << std::endl;
    
    return 0;
}
