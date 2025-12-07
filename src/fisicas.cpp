#include <SFML/Graphics.hpp>

int main()
{
    // Crear ventana
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(400, 600)),
        "Pelota rebotando"
    );

    window.setFramerateLimit(60);

    // Crear pelota
    sf::CircleShape ball(20.f);
    ball.setFillColor(sf::Color::Green);
    ball.setPosition({180.f, 0.f});

    float velocityY = 4.f; // velocidad vertical

    while (window.isOpen())
    {
        // Manejo de eventos (SFML 3)
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Movimiento vertical
        ball.move({0.f, velocityY});

        // Rebote superior
        if (ball.getPosition().y <= 0.f)
        {
            velocityY = -velocityY;
        }

        // Rebote inferior
        if (ball.getPosition().y + ball.getRadius() * 2 >= window.getSize().y)
        {
            velocityY = -velocityY;
        }

        // Dibujar
        window.clear(sf::Color::Black);
        window.draw(ball);
        window.display();
    }

    return 0;
}
