#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>
#include <limits>

// --- CLASA 1: Point ---
class Point {
private:
    int x = 0, y = 0;

public:
    explicit Point(int x = 0, int y = 0) : x(x), y(y) {}

    int getX() const { return x; }
    int getY() const { return y; }

    // Operator << pentru Point
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        os << "(" << p.x << "," << p.y << ")";
        return os;
    }
};

enum class PlaneType { INTERCEPTOR, BOMBER };
enum class Direction { NORTH, SOUTH, EAST, WEST };
enum class Difficulty { EASY, ADVANCED };

// --- CLASA 2: Aeroplane ---

class Aeroplane {
private:
    Point head;
    Direction dir = Direction::NORTH;
    PlaneType type = PlaneType::BOMBER;

public:
    Aeroplane(Point h, Direction d, PlaneType t) : head(h), dir(d), type(t) {
    }

    //calculam matematic toate cele 10 puncte ale bomber-ului sau cele 6 ale interceptor-ului
    std::vector<Point> getBodyPoints() const {
        std::vector<Point> body;
        int hx = head.getX();
        int hy = head.getY();

        if (type == PlaneType::BOMBER) {
            if (dir == Direction::NORTH) {
                for (int i = 1; i <= 3; ++i) body.emplace_back(hx + i, hy); // Corp central
                body.emplace_back(hx + 1, hy - 1);
                body.emplace_back(hx + 1, hy - 2); // Aripi stanga
                body.emplace_back(hx + 1, hy + 1);
                body.emplace_back(hx + 1, hy + 2); // Aripi dreapta
                body.emplace_back(hx + 3, hy - 1);
                body.emplace_back(hx + 3, hy + 1); // Coada
            } else if (dir == Direction::SOUTH) {
                for (int i = 1; i <= 3; ++i) body.emplace_back(hx - i, hy);
                body.emplace_back(hx - 1, hy - 1);
                body.emplace_back(hx - 1, hy - 2);
                body.emplace_back(hx - 1, hy + 1);
                body.emplace_back(hx - 1, hy + 2);
                body.emplace_back(hx - 3, hy - 1);
                body.emplace_back(hx - 3, hy + 1);
            } else if (dir == Direction::EAST) {
                for (int j = 1; j <= 3; ++j) body.emplace_back(hx, hy - j);
                body.emplace_back(hx - 1, hy - 1);
                body.emplace_back(hx - 2, hy - 1);
                body.emplace_back(hx + 1, hy - 1);
                body.emplace_back(hx + 2, hy - 1);
                body.emplace_back(hx - 1, hy - 3);
                body.emplace_back(hx + 1, hy - 3);
            } else if (dir == Direction::WEST) {
                for (int j = 1; j <= 3; ++j) body.emplace_back(hx, hy + j);
                body.emplace_back(hx - 1, hy + 1);
                body.emplace_back(hx - 2, hy + 1);
                body.emplace_back(hx + 1, hy + 1);
                body.emplace_back(hx + 2, hy + 1);
                body.emplace_back(hx - 1, hy + 3);
                body.emplace_back(hx + 1, hy + 3);
            }
        } else {
            if (dir == Direction::NORTH) {
                body.emplace_back(hx + 1, hy); // Corp central
                body.emplace_back(hx + 1, hy - 1); // Aripi stanga
                body.emplace_back(hx + 1, hy + 1); // Aripi dreapta
                body.emplace_back(hx + 2, hy);
                body.emplace_back(hx + 3, hy); // Coada
            } else if (dir == Direction::SOUTH) {
                body.emplace_back(hx - 1, hy);
                body.emplace_back(hx - 1, hy - 1);
                body.emplace_back(hx - 1, hy + 1);
                body.emplace_back(hx - 2, hy);
                body.emplace_back(hx - 3, hy);
            } else if (dir == Direction::EAST) {
                body.emplace_back(hx, hy - 1);
                body.emplace_back(hx - 1, hy - 1);
                body.emplace_back(hx + 1, hy - 1);
                body.emplace_back(hx, hy - 2);
                body.emplace_back(hx, hy - 3);
            } else if (dir == Direction::WEST) {
                body.emplace_back(hx, hy + 1);
                body.emplace_back(hx - 1, hy + 1);
                body.emplace_back(hx + 1, hy + 1);
                body.emplace_back(hx, hy + 2);
                body.emplace_back(hx, hy + 3);
            }
        }
        return body;
    }

    Point getHead() const { return head; }

    // Operator << pentru Aeroplane
    friend std::ostream& operator<<(std::ostream& os, const Aeroplane& a) {
        os << "Avion [" << (a.type == PlaneType::BOMBER ? "Bombardier" : "Interceptor") << "] cu capul la " << a.head;
        return os;
    }
};

