#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

// Estructura para representar una celda del mapa
struct MapCell {
    int x;
    int y;
    sf::RectangleShape shape;
};

int main() {
    // Dimensiones del mapa
    const int MAP_WIDTH = 100;
    const int MAP_HEIGHT = 100;
    const float CELL_SIZE = 10.0f; // Tamaño de cada celda en píxeles
    
    // Crear vector 2D para almacenar el mapa
    std::vector<std::vector<MapCell>> mapa(MAP_WIDTH, std::vector<MapCell>(MAP_HEIGHT));
    
    // Inicializar cada celda del mapa
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            mapa[x][y].x = x;
            mapa[x][y].y = y;
            
            // Configurar la forma rectangular de cada celda
            mapa[x][y].shape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            mapa[x][y].shape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
            mapa[x][y].shape.setFillColor(sf::Color::Blue); // Color azul para simular agua/mar
            mapa[x][y].shape.setOutlineThickness(0.5f);
            mapa[x][y].shape.setOutlineColor(sf::Color::Black);
        }
    }
    
    // AQUÍ: Cargar la textura de la imagen/gif del mar desde assets/images/
    // Para cargar una imagen estática:
    sf::Texture marTexture;
    if (!marTexture.loadFromFile("assets/images/mar.png")) {
        std::cerr << "Error al cargar la textura del mar desde assets/images/mar.png" << std::endl;
    }
    
    // Configurar la textura para que se repita (tiling)
    marTexture.setRepeated(true);
    
    // Crear sprite para el fondo del mapa
    sf::Sprite marFondo(marTexture);
    marFondo.setPosition(0, 0);
    
    // Variable para controlar el desplazamiento (efecto de movimiento)
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float velocidadMovimiento = 0.5f; // Velocidad del movimiento
    
    // Crear la ventana
    sf::RenderWindow window(sf::VideoMode(MAP_WIDTH * CELL_SIZE, MAP_HEIGHT * CELL_SIZE), "Mapa del Juego");
    window.setFramerateLimit(60);
    
    int frameCount = 0;
    int currentFrame = 0;
    
    // Loop principal
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // Ejemplo: Detectar clic del mouse para obtener coordenadas
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                int mapX = mousePos.x / CELL_SIZE;
                int mapY = mousePos.y / CELL_SIZE;
                
                if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT) {
                    std::cout << "Clic en coordenadas: (" << mapX << ", " << mapY << ")" << std::endl;
                    // Cambiar color de la celda seleccionada
                    mapa[mapX][mapY].shape.setFillColor(sf::Color::Red);
                }
            }
        }
        
        // Limpiar la ventana
        window.clear(sf::Color::White);
        
        // Actualizar el desplazamiento para el efecto de movimiento
        offsetX += velocidadMovimiento;
        offsetY += velocidadMovimiento * 0.5f; // Movimiento vertical más lento
        
        // Hacer que el offset sea cíclico (para efecto infinito)
        if (offsetX > marTexture.getSize().x) offsetX = 0;
        if (offsetY > marTexture.getSize().y) offsetY = 0;
        
        // Dibujar el fondo del mar con movimiento
        marFondo.setTextureRect(sf::IntRect(
            static_cast<int>(offsetX),
            static_cast<int>(offsetY),
            MAP_WIDTH * CELL_SIZE,
            MAP_HEIGHT * CELL_SIZE
        ));
        window.draw(marFondo);
        
        // Dibujar el grid del mapa encima (opcional)
        for (int x = 0; x < MAP_WIDTH; x++) {
            for (int y = 0; y < MAP_HEIGHT; y++) {
                // Solo dibujar el contorno para ver las coordenadas
                sf::RectangleShape grid(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                grid.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                grid.setFillColor(sf::Color::Transparent);
                grid.setOutlineThickness(0.3f);
                grid.setOutlineColor(sf::Color(0, 0, 0, 50)); // Negro semi-transparente
                window.draw(grid);
            }
        }
        
        // AQUÍ: Para animar, cambiar el frame cada X iteraciones
        // frameCount++;
        // if (frameCount >= 10) { // Cambiar frame cada 10 iteraciones
        //     currentFrame = (currentFrame + 1) % marFrames.size();
        //     frameCount = 0;
        // }
        
        // Mostrar la ventana
        window.display();
    }
    
    return 0;
}
