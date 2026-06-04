//
// Created by Maria Tuta on 20.05.2026.
//

#ifndef OOP_ROCKET_H
#define OOP_ROCKET_H


#include "Aeroplane.h"

class RocketPlane : public Aeroplane {
public:
    RocketPlane(Point h, Direction d) : Aeroplane(h, d) {
    }

    std::unique_ptr<Aeroplane> clone() const override {
        return std::make_unique<RocketPlane>(*this);
    }

protected:
    std::vector<Point> calculateShapePoints() const override;
};


#endif //OOP_ROCKET_H
