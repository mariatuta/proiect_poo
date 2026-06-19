//
// Created by Maria Tuta on 20.05.2026.
//

#include "Game.h"
#include "../include/Bomber.h"
#include "../include/Interceptor.h"
#include "Rocket.h"
#include "Stealth.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <tuple>

// Memorie statica pentru a putea dezvalui avioanele AI pe modul EASY
static std::vector<std::tuple<int, int, int, Direction> > g_aiPlanes;

std::vector<std::pair<int, int> > getPlaneCellsLocal(int type, int hRow, int hCol, Direction dir) {
    std::vector<std::pair<int, int> > rel;

    // NORD: Avionul este orientat in SUS. Corpul se intinde in JOS (+Linie).
    // Coordonatele sunt stocate ca {dRow, dCol}
    if (type == 0) rel = {{0, 0}, {1, 0}, {1, -1}, {1, 1}, {2, 0}, {3, 0}}; // Interceptor
    else if (type == 1)
        rel = {
            {0, 0}, {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {3, -1}, {3, 0}, {3, 1}
        }; // Bomber
    else if (type == 2) rel = {{0, 0}, {1, 0}, {2, 0}, {3, 0}}; // Rocket
    else if (type == 3) rel = {{0, 0}, {1, 0}, {2, -1}, {2, 1}}; // Stealth (Forma perfecta de sageata dictata de tine)

    std::vector<std::pair<int, int> > res;
    for (auto &p: rel) {
        int dRow = p.first, dCol = p.second;
        int rRow = dRow, rCol = dCol;

        // Rotatia Matematica Corectata
        if (dir == Direction::EAST) {
            // Orientat spre DREAPTA. Corpul se intinde spre STANGA (-Coloana)
            rRow = dCol;
            rCol = -dRow;
        } else if (dir == Direction::SOUTH) {
            // Orientat in JOS. Corpul se intinde in SUS (-Linie)
            rRow = -dRow;
            rCol = -dCol;
        } else if (dir == Direction::WEST) {
            // Orientat spre STANGA. Corpul se intinde spre DREAPTA (+Coloana)
            rRow = -dCol;
            rCol = dRow;
        }

        res.push_back({hRow + rRow, hCol + rCol});
    }
    return res;
}

// Constructorul clasei Game
Game::Game(std::string name)
    : projectName(std::move(name)), playerPlanesAlive(3), aiPlanesAlive(3),
      gameDifficulty(Difficulty::EASY), huntingMode(false) {
}

void Game::clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Generarea automată a flotei AI
void Game::setupAIBoard(int typeChoice) {
    g_aiPlanes.clear();
    int count = 0;
    while (count < 3) {
        int rx = rand() % 10;
        int ry = rand() % 10;
        auto rd = static_cast<Direction>(rand() % 4);

        std::unique_ptr<Aeroplane> newPlane = nullptr;

        if (typeChoice == 0) newPlane = std::make_unique<InterceptorPlane>(Point(rx, ry), rd);
        else if (typeChoice == 1) newPlane = std::make_unique<BomberPlane>(Point(rx, ry), rd);
        else if (typeChoice == 2) newPlane = std::make_unique<RocketPlane>(Point(rx, ry), rd);
        else newPlane = std::make_unique<StealthPlane>(Point(rx, ry), rd);

        if (aiBoard.addPlane(std::move(newPlane))) {
            g_aiPlanes.push_back({typeChoice, rx, ry, rd});
            count++;
        }
    }
}

Direction Game::rotateRight(Direction dir) {
    if (dir == Direction::NORTH) return Direction::EAST;
    if (dir == Direction::EAST) return Direction::SOUTH;
    if (dir == Direction::SOUTH) return Direction::WEST;
    return Direction::NORTH;
}

Direction Game::rotateLeft(Direction dir) {
    if (dir == Direction::NORTH) return Direction::WEST;
    if (dir == Direction::WEST) return Direction::SOUTH;
    if (dir == Direction::SOUTH) return Direction::EAST;
    return Direction::NORTH;
}

std::string Game::dirToString(Direction dir) {
    if (dir == Direction::NORTH) return "NORD";
    if (dir == Direction::SOUTH) return "SUD";
    if (dir == Direction::EAST) return "EST";
    return "VEST";
}

void Game::runGUI() {
    srand(static_cast<unsigned>(time(nullptr)));

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cout << ">>> ERROR: Cannot load arial.ttf from Windows/Fonts!\n";
    }

    currentState = GameState::DIFFICULTY;
    chosenFleetType = -1;
    currentDir = Direction::NORTH;
    planesPlaced = 0;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            visualAIGrid[i][j] = '~';
            visualPlayerGrid[i][j] = '~';
        }
    }

    sf::RenderWindow window(sf::VideoMode(1000, 600), projectName);
    window.setFramerateLimit(60);

    const int gridSize = 10;
    const float cellSize = 40.f;
    const float playerOffsetX = 50.f;
    const float playerOffsetY = 120.f;
    const float aiOffsetX = 550.f;
    const float aiOffsetY = 120.f;

    while (window.isOpen()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int hoverCol = (mousePos.x - playerOffsetX) / cellSize;
        int hoverRow = (mousePos.y - playerOffsetY) / cellSize;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && currentState == GameState::PLACEMENT) {
                if (event.key.code == sf::Keyboard::Right) currentDir = rotateRight(currentDir);
                else if (event.key.code == sf::Keyboard::Left) currentDir = rotateLeft(currentDir);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;

                if (currentState == GameState::DIFFICULTY) {
                    if (mouseY >= 250 && mouseY <= 330) {
                        if (mouseX >= 200 && mouseX <= 350) {
                            gameDifficulty = Difficulty::EASY;
                            currentState = GameState::SELECTION;
                        } else if (mouseX >= 425 && mouseX <= 575) {
                            gameDifficulty = Difficulty::MEDIUM;
                            currentState = GameState::SELECTION;
                        } else if (mouseX >= 650 && mouseX <= 800) {
                            gameDifficulty = Difficulty::ADVANCED;
                            currentState = GameState::SELECTION;
                        }
                    }
                } else if (currentState == GameState::SELECTION) {
                    if (mouseY >= 200 && mouseY <= 400) {
                        if (mouseX >= 50 && mouseX <= 250) chosenFleetType = 0;
                        else if (mouseX >= 275 && mouseX <= 475) chosenFleetType = 1;
                        else if (mouseX >= 500 && mouseX <= 700) chosenFleetType = 2;
                        else if (mouseX >= 725 && mouseX <= 925) chosenFleetType = 3;

                        if (chosenFleetType != -1) currentState = GameState::PLACEMENT;
                    }
                } else if (currentState == GameState::PLACEMENT) {
                    if (hoverRow >= 0 && hoverRow < gridSize && hoverCol >= 0 && hoverCol < gridSize) {
                        std::unique_ptr<Aeroplane> newPlane = nullptr;
                        // FIX: Trimitem la Backend Point(Linie, Coloana)
                        if (chosenFleetType == 0)
                            newPlane = std::make_unique<InterceptorPlane>(
                                Point(hoverRow, hoverCol), currentDir);
                        else if (chosenFleetType == 1)
                            newPlane = std::make_unique<BomberPlane>(
                                Point(hoverRow, hoverCol), currentDir);
                        else if (chosenFleetType == 2)
                            newPlane = std::make_unique<RocketPlane>(
                                Point(hoverRow, hoverCol), currentDir);
                        else newPlane = std::make_unique<StealthPlane>(Point(hoverRow, hoverCol), currentDir);

                        if (playerBoard.addPlane(std::move(newPlane))) {
                            auto cells = getPlaneCellsLocal(chosenFleetType, hoverRow, hoverCol, currentDir);
                            for (size_t k = 0; k < cells.size(); k++) {
                                int r = cells[k].first;
                                int c = cells[k].second;
                                if (k == 0) visualPlayerGrid[r][c] = 'H';
                                else visualPlayerGrid[r][c] = 'C';
                            }

                            planesPlaced++;
                            if (planesPlaced == 3) {
                                setupAIBoard(chosenFleetType);
                                if (Aeroplane::getTotalPlanesCreated() != 6) {
                                    throw FlotillaIncompleteException("Total planes in memory is invalid!");
                                }
                                currentState = GameState::BATTLE;
                            }
                        }
                    }
                } else if (currentState == GameState::BATTLE) {
                    int clickCol = (mouseX - aiOffsetX) / cellSize;
                    int clickRow = (mouseY - aiOffsetY) / cellSize;

                    if (clickRow >= 0 && clickRow < gridSize && clickCol >= 0 && clickCol < gridSize) {
                        if (visualAIGrid[clickRow][clickCol] == '~') {
                            char rez = aiBoard.attackCell(Point(clickRow, clickCol));

                            if (rez == 'B') rez = '!';
                            visualAIGrid[clickRow][clickCol] = rez;

                            if (rez == '!') {
                                aiPlanesAlive--;

                                if (gameDifficulty == Difficulty::EASY) {
                                    for (auto &p: g_aiPlanes) {
                                        int pType = std::get<0>(p);
                                        int pRow = std::get<1>(p);
                                        int pCol = std::get<2>(p);
                                        Direction pDir = std::get<3>(p);

                                        if (pRow == clickRow && pCol == clickCol) {
                                            auto cells = getPlaneCellsLocal(pType, pRow, pCol, pDir);
                                            for (auto &c: cells) {
                                                int cRow = c.first, cCol = c.second;
                                                if (cRow >= 0 && cRow < 10 && cCol >= 0 && cCol < 10) {
                                                    if (visualAIGrid[cRow][cCol] != '!') {
                                                        visualAIGrid[cRow][cCol] = '#';
                                                    }
                                                }
                                            }
                                            break;
                                        }
                                    }
                                }
                            }

                            if (aiPlanesAlive == 0) {
                                std::cout << "\n[VICTORY] You destroyed the AI Fleet!\n";
                                window.close();
                            } else {
                                Point aiMove;
                                int safetyCounter = 0;
                                do {
                                    if (gameDifficulty == Difficulty::ADVANCED && huntingMode && !targetsToTry.
                                        empty()) {
                                        aiMove = targetsToTry.back();
                                        targetsToTry.pop_back();
                                    } else {
                                        aiMove = Point(rand() % 10, rand() % 10);
                                    }
                                    safetyCounter++;
                                    if (safetyCounter > 50) {
                                        bool foundBackup = false;
                                        for (int r = 0; r < 10 && !foundBackup; ++r) {
                                            for (int c = 0; c < 10; ++c) {
                                                if (!playerBoard.isCellAlreadyAttacked(Point(r, c))) {
                                                    aiMove = Point(r, c);
                                                    foundBackup = true;
                                                    break;
                                                }
                                            }
                                        }
                                        break;
                                    }
                                } while (playerBoard.isCellAlreadyAttacked(aiMove));

                                char aiRes = playerBoard.attackCell(aiMove);
                                // Salvam raspunsul AI-ului direct la Linie/Coloana corecta
                                visualPlayerGrid[aiMove.getX()][aiMove.getY()] = (aiRes == 'B' || aiRes == '!')
                                        ? '!'
                                        : aiRes;

                                if (aiRes == 'X') {
                                    if (gameDifficulty == Difficulty::ADVANCED) {
                                        huntingMode = true;
                                        const int dx[] = {-1, 1, 0, 0};
                                        const int dy[] = {0, 0, -1, 1};
                                        for (int i = 0; i < 4; ++i) {
                                            int nx = aiMove.getX() + dx[i];
                                            int ny = aiMove.getY() + dy[i];
                                            if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10)
                                                targetsToTry.emplace_back(
                                                    nx, ny);
                                        }
                                    }
                                } else if (aiRes == '!' || aiRes == 'B') {
                                    playerPlanesAlive--;
                                    if (gameDifficulty == Difficulty::ADVANCED) {
                                        huntingMode = false;
                                        targetsToTry.clear();
                                    }
                                }

                                if (playerPlanesAlive == 0) {
                                    std::cout << "\n[DEFEAT] AI destroyed your fleet!\n";
                                    window.close();
                                }
                            }
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(240, 240, 245));

        sf::Text titleText;
        titleText.setFont(font);
        titleText.setCharacterSize(28);
        titleText.setFillColor(sf::Color(50, 50, 50));
        titleText.setStyle(sf::Text::Bold);

        if (currentState == GameState::DIFFICULTY) {
            titleText.setString("SELECT GAME DIFFICULTY");
            titleText.setPosition(320.f, 100.f);
            window.draw(titleText);

            std::string diffNames[] = {"EASY", "MEDIUM", "ADVANCED"};
            sf::Color diffColors[] = {sf::Color(100, 200, 100), sf::Color(255, 200, 50), sf::Color(200, 80, 80)};
            for (int i = 0; i < 3; i++) {
                sf::RectangleShape btn(sf::Vector2f(150.f, 80.f));
                btn.setPosition(200.f + i * 225.f, 250.f);
                btn.setFillColor(diffColors[i]);
                btn.setOutlineThickness(2.f);
                btn.setOutlineColor(sf::Color::Black);
                window.draw(btn);

                sf::Text btnText(diffNames[i], font, 20);
                btnText.setFillColor(sf::Color::Black);
                btnText.setStyle(sf::Text::Bold);
                btnText.setPosition(200.f + i * 225.f + 30.f, 275.f);
                window.draw(btnText);
            }
        } else if (currentState == GameState::SELECTION) {
            titleText.setString("CHOOSE YOUR FLEET TYPE");
            titleText.setPosition(320.f, 80.f);
            window.draw(titleText);

            std::string names[] = {"0. INTERCEPTOR", "1. BOMBER", "2. ROCKET", "3. STEALTH"};
            float blockX[] = {150.f, 375.f, 600.f, 825.f};

            for (int i = 0; i < 4; i++) {
                sf::Text nameText(names[i], font, 18);
                nameText.setFillColor(sf::Color::Black);
                nameText.setPosition(blockX[i] - 60.f, 400.f);
                window.draw(nameText);

                auto cells = getPlaneCellsLocal(i, 0, 0, Direction::NORTH);
                for (size_t k = 0; k < cells.size(); k++) {
                    sf::RectangleShape sq(sf::Vector2f(25.f, 25.f));
                    // Inversam Linia cu Coloana doar vizual pentru meniu ca sa fie orientate in sus pe ecran
                    sq.setPosition(blockX[i] + cells[k].second * 25.f, 200.f + cells[k].first * 25.f);
                    sq.setOutlineThickness(1.f);
                    sq.setOutlineColor(sf::Color::Black);

                    if (k == 0) sq.setFillColor(sf::Color::Red);
                    else sq.setFillColor(sf::Color(100, 150, 200));

                    window.draw(sq);
                }
            }
        } else if (currentState == GameState::PLACEMENT || currentState == GameState::BATTLE) {
            sf::Text infoText;
            infoText.setFont(font);
            infoText.setCharacterSize(20);
            infoText.setFillColor(sf::Color::Black);

            if (currentState == GameState::PLACEMENT) {
                infoText.setString(
                    "FLEET DEPLOYMENT: Place " + std::to_string(3 - planesPlaced) + " more planes.\nCurrent Direction: "
                    + dirToString(currentDir) + " (Use L/R Arrows to rotate)");
                infoText.setPosition(50.f, 30.f);
            } else {
                infoText.setString("YOUR FLEET (" + std::to_string(playerPlanesAlive) + " left)");
                infoText.setPosition(50.f, 80.f);

                sf::Text enemyText("ENEMY RADAR (" + std::to_string(aiPlanesAlive) + " left)", font, 20);
                enemyText.setFillColor(sf::Color::Black);
                enemyText.setPosition(550.f, 80.f);
                window.draw(enemyText);
            }
            window.draw(infoText);

            // 1. Desenam grila Jucatorului
            for (int r = 0; r < gridSize; ++r) {
                for (int c = 0; c < gridSize; ++c) {
                    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                    // X = coloana, Y = linia
                    cell.setPosition(playerOffsetX + c * cellSize, playerOffsetY + r * cellSize);
                    cell.setOutlineThickness(1.f);
                    cell.setOutlineColor(sf::Color::Black);

                    if (visualPlayerGrid[r][c] == '~') cell.setFillColor(sf::Color::White);
                    else if (visualPlayerGrid[r][c] == 'H') cell.setFillColor(sf::Color::Red);
                    else if (visualPlayerGrid[r][c] == 'C') cell.setFillColor(sf::Color(150, 180, 255));
                    else if (visualPlayerGrid[r][c] == 'O') cell.setFillColor(sf::Color(200, 200, 200));
                    else if (visualPlayerGrid[r][c] == 'X') cell.setFillColor(sf::Color(255, 165, 0));
                    else if (visualPlayerGrid[r][c] == '!') cell.setFillColor(sf::Color::Red);

                    window.draw(cell);
                }
            }

            // 2. Desenam umbra in faza de plasare
            if (currentState == GameState::PLACEMENT && hoverRow >= 0 && hoverRow < gridSize && hoverCol >= 0 &&
                hoverCol < gridSize) {
                auto shadowCells = getPlaneCellsLocal(chosenFleetType, hoverRow, hoverCol, currentDir);

                bool isValid = true;
                for (auto &p: shadowCells) {
                    if (p.first < 0 || p.first >= gridSize || p.second < 0 || p.second >= gridSize || visualPlayerGrid[p
                            .first][p.second] != '~') {
                        isValid = false;
                        break;
                    }
                }

                for (size_t k = 0; k < shadowCells.size(); k++) {
                    int sRow = shadowCells[k].first;
                    int sCol = shadowCells[k].second;

                    if (sRow >= 0 && sRow < gridSize && sCol >= 0 && sCol < gridSize) {
                        sf::RectangleShape shdw(sf::Vector2f(cellSize, cellSize));
                        shdw.setPosition(playerOffsetX + sCol * cellSize, playerOffsetY + sRow * cellSize);

                        if (isValid) {
                            if (k == 0) shdw.setFillColor(sf::Color(255, 0, 0, 150));
                            else shdw.setFillColor(sf::Color(100, 150, 255, 150));
                        } else {
                            shdw.setFillColor(sf::Color(255, 0, 0, 100));
                        }
                        window.draw(shdw);
                    }
                }
            }

            // 3. Desenam grila AI-ului
            if (currentState == GameState::BATTLE) {
                for (int r = 0; r < gridSize; ++r) {
                    for (int c = 0; c < gridSize; ++c) {
                        sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                        cell.setPosition(aiOffsetX + c * cellSize, aiOffsetY + r * cellSize);
                        cell.setOutlineThickness(1.f);
                        cell.setOutlineColor(sf::Color::Black);

                        if (visualAIGrid[r][c] == '~') cell.setFillColor(sf::Color::White);
                        else if (visualAIGrid[r][c] == 'O') cell.setFillColor(sf::Color(200, 200, 200));
                        else if (visualAIGrid[r][c] == 'X') cell.setFillColor(sf::Color(255, 165, 0));
                        else if (visualAIGrid[r][c] == '!') cell.setFillColor(sf::Color::Red);
                        else if (visualAIGrid[r][c] == '#') cell.setFillColor(sf::Color(139, 0, 0));

                        window.draw(cell);
                    }
                }
            }
        }
        window.display();
    }
}

