//
// Created by Maria Tuta on 20.05.2026.
//

#ifndef OOP_BOARD_H
#define OOP_BOARD_H


#include <vector>
#include <memory>
#include "Point.h"
#include "Aeroplane.h"

class Board {
private:
    static constexpr int SIZE = 10;
    char grid[SIZE][SIZE];
    std::vector<std::unique_ptr<Aeroplane>> planes; // smart pointers

public:
    // Constructor implicit
    Board();

    // --- REGULA CELOR 3 (Rule of Three) cu Copy-and-Swap ---
    Board(const Board& other);                // Constructor de copiere (Deep Copy prin .clone())
    Board& operator=(Board other);           // Operator= care folosește copy-and-swap (transmitere prin valoare)
    ~Board() = default;                      // Destructorul este automat gestionat datorită unique_ptr și matricei statice

    // swap
    friend void swap(Board& first, Board& second) noexcept;

    bool addPlane(std::unique_ptr<Aeroplane> plane);
    char attackCell(const Point& p);                 // Procesează un atac la coordonatele date și întoarce rezultatul ('X', '!', 'O')
    bool isCellAlreadyAttacked(const Point& p) const;  //verifica daca celula a fost deja atacata sau este in afara tablei
    bool hasPlanesLeft() const;                      // Verifică dacă mai există avioane nedoborâte pe tablă
    void printBoard(bool hidePlanes) const;          // Afișează tabla
};

#endif //OOP_BOARD_H