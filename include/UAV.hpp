#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Estructura para representar un enemigo detectado
struct EnemigoDetectado {
    float x, y;
    std::string tipo;
    float distancia;
};

// Clase UAV (Unmanned Aerial Vehicle - Dron)
class UAV {
public:
    UAV(float x, float y, const std::string& modelo);
    ~UAV();

    // Getters
    float getX() const;
    float getY() const;
    std::string getModelo() const;
    float getVelocidad() const;
    float getAltitud() const;
    float getBateria() const;
    bool estaActivo() const;
    float getAlcanceEscaneo() const;
    bool estaEscaneando() const;

    // Setters
    void setX(float x);
    void setY(float y);
    void setAltitud(float altitud);

    // Métodos básicos
    void patrullar(float deltaTime);
    void disparar();
    void gastarBateria(float cantidad);
    void recargarBateria();
    void encender();
    void apagar();
    void dibujar(sf::RenderWindow& window);
    std::string getInfo() const;

    // Métodos de escaneo de mapa enemigo
    void iniciarEscaneo();          // Inicia el escaneo del mapa
    void detenerEscaneo();          // Detiene el escaneo
    void escanearMapa();            // Realiza un escaneo del área
    std::vector<EnemigoDetectado> getEnemigosDetectados() const; // Retorna lista de enemigos
    void agregarEnemigoDetectado(float x, float y, const std::string& tipo);
    void limpiarEnemigosDetectados();

private:
    float x, y;
    std::string modelo;
    float velocidad;
    float altitud;              // Altura sobre el agua
    float bateria;
    float bateriaMax;
    bool activo;
    bool escaneando;            // Estado del escaneo
    float alcanceEscaneo;       // Radio de escaneo en píxeles
    float consumoBateria;       // Consumo de batería por escaneo
    std::vector<EnemigoDetectado> enemigosDetectados; // Lista de enemigos detectados
    sf::CircleShape forma;
};

