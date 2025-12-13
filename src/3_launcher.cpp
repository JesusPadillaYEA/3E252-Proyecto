#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <vector>

// Función auxiliar para ejecutar un programa verificando múltiples rutas posibles
void ejecutarPrograma(const std::vector<std::string>& rutas) {
    for (const auto& ruta : rutas) {
        // Extraer ruta sin argumentos para verificación
        std::string rutaSinArgs = ruta;
        size_t espacio = rutaSinArgs.find(' ');
        if (espacio != std::string::npos) {
            rutaSinArgs = rutaSinArgs.substr(0, espacio);
        }
        
        if (std::filesystem::exists(rutaSinArgs)) {
            std::system(("start " + ruta).c_str());
            return;
        }
    }
}

// Función auxiliar para iniciar el tutorial
void iniciarTutorial(sf::RenderWindow& window, sf::Music& musica) {
    musica.stop();
    window.close();

    // Intenta ejecutar main.exe en múltiples ubicaciones posibles con flag --tutorial
    ejecutarPrograma({"./bin/main.exe --tutorial", "./main.exe --tutorial", "bin\\main.exe --tutorial", "main.exe --tutorial"});
}

// Función auxiliar para iniciar el juego y cerrar el launcher
void iniciarJuego(sf::RenderWindow& window, sf::Music& musica) {
    musica.stop();
    window.close();

    // Intenta ejecutar main.exe en múltiples ubicaciones posibles
    ejecutarPrograma({"./bin/main.exe", "./main.exe", "bin\\main.exe", "main.exe"}); 
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
    boton.setPosition({(window.getSize().x - 300.f) / 2.f, 350.f});
    boton.setOutlineThickness(3.f);
    boton.setOutlineColor(sf::Color::White);
    
    // Texto del botón
    sf::Text textoBoton(fontRing);
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
    
    // Crear botón "Tutorial"
    sf::RectangleShape botonTutorial({300.f, 80.f});
    botonTutorial.setFillColor(sf::Color(70, 130, 180)); // Azul acero
    botonTutorial.setPosition({(window.getSize().x - 300.f) / 2.f, 480.f});
    botonTutorial.setOutlineThickness(3.f);
    botonTutorial.setOutlineColor(sf::Color::White);
    
    // Texto del botón Tutorial
    sf::Text textoBotonTutorial(fontRing);
    textoBotonTutorial.setCharacterSize(40);
    textoBotonTutorial.setFillColor(sf::Color::White);
    textoBotonTutorial.setString("TUTORIAL");
    
    // Centrar texto en botón tutorial
    sf::FloatRect botonTutorialBounds = textoBotonTutorial.getLocalBounds();
    sf::Vector2f posBotonTutorial = botonTutorial.getPosition();
    sf::Vector2f sizeBotonTutorial = botonTutorial.getSize();
    
    textoBotonTutorial.setPosition({
        posBotonTutorial.x + (sizeBotonTutorial.x - botonTutorialBounds.size.x) / 2.f,
        posBotonTutorial.y + (sizeBotonTutorial.y - botonTutorialBounds.size.y) / 2.f - 10.f
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
                    if (botonTutorial.getGlobalBounds().contains(mousePos)) {
                        iniciarTutorial(window, musica);
                        return 0; // Terminar launcher
                    }
                }
            }
        }
        
        // Efecto visual: Hover sobre los botones
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        if (boton.getGlobalBounds().contains(mousePos)) {
            boton.setFillColor(sf::Color(255, 69, 0)); // Naranja más brillante
            textoBoton.setFillColor(sf::Color::Yellow);
        } else {
            boton.setFillColor(sf::Color(220, 20, 60)); // Rojo original
            textoBoton.setFillColor(sf::Color::White);
        }
        
        if (botonTutorial.getGlobalBounds().contains(mousePos)) {
            botonTutorial.setFillColor(sf::Color(100, 149, 237)); // Azul más brillante
            textoBotonTutorial.setFillColor(sf::Color::Yellow);
        } else {
            botonTutorial.setFillColor(sf::Color(70, 130, 180)); // Azul acero original
            textoBotonTutorial.setFillColor(sf::Color::White);
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
        window.draw(botonTutorial);
        window.draw(textoBotonTutorial);
        window.draw(instrucciones);
        
        window.display();
    }
    
    return 0;
}