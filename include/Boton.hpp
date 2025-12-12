#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Boton {
public:
    // CORRECCIÓN SFML 3.0:
    // Usamos la "lista de inicialización" (: label(font)) para construir el texto
    // con la fuente antes de que entre al cuerpo de la función.
    Boton(const sf::Font& font, std::string texto, sf::Color colorBase) 
        : label(font) 
    {
        // Configurar forma
        shape.setSize({100.f, 40.f});
        shape.setFillColor(colorBase);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::White);
        this->colorOriginal = colorBase;

        // Configurar texto (ya tiene la fuente, solo falta el string)
        label.setString(texto);
        label.setCharacterSize(18);
        label.setFillColor(sf::Color::White);
    }

    void setPosition(sf::Vector2f pos) {
        shape.setPosition(pos);
        // Centrar texto dentro del botón
        label.setPosition({pos.x + 15.f, pos.y + 8.f}); 
    }

    void setColor(sf::Color color) {
        shape.setFillColor(color);
    }

    void resetColor() {
        shape.setFillColor(colorOriginal);
    }

    bool esClickeado(sf::Vector2f mousePos) {
        return shape.getGlobalBounds().contains(mousePos);
    }
    
    sf::Vector2f getPosition() const {
        return shape.getPosition();
    }
    
    // SFML 3: FloatRect ahora es simplemente Rect<float>
    sf::Rect<float> getBounds() const {
        return shape.getGlobalBounds();
    }

    void dibujar(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(label);
    }

private:
    sf::RectangleShape shape;
    sf::Text label;
    sf::Color colorOriginal;
};