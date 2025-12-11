#pragma once

#include "Barco.hpp"

// Clase Submarino (Barco sigiloso bajo el agua)
class Submarino : public Barco {
public:
    Submarino(float x, float y);
    ~Submarino();

    // Getters
    bool estaaSumergido() const;
    float getProfundidad() const;
    float getSigiloNivel() const;

    // Métodos heredados
    void disparar() override;
    void mover(float deltaTime) override;
    std::string getInfo() const override;

    // Métodos propios
    void sumergirse();
    void emerger();
    void aumentarProfundidad(float cantidad);
    void disminuirProfundidad(float cantidad);

private:
    bool sumergido;
    float profundidad;      // Profundidad actual
    float profundidadMax;   // Profundidad máxima
    float sigiloNivel;      // Nivel de sigilo (0-100)
};