// Configurarea flotei jucătorului uman (Consola)
void Game::setupPlayerBoard() {
    std::cout << "--- Configurare Flota Jucator (3 Avioane) ---\n";

    // 1. Validare Meniu Dificultate prin IF-ELSE clasic
    bool dificultateValida = false;
    do {
        std::cout << "Alege dificultatea (0: Easy, 1: Medium, 2: Advanced): ";
        int diffChoice;

        if (!(std::cin >> diffChoice) || diffChoice < 0 || diffChoice > 2) {
            std::cout << ">>> Optiune invalida pentru nivel! Introdu strict 0, 1 sau 2.\n\n";
            clearInput();
        } else {
            if (diffChoice == 0) gameDifficulty = Difficulty::EASY;
            else if (diffChoice == 2) gameDifficulty = Difficulty::ADVANCED;
            else gameDifficulty = Difficulty::MEDIUM;
            dificultateValida = true;
        }
    } while (!dificultateValida);

    // 2. Validare Meniu Tip Flotă
    int chosenType = 1;
    bool tipFlotaValid = false;
    do {
        std::cout << "Alege tipul flotei pentru acest meci (0: Interceptor, 1: Bombardier, 2: Racheta, 3:Stealth): ";
        if (!(std::cin >> chosenType) || chosenType < 0 || chosenType > 3) {
            std::cout << ">>> Optiune invalida pentru tipul flotei! Trebuie sa introduci 0, 1, 2 sau 3.\n\n";
            clearInput();
        } else {
            tipFlotaValid = true;
        }
    } while (!tipFlotaValid);

    // 3. Validare Plasare Avioane
    int count = 0;
    while (count < 3) {
        std::cout << "Avion " << count + 1 << " - Introdu Capul (x y) si Directia (0:N, 1:S, 2:E, 3:W): ";
        int x, y, d;

        if (!(std::cin >> x >> y >> d)) {
            std::cout << ">>> Date invalide! Te rugam sa introduci doar numere intregi.\n\n";
            clearInput();
            continue;
        }

        if (x < 0 || x >= 10 || y < 0 || y >= 10) {
            std::cout << ">>> Coordonate in afara hartii! Introdu numere intre 0 si 9 pentru x si y.\n\n";
            continue;
        }

        if (d < 0 || d > 3) {
            std::cout << ">>> Directie invalida! Introdu un numar intre 0 si 3.\n\n";
            continue;
        }

        auto dir = static_cast<Direction>(d);
        std::unique_ptr<Aeroplane> a = nullptr;

        if (chosenType == 0) a = std::make_unique<InterceptorPlane>(Point(x, y), dir);
        else if (chosenType == 1) a = std::make_unique<BomberPlane>(Point(x, y), dir);
        else if (chosenType == 2) a = std::make_unique<RocketPlane>(Point(x, y), dir);
        else a = std::make_unique<StealthPlane>(Point(x, y), dir);

        if (playerBoard.addPlane(std::move(a))) {
            std::cout << "Avion plasat cu succes!\n\n";
            count++;
        } else {
            std::cout << ">>> Pozitie invalida! Avionul iese din harta sau se suprapune cu altul.\n\n";
        }
    }
    setupAIBoard(chosenType);
}

