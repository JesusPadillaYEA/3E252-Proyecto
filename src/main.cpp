#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <optional>

using namespace sf;
using namespace std;

// ================== CLASE CELDA ==================
class Cell {
public:
    bool hasShip = false;
    bool wasShot = false;
};

// ================== CLASE TABLERO ==================
class Board {
private:
    vector<vector<Cell>> grid;

public:
    Board() {
        grid.resize(10, vector<Cell>(10));
        placeShips();
    }

    void placeShips() {
        srand(time(nullptr));
        for (int i = 0; i < 5; i++) {
            int x = rand() % 10;
            int y = rand() % 10;
            grid[x][y].hasShip = true;
        }
    }

    void shoot(int x, int y) {
        if (!grid[x][y].wasShot)
            grid[x][y].wasShot = true;
    }

    Cell getCell(int x, int y) {
        return grid[x][y];
    }
};

// ================== CLASE JUEGO ==================
class Game {
private:
    RenderWindow window;
    Board board;
    int cellSize = 40;

public:
    Game() : window(VideoMode(Vector2u(400, 400)), "Battleship C++") {}

    void run() {
        while (window.isOpen()) {
            handleEvents();
            draw();
        }
    }

    void handleEvents() {
        while (true) {
            optional<Event> event = window.pollEvent();
            if (!event.has_value())
                break;

            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouse = event->get<sf::Event::MouseButtonPressed>();
                int x = mouse.position.x / cellSize;
                int y = mouse.position.y / cellSize;

                if (x >= 0 && x < 10 && y >= 0 && y < 10)
                    board.shoot(x, y);
            }
        }
    }

    void draw() {
        window.clear();

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {

                RectangleShape cell(Vector2f(cellSize - 2, cellSize - 2));
                cell.setPosition(Vector2f(i * cellSize, j * cellSize));

                Cell c = board.getCell(i, j);

                if (c.wasShot) {
                    if (c.hasShip)
                        cell.setFillColor(Color::Red);
                    else
                        cell.setFillColor(Color::Blue);
                } else {
                    cell.setFillColor(Color::White);
                }

                window.draw(cell);
            }
        }

        window.display();
    }
};

// ================== MAIN ==================
int main() {
    Game game;
    game.run();
    return 0;
}
