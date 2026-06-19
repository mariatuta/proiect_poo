//#include <ctime>
//#include <cstdlib>
#include "../include/Game.h"
//#include <iostream>
//#include <SFML/Graphics.hpp>

int main() {
    try {
        Game myGame("PaperSkyTactics - GUI Edition");
        myGame.runGUI();
    }
    // 1. Prindem eroarea de coordonate trimise greșit în constructorul Aeroplane
    catch (const InvalidCoordinatesException &e) {
        std::cout << "[EROARE CRITICA CONSTRUCTOR] Coordonate fatale obiect:\n";
        std::cout << e.what() << "\n";
        return 1;
    }
    // 2. Prindem eroarea de direcție invalidă trimisă în constructorul Aeroplane
    catch (const InvalidDirectionException &e) {
        std::cout << "[EROARE CRITICA CONSTRUCTOR] Directie fatala obiect:\n";
        std::cout << e.what() << "\n";
        return 2;
    }
    // 3. Prindem eroarea de integritate a flotei (bariera de securitate statică)
    catch (const FlotillaIncompleteException &e) {
        std::cout << "[EROARE CRITICA SISTEM] Integritate joc compromisa:\n";
        std::cout << e.what() << "\n";
        return 3;
    }
    // 4. Prindem orice altă excepție standard neprevăzută din C++
    catch (const std::exception &e) {
        std::cout << "\n[EROARE SISTEM NEPREVAZUTA]: " << e.what() << "\n";
        return 4;
    }
    return 0;

}
