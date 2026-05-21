//
// Created by Maria Tuta on 20.05.2026.
//

#ifndef OOP_AEROPLANE_H
#define OOP_AEROPLANE_H


#include <vector>
#include <memory>
#include "Point.h" // Avem nevoie de clasa Point pentru cap și corp
// #include "Game.h"

enum class Direction { NORTH, SOUTH, EAST, WEST };

class Aeroplane {
protected:
    Point head;
    Direction dir;

    static int totalPlanesCreated;

    //virtual function-fiecare tip de avion isi calculeaza punctele
    virtual std::vector<Point> calculateShapePoints() const = 0;

public:
    Aeroplane(Point h, Direction d) : head(h), dir(d) {
        if (h.getX() < 0 || h.getX() >= 10 || h.getY() < 0 || h.getY() >= 10) {
            throw std::out_of_range(">>> [Constructor Aeroplane] Capul avionului este in afara hartii!");
        }
        totalPlanesCreated++; // Incrementăm contorul static la fiecare obiect nou
    }

    // Destructor virtual obligatoriu pentru clasele de bază polimorfice
    virtual ~Aeroplane() { totalPlanesCreated--; }

    // Constructor virtual (clone)
    virtual std::unique_ptr<Aeroplane> clone() const = 0;

    // interfata nonvirtuala care apelează funcția virtuală protejată
    std::vector<Point> getBodyPoints() const {
        return calculateShapePoints();
    }

    Point getHead() const { return head; }
    //Direction getDirection() const { return dir; }

    // Funcție statică obligatorie pentru a citi membrul static privat
    static int getTotalPlanesCreated() {
        return totalPlanesCreated;
    }
};

#endif //OOP_AEROPLANE_H
