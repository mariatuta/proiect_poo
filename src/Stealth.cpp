//
// Created by Maria Tuta on 05.06.2026.
//

#include "Stealth.h"

std::vector<Point> StealthPlane::calculateShapePoints() const {
    std::vector<Point> shape;
    int hx = head.getX();
    int hy = head.getY();

    shape.push_back(head);

    if (dir == Direction::NORTH) {
        shape.emplace_back(hx + 1, hy);
        shape.emplace_back(hx + 2, hy - 1);
        shape.emplace_back(hx + 2, hy + 1);
    } else if (dir == Direction::SOUTH) {
        shape.emplace_back(hx - 1, hy);
        shape.emplace_back(hx - 2, hy - 1);
        shape.emplace_back(hx - 2, hy + 1);
    } else if (dir == Direction::EAST) {
        shape.emplace_back(hx, hy - 1);
        shape.emplace_back(hx - 1, hy - 2);
        shape.emplace_back(hx + 1, hy - 2);
    } else if (dir == Direction::WEST) {
        shape.emplace_back(hx, hy + 1);
        shape.emplace_back(hx - 1, hy + 2);
        shape.emplace_back(hx + 1, hy + 2);
    }
    return shape;
}
