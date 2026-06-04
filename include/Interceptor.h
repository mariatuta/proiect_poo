//
// Created by Maria Tuta on 20.05.2026.
//

#ifndef OOP_INTERCEPTOR_H
#define OOP_INTERCEPTOR_H


#include "Aeroplane.h"

class InterceptorPlane : public Aeroplane {
public:
    InterceptorPlane(Point h, Direction d) : Aeroplane(h, d) {
    }

    std::unique_ptr<Aeroplane> clone() const override {
        return std::make_unique<InterceptorPlane>(*this);
    }

protected:
    std::vector<Point> calculateShapePoints() const override;
};


#endif //OOP_INTERCEPTOR_H
