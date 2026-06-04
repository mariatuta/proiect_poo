//
// Created by Maria Tuta on 20.05.2026.
//

#include "../include/Bomber.h"

std::vector<Point> BomberPlane::calculateShapePoints() const {
    std::vector<Point> body;
    int hx = head.getX();
    int hy = head.getY();

    // Logica ta matematică originală pentru cele 10 puncte ale Bombardierului
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
    return body;
}
