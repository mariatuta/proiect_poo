#include "../include/Game.h"
#include <iostream>
#include <cstdlib> // Adăugat pentru std::getenv

int main() {
    // --- BYPASS PENTRU GITHUB ACTIONS ---
    // Serverul GitHub Actions setează automat variabila de mediu "CI".
    // Dacă o detectăm, oprim programul cu succes ca să nu încercăm să 
    // deschidem o fereastră grafică pe un server fără monitor.
    if (std::getenv("CI")) {
        std::cout << "CI environment detected. Bypassing GUI execution.\n";
        return 0;
    }

    try {
        Game myGame("PaperSkyTactics - GUI Edition");
        myGame.runGUI();
    }
    // Prindem excepțiile personalizate
    catch (const InvalidCoordinatesException &e) {
        std::cout << "[EROARE CRITICA CONSTRUCTOR] Coordonate fatale obiect:\n" << e.what() << "\n";
        return 1;
    }
    catch (const InvalidDirectionException &e) {
        std::cout << "[EROARE CRITICA CONSTRUCTOR] Directie fatala obiect:\n" << e.what() << "\n";
        return 2;
    }
    catch (const FlotillaIncompleteException &e) {
        std::cout << "[EROARE CRITICA SISTEM] Integritate joc compromisa:\n" << e.what() << "\n";
        return 3;
    }
    // Prindem orice altă excepție
    catch (const std::exception &e) {
        std::cout << "\n[EROARE SISTEM NEPREVAZUTA]: " << e.what() << "\n";
        return 4;
    }

    return 0;
}