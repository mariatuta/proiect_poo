//
// Created by Maria Tuta on 20.05.2026.
//

#include "../include/Interceptor.h"


std::vector<Point> InterceptorPlane::calculateShapePoints() const {
    std::vector<Point> body;
    int hx = head.getX();
    int hy = head.getY();

    // Logica ta originală pentru cele 5 puncte ale Interceptorului
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
    return body;
}
