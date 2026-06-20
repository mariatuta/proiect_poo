//
// Created by Maria Tuta on 20.05.2026.
//

#include "Game.h"
#include "../include/Factory.h"
#include "Templates.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <tuple>

// Memorie statica pentru a putea dezvalui avioanele AI pe modul EASY
static std::vector<std::tuple<int, int, int, Direction> > g_aiPlanes;

std::vector<std::pair<int, int> > getPlaneCellsLocal(int type, int hRow, int hCol, Direction dir) {
    std::vector<std::pair<int, int> > rel;

    if (type == 0) rel = {{0, 0}, {1, 0}, {1, -1}, {1, 1}, {2, 0}, {3, 0}}; // Interceptor
    else if (type == 1)
        rel = {{0, 0}, {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {3, -1}, {3, 0}, {3, 1}}; // Bomber
    else if (type == 2) rel = {{0, 0}, {1, 0}, {2, 0}, {3, 0}}; // Rocket
    else if (type == 3) rel = {{0, 0}, {1, 0}, {2, -1}, {2, 1}}; // Stealth

    std::vector<std::pair<int, int> > res;
    for (auto &p: rel) {
        int dRow = p.first, dCol = p.second;
        int rRow = dRow, rCol = dCol;

        if (dir == Direction::EAST) {
            rRow = dCol;
            rCol = -dRow;
        } else if (dir == Direction::SOUTH) {
            rRow = -dRow;
            rCol = -dCol;
        } else if (dir == Direction::WEST) {
            rRow = -dCol;
            rCol = dRow;
        }

        res.push_back({hRow + rRow, hCol + rCol});
    }
    return res;
}

// Constructorul: Initializam default AI-ul cu Strategia Random
Game::Game(std::string name)
    : projectName(std::move(name)), playerPlanesAlive(3), aiPlanesAlive(3),
      gameDifficulty(Difficulty::EASY), aiStrategy(std::make_unique<RandomAttackStrategy>()) {
}

void Game::clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Game::setupAIBoard(int typeChoice) {
    g_aiPlanes.clear();
    int count = 0;

    // Vectorii pentru extragere aleatorie folosind functia sablon
    std::vector<int> coordPool = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<Direction> dirPool = {Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST};

    while (count < 3) {
        // template function - instantierea 1 (int) si Instantierea 2 (Direction)
        int rx = getRandomElement<int>(coordPool);
        int ry = getRandomElement<int>(coordPool);
        Direction rd = getRandomElement<Direction>(dirPool);

        std::unique_ptr<Aeroplane> newPlane = PlaneFactory::createPlane(typeChoice, Point(rx, ry), rd);

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

    visualAIGrid.fill('~');
    visualPlayerGrid.fill('~');
    aiDiscoveredCells.fill(false);

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

            if (event.type == sf::Event::KeyPressed) {
                if (currentState == GameState::PLACEMENT) {
                    if (event.key.code == sf::Keyboard::Right) currentDir = rotateRight(currentDir);
                    else if (event.key.code == sf::Keyboard::Left) currentDir = rotateLeft(currentDir);
                } else if (currentState == GameState::GAME_OVER) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        window.close();
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;

                if (currentState == GameState::DIFFICULTY) {
                    if (mouseY >= 250 && mouseY <= 330) {
                        if (mouseX >= 200 && mouseX <= 350) {
                            gameDifficulty = Difficulty::EASY;
                            aiStrategy = std::make_unique<RandomAttackStrategy>(); // SETARE STRATEGIE RANDOM
                            currentState = GameState::SELECTION;
                        } else if (mouseX >= 425 && mouseX <= 575) {
                            gameDifficulty = Difficulty::MEDIUM;
                            aiStrategy = std::make_unique<RandomAttackStrategy>(); // SETARE STRATEGIE RANDOM
                            currentState = GameState::SELECTION;
                        } else if (mouseX >= 650 && mouseX <= 800) {
                            gameDifficulty = Difficulty::ADVANCED;
                            aiStrategy = std::make_unique<HuntAttackStrategy>(); // SETARE STRATEGIE HUNTING
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
                        std::unique_ptr<Aeroplane> newPlane = PlaneFactory::createPlane(
                            chosenFleetType, Point(hoverRow, hoverCol), currentDir);

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
                                playerWon = true;
                                currentState = GameState::GAME_OVER;
                            } else {
                                // STRATEGY
                                Point aiMove = aiStrategy->getNextMove(playerBoard);
                                char aiRes = playerBoard.attackCell(aiMove);

                                visualPlayerGrid[aiMove.getX()][aiMove.getY()] = (aiRes == 'B' || aiRes == '!')
                                        ? '!'
                                        : aiRes;

                                aiStrategy->updateState(aiMove, aiRes);

                                if (aiRes == '!' || aiRes == 'B') playerPlanesAlive--;

                                if (playerPlanesAlive == 0) {
                                    std::cout << "\n[DEFEAT] AI destroyed your fleet!\n";
                                    playerWon = false;
                                    currentState = GameState::GAME_OVER;
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

            for (int r = 0; r < gridSize; ++r) {
                for (int c = 0; c < gridSize; ++c) {
                    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
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
        if (currentState == GameState::GAME_OVER) {
            // Desenăm un fundal semi-transparent peste joc ca să îl întunecăm un pic
            sf::RectangleShape overlay(sf::Vector2f(1000.f, 600.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);

            sf::Text endText;
            endText.setFont(font);
            endText.setCharacterSize(45);
            endText.setStyle(sf::Text::Bold);

            if (playerWon) {
                endText.setString("VICTORY!\nYou destroyed the AI Fleet!\n\nPress ENTER to exit.");
                endText.setFillColor(sf::Color(100, 255, 100)); // Verde
            } else {
                endText.setString("DEFEAT!\nAI destroyed your fleet!\n\nPress ENTER to exit.");
                endText.setFillColor(sf::Color(255, 100, 100)); // Roșu
            }

            // Centram textul pe mijlocul ecranului
            sf::FloatRect textRect = endText.getLocalBounds();
            endText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            endText.setPosition(sf::Vector2f(500.f, 300.f));

            window.draw(endText);
        }
        window.display();
    }
}

void Game::setupPlayerBoard() {
    std::cout << "--- Configurare Flota Jucator (3 Avioane) ---\n";

    bool dificultateValida = false;
    do {
        std::cout << "Alege dificultatea (0: Easy, 1: Medium, 2: Advanced): ";
        int diffChoice;

        if (!(std::cin >> diffChoice) || diffChoice < 0 || diffChoice > 2) {
            std::cout << ">>> Optiune invalida pentru nivel! Introdu strict 0, 1 sau 2.\n\n";
            clearInput();
        } else {
            if (diffChoice == 0) {
                gameDifficulty = Difficulty::EASY;
                aiStrategy = std::make_unique<RandomAttackStrategy>(); // SETARE STRATEGIE
            } else if (diffChoice == 2) {
                gameDifficulty = Difficulty::ADVANCED;
                aiStrategy = std::make_unique<HuntAttackStrategy>(); // SETARE STRATEGIE
            } else {
                gameDifficulty = Difficulty::MEDIUM;
                aiStrategy = std::make_unique<RandomAttackStrategy>(); // SETARE STRATEGIE
            }
            dificultateValida = true;
        }
    } while (!dificultateValida);

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
            std::cout << ">>> Coordonate in afara hartii!\n\n";
            continue;
        }

        if (d < 0 || d > 3) {
            std::cout << ">>> Directie invalida!\n\n";
            continue;
        }

        auto dir = static_cast<Direction>(d);
        std::unique_ptr<Aeroplane> a = PlaneFactory::createPlane(chosenType, Point(x, y), dir);

        if (playerBoard.addPlane(std::move(a))) {
            std::cout << "Avion plasat cu succes!\n\n";
            count++;
        } else {
            std::cout << ">>> Pozitie invalida!\n\n";
        }
    }
    setupAIBoard(chosenType);
}

void Game::startBattle() {
    std::cout << "\n=== START " << projectName << " ===\n";
    std::cout << "=========================================================================\n";
    std::cout << "[SIMBOLURI HARTA]:\n";
    std::cout << "  ~ : Celula neatacata             O : MISS\n";
    std::cout << "  X : Corp avion lovit             ! : CAP AVION DOBORAT\n";
    std::cout << "=========================================================================\n\n";

    if (Aeroplane::getTotalPlanesCreated() != 6) {
        throw FlotillaIncompleteException("Numarul total de avioane din memorie este invalid!");
    }

    while (playerPlanesAlive > 0 && aiPlanesAlive > 0) {
        std::cout << "\nTABLA TA (Avioanele tale):\n";
        playerBoard.printBoard(false);
        std::cout << "Avioane proprii ramase: " << playerPlanesAlive << "\n";

        std::cout << "\nTABLA INAMICULUI (Atacurile tale):\n";
        aiBoard.printBoard(true);
        std::cout << "Avioane inamice detectate: " << aiPlanesAlive << "\n";

        int tx, ty;
        bool validTurn = false;
        do {
            std::cout << "\nRANDUL TAU! Unde ataci? (x y): ";

            if (!(std::cin >> tx >> ty) || tx < 0 || tx >= 10 || ty < 0 || ty >= 10) {
                std::cout << ">>> Coordonate invalide!\n";
                clearInput();
                continue;
            }

            if (aiBoard.isCellAlreadyAttacked(Point(tx, ty))) {
                std::cout << ">>> Ai tras deja in aceasta casuta!\n";
                continue;
            }

            char res = aiBoard.attackCell(Point(tx, ty));

            if (res == 'X') {
                std::cout << ">>> Lovit in corp!\n";
            } else if (res == 'O') {
                std::cout << ">>> Miss.\n";
            } else if (res == 'B' || res == '!') {
                std::cout << ">>> CAP! Ai doborat un avion inamic.\n";
                aiPlanesAlive--;
            }

            validTurn = true;
        } while (!validTurn);

        if (aiPlanesAlive == 0) {
            std::cout << "\nVICTORIE! Ai distrus toate cele 3 avioane inamice!\n";
            break;
        }

        // --- SHABLONUL STRATEGY IN ACTIUNE ---
        std::cout << "\nRandul inamicului... ";

        Point aiMove = aiStrategy->getNextMove(playerBoard);
        char aiRes = playerBoard.attackCell(aiMove);
        aiStrategy->updateState(aiMove, aiRes); // AI-ul isi aminteste lovitura

        std::cout << "A tras la " << aiMove << ": ";

        if (aiRes == 'X') {
            std::cout << "Ti-a lovit un avion in corp!\n";
        } else if (aiRes == '!' || aiRes == 'B') {
            std::cout << "CRITICAL! Ti-a doborat un avion complet!\n";
            playerPlanesAlive--;
        } else {
            std::cout << "A ratat.\n";
        }

        if (playerPlanesAlive == 0) {
            std::cout << "\nAi pierdut! Toate avioanele tale au fost doborate.\n";
            return;
        }
        // -------------------------------------
    }
}