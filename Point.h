//
// Created by Maria Tuta on 20.05.2026.
//

#ifndef OOP_POINT_H
#define OOP_POINT_H

#include <iostream>

class Point {
private:
    int x = 0;
    int y = 0;

public:
    // Constructor cu valori implicite
    explicit Point(int x = 0, int y = 0);

    // Getteri marcați cu const
    int getX() const;
    int getY() const;

    // Operatorul << definit inline
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        os << "(" << p.x << "," << p.y << ")";
        return os;
    }
};

#endif //OOP_POINT_H