// --- CLASA Board ( Rule of Three) ---

enum class AttackResult { MISS, HIT_BODY, HIT_HEAD, ALREADY_SHOT, INVALID_COORD };

class Board {
private:
    static constexpr int SIZE = 10;
    int** grid; // 0=Celula goala, 1=Corp, 2=Cap, 3=Lovit(Corp), 4=Ratat(Celula goala), 5=Distrus(Cap)
    std::vector<Aeroplane> planes; // Compunere

public:
    // --- Constructor cu parametri ---
    Board() {
        grid = new int *[SIZE];
        for (int i = 0; i < SIZE; ++i) {
            grid[i] = new int[SIZE]{0};
        }
    }

    // ---Constructor de copiere ---
    Board(const Board &other) : planes(other.planes) {
        grid = new int*[SIZE];
        for (int i = 0; i < SIZE; ++i) {
            grid[i] = new int[SIZE];
            for (int j = 0; j < SIZE; ++j) {
                grid[i][j] = other.grid[i][j];
            }
        }
    }

    // --- Operator= ---
    Board &operator=(const Board &other) {
        if (this != &other) {
            int **newGrid = new int *[SIZE];
            for (int i = 0; i < SIZE; ++i) {
                newGrid[i] = new int[SIZE];
                for (int j = 0; j < SIZE; ++j) {
                    newGrid[i][j] = other.grid[i][j];
                }
            }

            //stergem vechiul grid doar dupa ce alocarea celui nou a reusit
            for (int i = 0; i < SIZE; ++i) {
                delete[] grid[i];
            }
            delete[] grid;

            grid = newGrid;
            planes = other.planes;
        }
        return *this;
    }

    // --- Destructor ---
    ~Board() {
        for (int i = 0; i < SIZE; ++i) {
            delete[] grid[i];
        }
        delete[] grid;
    }

    friend std::ostream& operator<<(std::ostream& os, const Board& b) {
        int alive = 0;
        for (const auto& plane : b.planes) {
            int hx = plane.getHead().getX();
            int hy = plane.getHead().getY();

            //valoarea 2 inseamna ca este cap si este alive
            if (b.grid[hx][hy] == 2) {
                alive++;
            }
        }
        os << "Flota activa: [" << alive << " avioane ramase pe campul de lupta]";
        return os;
    }

    int getCellValue(int r, int c) const {
        if (r >= 0 && r < SIZE && c >= 0 && c < SIZE)
            return grid[r][c];
        return -1;
    }

    // --- validare și plasare ---
    bool validateAndPlace(const Aeroplane& a) {
        std::vector<Point> pts = a.getBodyPoints();
        Point head = a.getHead();
        pts.emplace_back(head.getX(), head.getY());

        for (const auto& p : pts) {
            if (p.getX() < 0 || p.getX() >= SIZE || p.getY() < 0 || p.getY() >= SIZE) return false;
            if (grid[p.getX()][p.getY()] != 0) return false;
        }
        for (const auto& p : pts) {
            if (p.getX() == head.getX() && p.getY() == head.getY())
                grid[p.getX()][p.getY()] = 2; // cap
            else
                grid[p.getX()][p.getY()] = 1; // corp
        }

        planes.push_back(a);
        return true;
    }

    // --- procesare atac ---
    AttackResult receiveAttack(Point p) {
        if (p.getX() < 0 || p.getX() >= SIZE || p.getY() < 0 || p.getY() >= SIZE)
            return AttackResult::INVALID_COORD;

        int& cell = grid[p.getX()][p.getY()];

        if (cell == 2) {
            cell = 5; // Head hit -> Dead
            return AttackResult::HIT_HEAD;
        }
        else if (cell == 1) {
            cell = 3; // Body hit -> Hit
            return AttackResult::HIT_BODY;
        }
        else if (cell == 0) {
            cell = 4; // Empty cell -> Miss
            return AttackResult::MISS;
        }

        return AttackResult::ALREADY_SHOT; // Dacă e deja 3, 4 sau 5
    }

