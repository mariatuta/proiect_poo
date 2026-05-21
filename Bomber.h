//
// Created by Maria Tuta on 20.05.2026.
//

#ifndef OOP_BOMBER_H
#define OOP_BOMBER_H


#include "Aeroplane.h"

// BomberPlane moștenește public Aeroplane
class BomberPlane : public Aeroplane {
public:
    // Apelăm constructorul clasei de bază (Cerință Etapa 2)
    BomberPlane(Point h, Direction d) : Aeroplane(h, d) {
    }

    // Constructor virtual pentru deep copy (clone)
    std::unique_ptr<Aeroplane> clone() const override {
        return std::make_unique<BomberPlane>(*this);
    }

protected:
    // Suprascriem funcția virtuală pentru a returna cele 10 puncte ale bombardierului
    std::vector<Point> calculateShapePoints() const override;
};

#endif //OOP_BOMBER_H
