#include "Board.h"
#include "Bomber.h"       // Adaugă asta!
#include "Interceptor.h"
#include "Rocket.h"
#include <iostream>
#include <algorithm>
#include <utility> // Obligatoriu pentru std::swap pe unique_ptr

Board::Board() {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            grid[i][j] = '~';
        }
    }
}

void swap(Board &first, Board &second) noexcept {
    using std::swap;
    swap(first.grid, second.grid);
    swap(first.planes, second.planes);
}

// Constructorul de copiere (Deep Copy)
Board::Board(const Board &other) {
    std::copy(&other.grid[0][0], &other.grid[0][0] + SIZE * SIZE, &grid[0][0]);

    // clonam fiecare avion polimorfic folosind constructorul virtual clone()
    for (const auto &p: other.planes) {
        if (p) {
            planes.push_back(p->clone());
        }
    }
}

// Operatorul =, copy-and-swap
Board &Board::operator=(Board other) {
    swap(*this, other);
    return *this;
}

bool Board::addPlane(std::unique_ptr<Aeroplane> plane) {
    std::vector<Point> body = plane->getBodyPoints(); // Apel NVI
    Point head = plane->getHead();

    // Verificare margini tablă
    if (head.getX() < 0 || head.getX() >= SIZE || head.getY() < 0 || head.getY() >= SIZE) return false;
    for (const auto &p: body) {
        if (p.getX() < 0 || p.getX() >= SIZE || p.getY() < 0 || p.getY() >= SIZE) return false;
    }

    // Verificare coliziuni în matricea curentă
    if (grid[head.getX()][head.getY()] != '~') return false;
    for (const auto &p: body) {
        if (grid[p.getX()][p.getY()] != '~') return false;
    }

    // Plasare în matrice
    grid[head.getX()][head.getY()] = 'H';
    for (const auto &p: body) {
        grid[p.getX()][p.getY()] = 'B';
    }

    // Transferăm dreptul de proprietate asupra pointerului în vector
    planes.push_back(std::move(plane));
    return true;
}

//procesare atac
char Board::attackCell(const Point &p) {
    int x = p.getX();
    int y = p.getY();

    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return ' ';

    if (grid[x][y] == 'H') {
        grid[x][y] = '!'; // Cap lovit (Doborât)

        char tipAvion = '!'; // Valoare implicită

        // Căutăm avionul doborât în vectorul de avioane
        for (const auto &plane: planes) {
            if (plane && plane->getHead().getX() == x && plane->getHead().getY() == y) {
                // 1. DYNAMIC_CAST pt aflarea tipului de avion
                Aeroplane *rawPtr = plane.get();
                if (dynamic_cast<BomberPlane *>(rawPtr)) tipAvion = 'B';
                else if (dynamic_cast<RocketPlane *>(rawPtr)) tipAvion = 'R';
                else if (dynamic_cast<InterceptorPlane *>(rawPtr)) tipAvion = 'I';

                // descoperim automat corpul avionului mort pe tabla
                // luam pct avionului NVI
                std::vector<Point> corpAvion = plane->getBodyPoints();
                for (const auto &punctCorp: corpAvion) {
                    // Marcăm pe grilă cu '#' celulele corpului acestui avion mort
                    grid[punctCorp.getX()][punctCorp.getY()] = '#';
                }

                break;
            }
        }
        return tipAvion; // returnăm tipul ('B', 'R', 'I')
    } else if (grid[x][y] == 'B') {
        grid[x][y] = 'X'; // Corp lovit în mod normal (dacă trage în el înainte să doboare capul)
        return 'X';
    } else if (grid[x][y] == '~') {
        grid[x][y] = 'O'; // Ratat
        return 'O';
    }
    return grid[x][y];
}

bool Board::isCellAlreadyAttacked(const Point &p) const {
    int x = p.getX();
    int y = p.getY();

    // Verificăm dacă coordonatele sunt în afara tablei
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        return true;
    }

    // Dacă în matrice găsim un simbol de celulă deja atacată, returnăm true
    return (grid[x][y] == 'X' || grid[x][y] == '!' || grid[x][y] == 'O' || grid[x][y] == '#');
}

// bool Board::hasPlanesLeft() const {
//     for (int i = 0; i < SIZE; ++i) {
//         for (int j = 0; j < SIZE; ++j) {
//             if (grid[i][j] == 'B' || grid[i][j] == 'H') {
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// Afișarea grafică a tablei (High-level)
void Board::printBoard(bool hidePlanes) const {
    std::cout << "  ";
    for (int i = 0; i < SIZE; ++i) std::cout << i << " ";
    std::cout << "\n";

    for (int i = 0; i < SIZE; ++i) {
        std::cout << i << " ";
        for (int j = 0; j < SIZE; ++j) {
            char cell = grid[i][j];

            // Dacă ascundem avioanele inamice, mascăm doar 'B' și 'H' (cele vii),
            // dar lăsăm la vedere celulele deja lovite ('X', '!', 'O') și corpul descoperit ('#')
            if (hidePlanes && (cell == 'B' || cell == 'H')) {
                std::cout << "~ ";
            } else {
                std::cout << cell << " ";
            }
        }
        std::cout << "\n";
    }
}