    // --- afisare---
    // hidePlanes = true pentru tabla inamicului (se vede doar unde a tras jucatorul)
    // hidePlanes = false pentru tabla jucatorului (se vede tot)
    void display(bool hidePlanes) const {
        std::cout << "   ";
        for (int j = 0; j < SIZE; ++j) std::cout << j << " ";
        std::cout << "\n  ---------------------\n";

        for (int i = 0; i < SIZE; ++i) {
            std::cout << i << " |";
            for (int j = 0; j < SIZE; ++j) {
                int val = grid[i][j];

                if (val == 0) std::cout << "~ "; // Celula goala(neatinsa)
                else if (val == 1 || val == 2) {
                    if (hidePlanes) std::cout << "~ "; // Ascunde avionul inamic viu
                    else std::cout << (val == 2 ? "^ " : "# "); // Arată avionul tău
                }
                else if (val == 3) std::cout << "X "; // Corp lovit
                else if (val == 4) std::cout << "O "; // celula goala lovită (Miss)
                else if (val == 5) std::cout << "! "; // Cap lovit (Avion distrus)
            }
            std::cout << "|\n";
        }
        std::cout << "  ---------------------\n";
    }

};

// class AIPlayer {
// private:
//     Board myBoard;
//     bool hitMap[10][10]; // Ține minte unde a tras deja
//
// public:
//     AIPlayer() {
//         for(int i=0; i<10; ++i)
//             for(int j=0; j<10; ++j) hitMap[i][j] = false;
//         std::srand(std::time(0));
//     }
//
//     Board& getBoard() { return myBoard; }
//
//     Point makeRandomMove() {
//         int x, y;
//         do {
//             x = std::rand() % 10;
//             y = std::rand() % 10;
//         } while (hitMap[x][y]); // Caută un punct unde n-a mai tras
//
//         hitMap[x][y] = true;
//         return Point(x, y);
//     }
// };

//--- CLASA Game ---

class Game {
private:
    Board playerBoard;
    Board aiBoard;
    std::string projectName;
    int playerPlanesAlive;
    int aiPlanesAlive;


    Difficulty gameDifficulty = Difficulty::EASY;
    bool huntingMode = false;
    std::vector<Point> targetsToTry;

    static void clearInput() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

public:
    // Constructor cu parametri
    explicit Game(const std::string &name) : projectName(name), playerPlanesAlive(3), aiPlanesAlive(3) {
    }

    // Logica AI-ului de a-și plasa singur avioanele (Random)
    void setupAIBoard(PlaneType typeToMatch) {
        int count = 0;
        while (count < 3) {
            if (aiBoard.validateAndPlace(Aeroplane(Point(rand()%10, rand()%10), static_cast<Direction>(rand()%4), typeToMatch)))
                count++;
        }
        std::cout << "AI-ul si-a pregatit flota simetrica.\n";
    }

    // crearea boardului cu avioanele jucatorului
    void setupPlayerBoard() {
        std::cout << "--- Configurare Flota Jucator (3 Avioane) ---\n";

        std::cout << "Alege dificultatea AI (0: Incepator, 1: Avansat): ";
        int diffChoice;
        if (!(std::cin >> diffChoice)) {
            clearInput();
            diffChoice = 0;
        }
        gameDifficulty = (diffChoice == 1) ? Difficulty::ADVANCED : Difficulty::EASY;

        int count = 0;
        PlaneType chosenType = PlaneType::BOMBER;

        while (count < 3) {
            if (count == 0) {
                std::cout << "Alege tipul flotei pentru acest meci (0: Interceptor, 1: Bombardier): ";
                int t;
                std::cin >> t;
                chosenType = static_cast<PlaneType>(t);
            }

            std::cout << "Avion " << count + 1 << " - Introdu Cap (x y) si Directie (0:N, 1:S, 2:E, 3:W): ";
            int x, y, d;
            if (!(std::cin >> x >> y >> d)) {
                clearInput();
                continue;
            }

            Direction dir = static_cast<Direction>(d);
            Aeroplane a(Point(x, y), dir, chosenType);

            if (playerBoard.validateAndPlace(a)) {
                std::cout << "Avion plasat cu succes!\n";
                count++;
            } else {
                std::cout << "Pozitie invalida sau suprapunere! Incearca din nou.\n";
            }
        }
        setupAIBoard(chosenType);
    }

    //am implementat nivel easy si advanced
    //nivel easy= ai ul trage in celule random
    //nivel avansat= daca nimereste ceva, huntingmode=true, si adauga cei 4 vecini în stiva
    //o extrage pe ultima adaugata, si continua tot asa prin backtracking

