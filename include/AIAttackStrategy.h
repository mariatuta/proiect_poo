#ifndef OOP_AIATTACKSTRATEGY_H
#define OOP_AIATTACKSTRATEGY_H

#include "Point.h"
#include "Board.h"
#include <vector>
#include <cstdlib>

// 1. Interfata Strategiei (Regula generala pentru orice "creier" de AI)
class AIAttackStrategy {
public:
    virtual ~AIAttackStrategy() = default;

    // Metoda pentru a gasi urmatoarea miscare
    virtual Point getNextMove(const Board& playerBoard) = 0;

    // Metoda prin care AI-ul afla daca a lovit sau a ratat (ca sa invete)
    virtual void updateState(Point lastMove, char result) = 0;
};

// 2. Strategia "Oarba" (Trage random - folosita pt EASY si MEDIUM)
class RandomAttackStrategy : public AIAttackStrategy {
public:
    Point getNextMove(const Board& playerBoard) override {
        Point aiMove;
        int safetyCounter = 0;
        do {
            aiMove = Point(rand() % 10, rand() % 10);
            safetyCounter++;
            if (safetyCounter > 50) {
                // Fallback daca a incercat de prea multe ori random
                for (int i = 0; i < 10; ++i) {
                    for (int j = 0; j < 10; ++j) {
                        if (!playerBoard.isCellAlreadyAttacked(Point(i, j))) return Point(i, j);
                    }
                }
            }
        } while (playerBoard.isCellAlreadyAttacked(aiMove));
        return aiMove;
    }

    // FIX AICI: Am eliminat numele parametrilor "lastMove" si "result" pentru a scapa de eroarea -Werror=unused-parameter
    void updateState(Point, char) override {
        // Nu tine minte nimic, e strategie random
    }
};

// 3. Strategia Inteligenta (Hunting Mode - folosita pt ADVANCED)
class HuntAttackStrategy : public AIAttackStrategy {
private:
    std::vector<Point> targetsToTry;
    bool huntingMode = false;
public:
    Point getNextMove(const Board& playerBoard) override {
        Point aiMove;
        int safetyCounter = 0;
        do {
            if (huntingMode && !targetsToTry.empty()) {
                aiMove = targetsToTry.back();
                targetsToTry.pop_back();
            } else {
                aiMove = Point(rand() % 10, rand() % 10);
            }
            safetyCounter++;
            if (safetyCounter > 50) {
                for (int i = 0; i < 10; ++i) {
                    for (int j = 0; j < 10; ++j) {
                        if (!playerBoard.isCellAlreadyAttacked(Point(i, j))) return Point(i, j);
                    }
                }
            }
        } while (playerBoard.isCellAlreadyAttacked(aiMove));
        return aiMove;
    }

    void updateState(Point lastMove, char result) override {
        if (result == 'X') {
            huntingMode = true; // Intra in modul de vanatoare
            const int dx[] = {-1, 1, 0, 0};
            const int dy[] = {0, 0, -1, 1};
            for (int i = 0; i < 4; ++i) {
                int nx = lastMove.getX() + dx[i];
                int ny = lastMove.getY() + dy[i];
                if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10) {
                    targetsToTry.emplace_back(nx, ny);
                }
            }
        } else if (result == '!' || result == 'B') {
            huntingMode = false; // A doborat avionul, se opreste din vanat
            targetsToTry.clear();
        }
    }
};

#endif // OOP_AIATTACKSTRATEGY_H