// Bucla principală a bătăliei tactice (Consola)
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
    std::cout << "  * Stealth     (Tip 3) -> Forma aerodinamica de sageata(4 celule).\n";
    std::cout << "=========================================================================\n\n";

    if (Aeroplane::getTotalPlanesCreated() != 6) {
        throw FlotillaIncompleteException("Numarul total de avioane din memorie este invalid! Jocul nu poate incepe.");
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
            std::cout << "\nRANDUL TAU! Unde ataci? (x y): ";

            // Validare curată: afișăm mesajul și dăm 'continue' ca să reia bucla
            if (!(std::cin >> tx >> ty) || tx < 0 || tx >= 10 || ty < 0 || ty >= 10) {
                std::cout << ">>> Coordonate invalide! Trebuie sa introduci numere intre 0 si 9.\n";
                clearInput();
                continue; // Reia do-while de la capăt, nu mai aruncă nicio excepție!
            }

            // Verificăm dacă celula a fost deja atacată
            if (aiBoard.isCellAlreadyAttacked(Point(tx, ty))) {
                std::cout << ">>> Ai tras deja in aceasta casuta! Alege o celula neatacata.\n";
                continue;
            }

            // Dacă codul a trecut de validări, tragem!
            char res = aiBoard.attackCell(Point(tx, ty));

            if (res == 'X') {
                std::cout << ">>> Lovit in corp!\n";
            } else if (res == 'O') {
                std::cout << ">>> Miss. Mai incearca tura urmatoare.\n";
            } else if (res == 'B' || res == '!') {
                std::cout << ">>> CAP! Ai doborat un avion ";
                aiPlanesAlive--;

                // RAPORT RADAR DYNAMIC_CAST - DOAR PE MODUL EASY
                if (res == 'B') std::cout << "BOMBARDIER inamic.\n";
                else std::cout << "inamic.\n";
            }

            validTurn = true; // Tura s-a terminat cu succes, ieșim din do-while
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