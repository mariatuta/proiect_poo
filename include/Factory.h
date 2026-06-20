//
// Created by Maria Tuta on 20.06.2026.
//

#ifndef OOP_FACTORY_H
#define OOP_FACTORY_H
#include <memory>
#include <stdexcept>
#include "Aeroplane.h"
#include "Interceptor.h"
#include "Bomber.h"
#include "Rocket.h"
#include "Stealth.h"
#include "Point.h"

// Clasa Factory care se ocupă exclusiv cu instanțierea avioanelor
class PlaneFactory {
public:
    // Metodă statică: poate fi apelată direct, fără a crea un obiect de tip PlaneFactory
    static std::unique_ptr<Aeroplane> createPlane(int type, Point head, Direction dir) {
        switch (type) {
            case 0: return std::make_unique<InterceptorPlane>(head, dir);
            case 1: return std::make_unique<BomberPlane>(head, dir);
            case 2: return std::make_unique<RocketPlane>(head, dir);
            case 3: return std::make_unique<StealthPlane>(head, dir);
            default: throw std::invalid_argument("Tip de avion necunoscut in Factory!");
        }
    }
};
#endif //OOP_FACTORY_H
