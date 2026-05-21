//
// Created by Maria Tuta on 20.05.2026.
//

#include "Game.h"
#include "Bomber.h"
#include "Interceptor.h"
#include "Rocket.h"
#include <iostream>
#include <limits>
//#include <cstdlib>

int Aeroplane::totalPlanesCreated = 0;

// Constructorul clasei Game
Game::Game(std::string name)
    : projectName(std::move(name)), playerPlanesAlive(3), aiPlanesAlive(3),
      gameDifficulty(Difficulty::EASY), huntingMode(false) {
}

void Game::clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Generarea automată a flotei AI folosind polimorfismul
void Game::setupAIBoard(int typeChoice) {
    int count = 0;
    while (count < 3) {
        int rx = rand() % 10;
        int ry = rand() % 10;
        auto rd = static_cast<Direction>(rand() % 4);

        std::unique_ptr<Aeroplane> newPlane = nullptr;

        // Instanțiere polimorfică în funcție de tipul ales pentru meci
        if (typeChoice == 0) {
            newPlane = std::make_unique<InterceptorPlane>(Point(rx, ry), rd);
        } else if (typeChoice == 1) {
            newPlane = std::make_unique<BomberPlane>(Point(rx, ry), rd);
        } else {
            newPlane = std::make_unique<RocketPlane>(Point(rx, ry), rd);
        }

        if (aiBoard.addPlane(std::move(newPlane))) {
            count++;
        }
    }
    std::cout << "Inamicul si-a pregatit flota simetrica în mod automat.\n";
}

// Configurarea flotei jucătorului uman
void Game::setupPlayerBoard() {
    std::cout << "--- Configurare Flota Jucator (3 Avioane) ---\n";
    //std::cout << "Alege dificultatea (0: Incepator, 1: Avansat): ";

    bool dificultateValida = false;
    do {
        try {
            std::cout << "Alege dificultatea (0: Easy, 1: Medium, 2: Advanced): ";
            int diffChoice;

            if (!(std::cin >> diffChoice) || diffChoice < 0 || diffChoice > 2) {
                clearInput();
                throw InvalidCoordinatesException(">>> Optiune invalida pentru nivel! Introdu strict 0, 1 sau 2.");
            }

            if (diffChoice == 0) gameDifficulty = Difficulty::EASY;
            else if (diffChoice == 2) gameDifficulty = Difficulty::ADVANCED;
            else gameDifficulty = Difficulty::MEDIUM;

            dificultateValida = true;
        } catch (const GameException &e) {
            std::cout << e.what() << " Incearca din nou.\n\n";
        }
    } while (!dificultateValida);

    int count = 0;
    int chosenType = 1; // Implicit Bombardier

    bool tipFlotaValid = false;
    do {
        try {
            std::cout << "Alege tipul flotei pentru acest meci (0: Interceptor, 1: Bombardier, 2: Racheta): ";
            if (!(std::cin >> chosenType) || chosenType < 0 || chosenType > 2) {
                clearInput();
                throw InvalidCoordinatesException(
                    ">>> Optiune invalida pentru tipul flotei! Trebuie sa introduci 0, 1 sau 2.");
            }
            tipFlotaValid = true; // Dacă e corect, ieșim din buclă
        } catch (const GameException &e) {
            std::cout << e.what() << " Incearca din nou.\n\n";
        }
    } while (!tipFlotaValid);

    while (count < 3) {
        try {
            std::cout << "Avion " << count + 1 << " - Introdu Capul (x y) si Directia (0:N, 1:S, 2:E, 3:W): ";
            int x, y, d;

            // Am unit verificarea de caractere și de margini ale hărții pentru x și y
            if (!(std::cin >> x >> y >> d) || x < 0 || x >= 10 || y < 0 || y >= 10) {
                clearInput();
                throw InvalidCoordinatesException(
                    ">>> Coordonate invalide! Introdu numere intregi intre 0 si 9 pentru x si y.");
            }

            // Verificăm separat direcția ca să dăm un mesaj precis
            if (d < 0 || d > 3) {
                throw InvalidDirectionException(">>> Directie invalida! Introdu un numar intre 0 si 3.");
            }

            auto dir = static_cast<Direction>(d);
            std::unique_ptr<Aeroplane> a = nullptr;

            if (chosenType == 0) a = std::make_unique<InterceptorPlane>(Point(x, y), dir);
            else if (chosenType == 1) a = std::make_unique<BomberPlane>(Point(x, y), dir);
            else a = std::make_unique<RocketPlane>(Point(x, y), dir);

            // Verificăm dacă se poate adăuga fizic pe tablă
            if (playerBoard.addPlane(std::move(a))) {
                std::cout << "Avion plasat cu succes!\n\n";
                count++;
            } else {
                throw CellUnavailableException(
                    ">>> Pozitie invalida! Avionul iese din harta sau se suprapune cu altul.");
            }
        } catch (const GameException &e) {
            std::cout << e.what() << " Reintrodu datele pentru acest avion.\n\n";
        }
    }
    setupAIBoard(chosenType);
}

