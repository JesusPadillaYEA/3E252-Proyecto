#pragma once

#include "Barco.hpp"

// Clase 1: Carguero (Barco lento pero con mucha carga)
class Carguero : public Barco {
public:
    Carguero(float x, float y);
    ~Carguero();

    void disparar() override;
    void mover(float deltaTime) override;
    std::string getInfo() const override;

private:
    float carga;
};

// Clase 2: Destructor (Barco rápido y con buen armamento)
class Destructor : public Barco {
public:
    Destructor(float x, float y);
    ~Destructor();

    void disparar() override;
    void mover(float deltaTime) override;
    std::string getInfo() const override;

private:
    int numCañones;
};

// Clase 3: Crucero (Barco balanceado)
class Crucero : public Barco {
public:
    Crucero(float x, float y);
    ~Crucero();

    void disparar() override;
    void mover(float deltaTime) override;
    std::string getInfo() const override;

private:
    float defensa;
};

// Clase 4: Submarino (Barco sigiloso bajo el agua)
class Submarino : public Barco {
public:
    Submarino(float x, float y);
    ~Submarino();

    void disparar() override;
    void mover(float deltaTime) override;
    std::string getInfo() const override;

    void sumergirse();
    void emerger();

private:
    bool sumergido;
    float profundidad;
};
