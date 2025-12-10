#pragma once

#include "Barco.hpp"

// Clase Destructor (Barco rápido y con buen armamento)
class Destructor : public Barco {
public:
    Destructor(float x, float y);
    ~Destructor();

    // Getters
    int getNumCañones() const;
    float getVelocidadMaxima() const;
    float getRadarAlcance() const;

    // Métodos heredados
    void disparar() override;
    void mover(float deltaTime) override;
    std::string getInfo() const override;

    // Métodos propios
    void activarRadar();
    void desactivarRadar();
    void aumentarVelocidad();
    void disminuirVelocidad();
    void dispararMultiple();  // Dispara con múltiples cañones

private:
    int numCañones;         // Número de cañones
    float velocidadMaxima;  // Velocidad máxima del destructor
    float radarAlcance;     // Alcance del radar
    bool radarActivo;       // Estado del radar
};
