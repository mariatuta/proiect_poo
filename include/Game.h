//
// Created by Maria Tuta on 20.05.2026.
//

#ifndef OOP_GAME_H
#define OOP_GAME_H

#include <stdexcept>
#include <string>
#include <vector>
#include "Board.h"
#include "Point.h"
#include <SFML/Graphics.hpp>

// 1. Clasa de bază
class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string &message) : std::runtime_error(message) {
    }
};

// 2. Pentru orice introducere greșită de coordonate (Configurare + Luptă)
class InvalidCoordinatesException : public GameException {
public:
    explicit InvalidCoordinatesException(const std::string &message) : GameException(message) {
    }
};

// 3. Pentru o direcție care nu este între 0 și 3 (Configurare)
class InvalidDirectionException : public GameException {
public:
    explicit InvalidDirectionException(const std::string &message) : GameException(message) {
    }
};

// 4. daca nu sunt exact 6 avioane în total
class FlotillaIncompleteException : public GameException {
public:
    explicit FlotillaIncompleteException(const std::string &message) : GameException(message) {
    }
};

enum class GameState {
    DIFFICULTY,
    SELECTION,
    PLACEMENT,
    BATTLE
};

enum class Difficulty { EASY, MEDIUM, ADVANCED };

class Game {
private:
    Board playerBoard;
    Board aiBoard;
    std::string projectName;
    int playerPlanesAlive;
    int aiPlanesAlive;

    Difficulty gameDifficulty;
    bool huntingMode;
    std::vector<Point> targetsToTry;

    // Funcții utilitare interne privatizate
    static void clearInput();

    void setupAIBoard(int typeChoice);
    // --- VARIABILE NOI PENTRU GRAFICĂ ---
    GameState currentState;
    int chosenFleetType;
    Direction currentDir;
    int planesPlaced;
    char visualAIGrid[10][10];
    char visualPlayerGrid[10][10];

    // Funcții utilitare interne privatizate
    // static void clearInput();
    // void setupAIBoard(int typeChoice);

    // Funcții utilitare noi pentru grafică
    Direction rotateRight(Direction dir);
    Direction rotateLeft(Direction dir);
    std::string dirToString(Direction dir);

public:
    explicit Game(std::string name);

    void setupPlayerBoard(); // Gestionează faza de configurare a ambilor jucători
    void startBattle();
    void runGUI();
};


#endif //OOP_GAME_H
