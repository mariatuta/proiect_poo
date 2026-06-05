//
// Created by Maria Tuta on 20.05.2026.
//

#include "../include/Aeroplane.h"
#include "../include/Game.h"

int Aeroplane::totalPlanesCreated = 0;

Aeroplane::Aeroplane(Point h, Direction d) : head(h), dir(d) {
    // Excepția 1: Validare coordonate la nivel de obiect
    if (h.getX() < 0 || h.getX() >= 10 || h.getY() < 0 || h.getY() >= 10) {
        throw InvalidCoordinatesException(">>> Obiectul nu poate fi creat: Capul este in afara matricei!");
    }

    // Excepția 2: Validare direcție la nivel de obiect
    if (static_cast<int>(d) < 0 || static_cast<int>(d) > 3) {
        throw InvalidDirectionException(">>> Obiectul nu poate fi creat: Directia este invalidA!");
    }

    totalPlanesCreated++;
}

Aeroplane::~Aeroplane() {
    totalPlanesCreated--;
}