// Bucla principală a bătăliei tactice
void Game::startBattle() {
    std::cout << "\n=== START " << projectName << " ===\n";
    std::cout << "=========================================================================\n";
    std::cout << "                        LEGENDA SI REGULI DE JOC                         \n";
    std::cout << "=========================================================================\n";
    std::cout << "[NIVELURI DE DIFICULTATE]:\n";
    std::cout << "  0. EASY     - Cand dobori un avion inamic, toate celulele avionului doborat sunt\n";
    std::cout << "                hasurate doborat sunt hasurate. Inamicul trage random.\n";
    std::cout << "  1. MEDIUM   - Mod Clasic. Cand dobori un cap, vezi doar simbolic '!',\n";
    std::cout << "                dar corpul ramane ascuns.Inamicul trage random.\n";
    std::cout << "  2. ADVANCED - Mod Expert. Inamicul devine inteligent si intra in 'Hunting\n";
    std::cout << "                Mode' (vaneaza celulele vecine daca te-a lovit).\n\n";
    std::cout << "[SIMBOLURI HARTA]:\n";
    std::cout << "  ~ : Celula neatacata             O : MISS\n";
    std::cout << "  X : Corp avion lovit             ! : CAP AVION DOBORAT\n";
    std::cout << "  # : Corp descoperit automat (Disponibil doar pe modul EASY)\n\n";
    std::cout << "[TIPURI DE AVIOANE DISPONIBILE]:\n";
    std::cout << "  * Interceptor (Tip 0) -> Clasic, are forma clasica de cruce, 6 celule in total.\n";
    std::cout << "  * Bombardier  (Tip 1) -> Masiv, 5 celule pt aripi, 3 pentru coada.\n";
    std::cout << "  * Racheta     (Tip 2) -> Linie dreapta de 4 celule.\n";
    std::cout << "=========================================================================\n\n";

    if (Aeroplane::getTotalPlanesCreated() != 6) {
        throw GameException("ERR-SECURE: Numarul total de avioane din memorie este invalid! Jocul nu poate incepe.");
    }

    while (playerPlanesAlive > 0 && aiPlanesAlive > 0) {
        // Afișăm tablele apelând metodele high-level din Board
        std::cout << "\nTABLA TA (Avioanele tale):\n";
        playerBoard.printBoard(false);
        std::cout << "Avioane proprii ramase: " << playerPlanesAlive << "\n";

        std::cout << "\nTABLA INAMICULUI (Atacurile tale):\n";
        aiBoard.printBoard(true);
        std::cout << "Avioane inamice detectate: " << aiPlanesAlive << "\n";

        // --- RÂNDUL JUCĂTORULUI ---
        int tx, ty;
        bool validTurn = false;
        do {
            try {
                std::cout << "\nRANDUL TAU! Unde ataci? (x y): ";

                // Am unit verificarea pentru litere și pentru margini în afara hărții (0-9)
                if (!(std::cin >> tx >> ty) || tx < 0 || tx >= 10 || ty < 0 || ty >= 10) {
                    clearInput();
                    throw InvalidCoordinatesException(
                        ">>> Coordonate invalide! Trebuie sa introduci numere intre 0 si 9.");
                }

                // Verificăm dacă celula a fost deja atacată
                if (aiBoard.isCellAlreadyAttacked(Point(tx, ty))) {
                    throw CellUnavailableException(">>> Ai tras deja in aceasta casuta! Alege o celula neatacata.");
                }

                // Dacă codul a trecut de excepții, tragem!
                char res = aiBoard.attackCell(Point(tx, ty));

                if (res == 'X') {
                    std::cout << ">>> Lovit in corp!\n";
                } else if (res == 'O') {
                    std::cout << ">>> Miss. Mai incearca tura urmatoare.\n";
                } else if (res == 'B' || res == 'R' || res == 'I' || res == '!') {
                    std::cout << ">>> CAP! Ai doborat un avion ";
                    aiPlanesAlive--;

                    // RAPORT RADAR DYNAMIC_CAST - DOAR PE MODUL EASY
                    if (res == 'B') std::cout << "BOMBARDIER inamic.\n";
                    else if (res == 'R') std::cout << "RACHETA inamic.\n";
                    else if (res == 'I') std::cout << "INTERCEPTOR inamic.\n";
                }

                validTurn = true; // Tura s-a terminat cu succes, ieșim din do-while
            } catch (const GameException &e) {
                std::cout << e.what() << " Incearca din nou.\n";
                // validTurn rămâne false, deci bucla reia introducerea datelor
            }
        } while (!validTurn);

        if (aiPlanesAlive == 0) {
            std::cout << "\nVICTORIE! Ai distrus toate cele 3 avioane inamice! Felicitari!\n";
            break;
        }

        // --- RÂNDUL AI-ULUI ---
        std::cout << "\nRandul inamicului...";
        Point aiMove;
        int safetyCounter = 0; // Contor de siguranță anti-buclă infinită

        do {
            if (gameDifficulty == Difficulty::ADVANCED && huntingMode && !targetsToTry.empty()) {
                aiMove = targetsToTry.back();
                targetsToTry.pop_back();
            } else {
                aiMove = Point(rand() % 10, rand() % 10);
            }

            safetyCounter++;

            // Dacă a încercat de prea multe ori la rând și nu a nimerit o celulă liberă
            if (safetyCounter > 50) {
                // Căutăm prima celulă neatacată de la (0,0) la (9,9)
                bool foundBackup = false;
                for (int i = 0; i < 10 && !foundBackup; ++i) {
                    for (int j = 0; j < 10; ++j) {
                        if (!playerBoard.isCellAlreadyAttacked(Point(i, j))) {
                            aiMove = Point(i, j);
                            foundBackup = true;
                            break;
                        }
                    }
                }
                break;
            }

        } while (playerBoard.isCellAlreadyAttacked(aiMove));
        char aiRes = playerBoard.attackCell(aiMove);
        std::cout << "A tras la coordonatele " << aiMove << ": ";
        if (aiRes == 'X') {
            std::cout << "Ti-a lovit un avion in corp!\n";
            if (gameDifficulty == Difficulty::ADVANCED) {
                huntingMode = true;
                int x = aiMove.getX();
                int y = aiMove.getY();

                const int dx[] = {-1, 1, 0, 0};
                const int dy[] = {0, 0, -1, 1};

                for (int i = 0; i < 4; ++i) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];

                    if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10) {
                        targetsToTry.emplace_back(nx, ny);
                    }
                }
            }
        } else if (aiRes == '!') {
            std::cout << "CRITICAL! Ti-a doborat un avion complet!\n";
            playerPlanesAlive--;
            if (gameDifficulty == Difficulty::ADVANCED) {
                huntingMode = false;
                targetsToTry.clear();
            }
        } else {
            std::cout << "A ratat .\n";
        }

        if (playerPlanesAlive == 0) {
            std::cout << "\nAi pierdut! Toate avioanele tale au fost doborate de inamic.\n";
            return;
        }
    }
}
