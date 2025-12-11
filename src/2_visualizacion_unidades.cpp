#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <iostream>

// Estructura para representar una unidad en el juego
struct Unidad {
    std::string nombre;
    float x, y;
    sf::Sprite sprite;
    std::string ruta;
};

int main() {
    // Crear ventana
    sf::RenderWindow window(sf::VideoMode(1200, 700), "Vista Superior - Unidades del Juego");
    window.setFramerateLimit(60);
    
    // Vector para almacenar todas las unidades
    std::vector<Unidad> unidades;
    
    // Cargar texturas
    
    // BARCOS
    sf::Texture textDestructor;
    if (!textDestructor.loadFromFile("assets/images/destructor .png")) {
        std::cerr << "Error al cargar destructor .png" << std::endl;
    }
    
    sf::Texture textSubmarino;
    if (!textSubmarino.loadFromFile("assets/images/submarino.png")) {
        std::cerr << "Error al cargar submarino.png" << std::endl;
    }
    
    sf::Texture textPortaviones;
    if (!textPortaviones.loadFromFile("assets/images/portaviones.png")) {
        std::cerr << "Error al cargar portaviones.png" << std::endl;
    }
    
    // AVIONES
    sf::Texture textAvion1;
    if (!textAvion1.loadFromFile("assets/images/avion1.png")) {
        std::cerr << "Error al cargar avion1.png" << std::endl;
    }
    
    sf::Texture textAvion2;
    if (!textAvion2.loadFromFile("assets/images/avion2.png")) {
        std::cerr << "Error al cargar avion2.png" << std::endl;
    }
    
    // UAV
    sf::Texture textUAV;
    if (!textUAV.loadFromFile("assets/images/UAV.png")) {
        std::cerr << "Error al cargar UAV.png" << std::endl;
    }
    
    // MAR (fondo)
    sf::Texture textMar;
    if (!textMar.loadFromFile("assets/images/mar.png")) {
        std::cerr << "Error al cargar mar.png" << std::endl;
    }
    textMar.setRepeated(true);
    sf::Sprite fondoMar(textMar);
    fondoMar.setPosition(0, 0);
    fondoMar.setScale(1200.0f / textMar.getSize().x, 700.0f / textMar.getSize().y);
    
    // ============ CREAR UNIDADES ============
    
    // Tamaño de celda en píxeles (para escalar las unidades)
    const float cellSize = 10.0f;
    
    // DESTRUCTORES (2x3)
    Unidad destructor1;
    destructor1.nombre = "Destructor 1";
    destructor1.x = 100;
    destructor1.y = 100;
    destructor1.sprite.setTexture(textDestructor);
    destructor1.sprite.setPosition(destructor1.x, destructor1.y);
    destructor1.sprite.setScale(0.3f, 0.3f); // 2x3 celdas
    unidades.push_back(destructor1);
    
    // SUBMARINOS (2x3)
    Unidad submarino1;
    submarino1.nombre = "Submarino 1";
    submarino1.x = 200;
    submarino1.y = 200;
    submarino1.sprite.setTexture(textSubmarino);
    submarino1.sprite.setPosition(submarino1.x, submarino1.y);
    submarino1.sprite.setScale(0.45f, 0.45f); // 2x3 celdas
    unidades.push_back(submarino1);
    
    // PORTAVIONES (3x5)
    Unidad portaviones1;
    portaviones1.nombre = "Portaviones 1";
    portaviones1.x = 300;
    portaviones1.y = 300;
    portaviones1.sprite.setTexture(textPortaviones);
    portaviones1.sprite.setPosition(portaviones1.x, portaviones1.y);
    portaviones1.sprite.setScale(0.9f, 0.7f); // 3x5 celdas
    unidades.push_back(portaviones1);
    
    // AVIONES (2x2)
    Unidad avion1;
    avion1.nombre = "Avion 1";
    avion1.x = 400;
    avion1.y = 400;
    avion1.sprite.setTexture(textAvion1);
    avion1.sprite.setPosition(avion1.x, avion1.y);
    avion1.sprite.setScale(0.2f, 0.2f); // 2x2 celdas
    unidades.push_back(avion1);
    
    Unidad avion2;
    avion2.nombre = "Avion 2";
    avion2.x = 500;
    avion2.y = 500;
    avion2.sprite.setTexture(textAvion2);
    avion2.sprite.setPosition(avion2.x, avion2.y);
    avion2.sprite.setScale(0.17f, 0.17f); // 2x2 celdas
    unidades.push_back(avion2);
    
    // UAV (1x1)
    Unidad uav1;
    uav1.nombre = "UAV 1";
    uav1.x = 600;
    uav1.y = 600;
    uav1.sprite.setTexture(textUAV);
    uav1.sprite.setPosition(uav1.x, uav1.y);
    uav1.sprite.setScale(0.15f, 0.15f); // 1x1 celdas
    unidades.push_back(uav1);
    
    // Variables para la selección
    int unidadSeleccionada = -1;
    sf::Font font;
    
    // Loop principal
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // Detectar clic del mouse para seleccionar unidades
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosf(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                
                for (size_t i = 0; i < unidades.size(); i++) {
                    if (unidades[i].sprite.getGlobalBounds().contains(mousePosf)) {
                        unidadSeleccionada = i;
                        std::cout << "Seleccionada: " << unidades[i].nombre << std::endl;
                        break;
                    }
                }
            }
        }
        
        // Mover la unidad seleccionada con teclas
        if (unidadSeleccionada != -1) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                unidades[unidadSeleccionada].sprite.move(2.0f, 0.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                unidades[unidadSeleccionada].sprite.move(-2.0f, 0.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                unidades[unidadSeleccionada].sprite.move(0.0f, -2.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                unidades[unidadSeleccionada].sprite.move(0.0f, 2.0f);
            }
        }
        
        // Limpiar ventana y dibujar
        window.clear();
        
        // Dibujar fondo de mar
        window.draw(fondoMar);
        
        // Dibujar todas las unidades
        for (size_t i = 0; i < unidades.size(); i++) {
            window.draw(unidades[i].sprite);
            
            // Dibujar un círculo rojo alrededor de la unidad seleccionada
            if (i == unidadSeleccionada) {
                sf::RectangleShape seleccion(sf::Vector2f(60.0f, 50.0f));
                seleccion.setFillColor(sf::Color::Transparent);
                seleccion.setOutlineThickness(2.0f);
                seleccion.setOutlineColor(sf::Color::Red);
                seleccion.setPosition(unidades[i].sprite.getPosition().x, 
                                    unidades[i].sprite.getPosition().y);
                window.draw(seleccion);
            }
        }
        
        // Dibujar información en pantalla
        sf::Text infoText;
        infoText.setFont(font);
        infoText.setCharacterSize(14);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(10, 10);
        infoText.setString("Vista Superior del Juego Naval\n\n"
                          "BARCOS:\n"
                          "- Destructor (rapido y armado)\n"
                          "- Submarino (sigiloso)\n"
                          "- Portaviones (grande, transporta aviones)\n\n"
                          "AERONAVES:\n"
                          "- Avion 1 y 2 (tripulados)\n"
                          "- UAV (Dron sin tripulacion)\n\n"
                          "Haz clic para seleccionar una unidad\n"
                          "Usa flechas para mover");
        window.draw(infoText);
        
        // Mostrar unidad seleccionada
        if (unidadSeleccionada != -1) {
            sf::Text selectedText;
            selectedText.setFont(font);
            selectedText.setCharacterSize(16);
            selectedText.setFillColor(sf::Color::Yellow);
            selectedText.setPosition(10, 300);
            selectedText.setString("Seleccionada: " + unidades[unidadSeleccionada].nombre);
            window.draw(selectedText);
        }
        
        window.display();
    }
    
    return 0;
}
