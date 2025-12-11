#pragma once

#include "Barco.hpp"
#include <vector>

// Clase Portaviones (Barco grande con capacidad de almacenar aviones)
class Portaviones : public Barco {
public:
    Portaviones(float x, float y);
    ~Portaviones();

    // Getters
    int getCapacidadAviones() const;
    int getAvionesDisponibles() const;
    float getRadarAlcance() const;
    bool puedeDecolarAvion() const;

    // Métodos heredados
    void disparar() override;
    void mover(float deltaTime) override;
    std::string getInfo() const override;

    // Métodos propios
    void decollarAvion();        // Despega un avión
    void aterrizarAvion();       // Aterriza un avión
    void activarDefensa();       // Activa defensa aérea
    void desactivarDefensa();    // Desactiva defensa aérea
    void reparar(float cantidad); // Repara el portaviones
    void recargarAviones(int cantidad);

private:
    int capacidadAviones;    // Capacidad máxima de aviones
    int avionesDisponibles;  // Aviones disponibles actualmente
    float radarAlcance;      // Alcance del radar
    bool defensaActiva;      // Estado de la defensa aérea
    std::vector<int> hangar; // Vector de aviones en el hangar
};
