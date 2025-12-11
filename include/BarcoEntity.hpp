#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct BarcoEntity {
    sf::Sprite sprite;
    std::string nombre;
    
    // --- NUEVAS PROPIEDADES DE ESTADO ---
    int hp;           // Puntos de vida actuales
    int maxHp;        // Vida máxima
    bool destruido;   // Bandera de existencia

    BarcoEntity(const sf::Texture& texture, std::string name, int vida = 100) 
        : sprite(texture), nombre(name), hp(vida), maxHp(vida), destruido(false) {
    }

    // Método simple para recibir daño
    void recibirDano(int cantidad) {
        if (destruido) return;

        hp -= cantidad;
        if (hp <= 0) {
            hp = 0;
            destruido = true;
            // Opcional: Cambiar color a gris o rojo oscuro para indicar muerte visualmente
            // sprite.setColor(sf::Color(50, 50, 50, 100)); 
        }
    }
};