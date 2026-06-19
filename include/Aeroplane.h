//
// Created by Maria Tuta on 20.05.2026.
//

#ifndef OOP_AEROPLANE_H
#define OOP_AEROPLANE_H


#include <vector>
#include <memory>
#include "Point.h"
// #include "Game.h"

enum class Direction { NORTH, SOUTH, EAST, WEST };

class Aeroplane {
protected:
    Point head;
    Direction dir;

    static int totalPlanesCreated;

    // virtual function - fiecare tip de avion isi calculeaza punctele
    virtual std::vector<Point> calculateShapePoints() const = 0;

public:
    Aeroplane(Point h, Direction d);

    virtual ~Aeroplane();

    // Constructor virtual (clone)
    virtual std::unique_ptr<Aeroplane> clone() const = 0;

    // Interfața non-virtuală (NVI)
    std::vector<Point> getBodyPoints() const {
        return calculateShapePoints();
    }

    Point getHead() const { return head; }

    // cppcheck-suppress unusedFunction
    static int getTotalPlanesCreated() {
        return totalPlanesCreated;
    }
};

#endif //OOP_AEROPLANE_H
