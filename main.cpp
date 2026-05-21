#include <ctime>
#include <cstdlib>
#include "Game.h"


int main() {
    srand(static_cast<unsigned>(time(nullptr))); // Pentru randomizare reală
    try {
        Game myGame("PaperSkyTactics");
        myGame.setupPlayerBoard();
        myGame.startBattle();
    } catch (const GameException &e) {
        std::cout << "[EROARE CRITICA PRINSA IN MAIN]:\n";
        std::cout << e.what() << "\n";
        return 1;
    } catch (const std::exception &e) {
        std::cout << "\n[EROARE SISTEM]: " << e.what() << "\n";
        return 2;
    }
    return 0;
}