    void startBattle() {
        std::cout << "\n=== START BATAIE: " << projectName << " ===\n";

        while (playerPlanesAlive > 0 && aiPlanesAlive > 0) {

            // --- PASUL 1: AFIȘARE TABLA TA (LA VEDERE) ---
            std::cout << "\nTABLA TA (Avioanele tale):\n";
            playerBoard.display(false); //vezi ^ si #
            //std::cout << "Avioanele tale ramase: " << playerPlanesAlive << "\n";

            // --- PASUL 2: AFIȘARE TABLA INAMICULUI ---
            std::cout << "\nTABLA INAMICULUI (Atacurile tale):\n";
            aiBoard.display(true); // vezi doar unde ai tras (O, X, !)
            //std::cout << "Avioane inamice ramase: " << aiPlanesAlive << "\n";

            std::cout << *this;

            // --- PASUL 3: RÂNDUL JUCĂTORULUI ---
            int tx, ty;
            AttackResult res;
            bool validTurn = false;
            do {
                std::cout << "\nRANDUL TAU! Unde ataci? (x y): ";
                if (!(std::cin >> tx >> ty)) {
                    std::cout << ">>> Te rog introdu numere valide!\n";
                    clearInput();
                    continue;
                }

                res = aiBoard.receiveAttack(Point(tx, ty));

                if (res == AttackResult::INVALID_COORD) {
                    std::cout << ">>> Coordonate in afara hartii (0-9)! Incearca din nou.\n";
                } else if (res == AttackResult::ALREADY_SHOT) {
                    std::cout << ">>> Ai mai tras aici! Alege alta casuta.\n";
                } else {
                    // Dacă am ajuns aici, atacul a fost MISS, HIT_BODY sau HIT_HEAD
                    validTurn = true;
                    if (res == AttackResult::HIT_HEAD) {
                        std::cout << ">>> EXPLOZIE! Ai doborat un avion inamic!\n";
                        aiPlanesAlive--;
                    } else if (res == AttackResult::HIT_BODY) {
                        std::cout << ">>> Lovit in corp!\n";
                    } else {
                        std::cout << ">>> Nimic. Mai incerca.\n";
                    }
                }
            } while (!validTurn); // Repeta pana cand jucatorul face o miscare valida

            if (aiPlanesAlive == 0) {
                std::cout << "\nFELICITARI! Ai distrus toata flota inamica! Ai castigat.\n";
                break;
            }

            // --- PASUL 4: RÂNDUL AI-ULUI ---
            std::cout << "\nAI-ul se pregateste sa atace...\n";
            Point aiMove;
            AttackResult aiRes;
            do {
                if (gameDifficulty == Difficulty::ADVANCED && huntingMode && !targetsToTry.empty()) {
                    aiMove = targetsToTry.back();
                    targetsToTry.pop_back();
                } else {
                    aiMove = Point(rand() % 10, rand() % 10);
                }
                aiRes = playerBoard.receiveAttack(aiMove);
            } while (aiRes == AttackResult::ALREADY_SHOT || aiRes == AttackResult::INVALID_COORD);

            std::cout << "AI-ul a tras la " << aiMove << ": ";
            if (aiRes == AttackResult::HIT_BODY) {
                std::cout << "Lovit in corp!\n";
                if (gameDifficulty == Difficulty::ADVANCED) {
                    huntingMode = true;
                    int x = aiMove.getX();
                    int y = aiMove.getY();

                    const int dx[] = {-1, 1, 0, 0};
                    const int dy[] = {0, 0, -1, 1};

                    for (int i = 0; i < 4; ++i) {
                        int nx = x + dx[i];
                        int ny = y + dy[i];

                        //verific sa fie pe tabla
                        if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10) {
                            int cellStatus = playerBoard.getCellValue(nx, ny);

                            //verific sa nu fi fost atacata de ai deja
                            if (cellStatus == 0 || cellStatus == 1 || cellStatus == 2) {
                                targetsToTry.emplace_back(nx, ny);
                            }
                        }
                    }
                }
            } else if (aiRes == AttackResult::HIT_HEAD) {
                std::cout << "CRITICAL! Ti-a doborat un avion!\n";
                playerPlanesAlive--;
                if (gameDifficulty == Difficulty::ADVANCED) {
                    huntingMode = false;
                    targetsToTry.clear();
                }
            } else {
                std::cout << "Miss.\n";
            }

            if (playerPlanesAlive == 0) {
                std::cout << "\nDEFEAT! Toate avioanele tale au fost distruse.\n";
                std::cout << "Iata unde era flota inamica:\n" << aiBoard << "\n";
                return;
            }
        }
    }

    // Operator <<
    friend std::ostream &operator<<(std::ostream &os, const Game &g) {
        os << "--- Status Joc: " << g.projectName << " ---\n";
        os << " JUCATOR: " << g.playerBoard << "\n";
        os << " INAMIC : " << g.aiBoard << "\n";
        return os;
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr))); // Pentru randomizare reală
    Game myGame("PaperSkyTactics");
    myGame.setupPlayerBoard();
    std::cout << myGame << std::endl;
    myGame.startBattle();
    return 0;
}
