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
        // return -1; // Descomenta si quieres que el programa se cierre si no encuentra la imagen
    }
    
    // Para animar un gif, necesitarías cargar múltiples frames:
    // Descomenta y ajusta según tus archivos PNG en assets/images/
    // const int NUM_FRAMES = 10; // Cambia esto según cuántos frames tengas
    // std::vector<sf::Texture> marFrames;
    // for (int i = 0; i < NUM_FRAMES; i++) {
    //     sf::Texture frame;
    //     std::string framePath = "assets/images/mar_frame_" + std::to_string(i) + ".png";
    //     if (!frame.loadFromFile(framePath)) {
    //         std::cerr << "Error al cargar frame: " << framePath << std::endl;
    //     }
    //     marFrames.push_back(frame);
    // }
    
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
        
        // Dibujar el mapa
        for (int x = 0; x < MAP_WIDTH; x++) {
            for (int y = 0; y < MAP_HEIGHT; y++) {
                window.draw(mapa[x][y].shape);
                
                // AQUÍ: Aplicar la textura del mar a cada celda
                // sf::Sprite marSprite(marTexture);
                // marSprite.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                // marSprite.setScale(CELL_SIZE / marTexture.getSize().x, CELL_SIZE / marTexture.getSize().y);
                // window.draw(marSprite);
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
