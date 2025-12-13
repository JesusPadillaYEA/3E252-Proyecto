#include "Game.hpp"

int main() {
    // Semilla aleatoria
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    Game game;
    game.run();
    
    return 0;
}