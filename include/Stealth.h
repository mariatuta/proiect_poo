//
// Created by Maria Tuta on 05.06.2026.
//

#ifndef OOP_STEALTH_H
#define OOP_STEALTH_H
#include "Aeroplane.h"

class StealthPlane : public Aeroplane {
protected:
    std::vector<Point> calculateShapePoints() const override;

public:
    StealthPlane(Point h, Direction d) : Aeroplane(h, d) {}
    std::unique_ptr<Aeroplane> clone() const override {
        return std::make_unique<StealthPlane>(*this);
    }
};

#endif //OOP_STEALTH_H