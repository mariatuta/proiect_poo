//
// Created by Maria Tuta on 20.05.2026.
//

#include "Rocket.h"

std::vector<Point> RocketPlane::calculateShapePoints() const {
    std::vector<Point> body;
    int hx = head.getX();
    int hy = head.getY();

    // Logica pentru "Rachetă": 3 puncte în linie dreaptă în spatele capului
    for (int i = 1; i <= 3; ++i) {
        if (dir == Direction::NORTH) body.emplace_back(hx + i, hy);
        else if (dir == Direction::SOUTH) body.emplace_back(hx - i, hy);
        else if (dir == Direction::EAST) body.emplace_back(hx, hy - i);
        else if (dir == Direction::WEST) body.emplace_back(hx, hy + i);
    }
    return body;
}