#ifndef OOP_TEMPLATES_H
#define OOP_TEMPLATES_H

#include <vector>
#include <stdexcept>
#include <cstdlib>

// 1. Template Class
// O matrice 2D generală pe care o putem folosi și pt caractere, și pt numere, și pt bool.
template <typename T, int Size>
class Grid2D {
private:
    T data[Size][Size];
public:
    Grid2D() {
        fill(T()); // Inițializare default
    }

    // Funcție utilitară să umplem toată matricea dintr-o lovitură
    void fill(T value) {
        for (int i = 0; i < Size; ++i) {
            for (int j = 0; j < Size; ++j) {
                data[i][j] = value;
            }
        }
    }

    // Suprapunerea operatorului [] ne permite să folosim Grid2D exact ca pe o matrice normală!
    T* operator[](int row) { return data[row]; }
    const T* operator[](int row) const { return data[row]; }
};

// 2. Template Function
// Alege un element random din orice fel de vector
template <typename T>
T getRandomElement(const std::vector<T>& vec) {
    if (vec.empty()) {
        throw std::out_of_range("Vectorul este gol!");
    }
    return vec[rand() % vec.size()];
}

#endif // OOP_TEMPLATES_H