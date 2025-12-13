#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

// Estructuras auxiliares movidas aquí
struct ExplosionVisual {
    sf::Sprite sprite;
    float frameTime = 0.f;
    int currentFrame = 0;
    ExplosionVisual(const sf::Texture& texture, sf::Vector2f pos) : sprite(texture) {
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
        sprite.setPosition(pos);
    }
};

struct FuegoVisual {
    sf::Sprite sprite;
    float frameTime = 0.f;
    int currentFrame;
    FuegoVisual(const sf::Texture& texture, sf::Vector2f pos, int startFrame) : sprite(texture) {
        sprite.setPosition(pos);
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
        currentFrame = startFrame;
        float escala = 1.0f + static_cast<float>(std::rand() % 5) / 10.0f;
        sprite.setScale({escala, escala});
    }
};

struct Particula {
    sf::Vector2f pos, vel;
    float vida = 1.0f, angulo, velAngulo, escala = 1.0f;
    sf::Color color;
    Particula(sf::Vector2f p, sf::Color c) : pos(p), color(c) {
        float a = (std::rand() % 360) * 3.14159f / 180.f;
        float v = (std::rand() % 60 + 40) / 10.f;
        vel = { std::cos(a) * v, std::sin(a) * v };
        angulo = static_cast<float>(std::rand() % 360);
        velAngulo = static_cast<float>((std::rand() % 200) - 100);
    }
};

class EffectManager {
public:
    std::vector<ExplosionVisual> explosiones;
    std::vector<FuegoVisual> fuegos;
    std::vector<Particula> particulas;

    void addExplosion(const sf::Texture& tex, sf::Vector2f pos) {
        explosiones.emplace_back(tex, pos);
    }

    void addFuego(const std::vector<sf::Texture>& animFuegoTex, float colX, float winHeight) {
        int cantidadFuego = 40;
        float alturaTramo = winHeight / cantidadFuego;
        for (int i = 0; i < cantidadFuego; ++i) {
            float yBase = i * alturaTramo;
            float variacionY = static_cast<float>(std::rand() % (int)alturaTramo);
            float offsetX = (std::rand() % 50) - 25.f;
            int frame = std::rand() % 7;
            
            FuegoVisual f(animFuegoTex[frame], {colX + offsetX, yBase + variacionY}, frame);
            f.sprite.setTexture(animFuegoTex[frame]); // Asegurar textura inicial
            fuegos.push_back(f);
        }
    }

    void addParticulas(sf::Vector2f pos, int count) {
        std::vector<sf::Color> paleta = {sf::Color::Red, sf::Color::Green, sf::Color::Cyan, sf::Color::Magenta, sf::Color::Yellow, sf::Color::White};
        sf::Color col = paleta[std::rand() % paleta.size()];
        for(int i=0; i<count; ++i) particulas.emplace_back(pos, col);
    }

    void update(float dt, const std::vector<sf::Texture>& expTex, const std::vector<sf::Texture>& fueTex) {
        // Explosiones
        for (size_t i = 0; i < explosiones.size(); ) {
            explosiones[i].frameTime += dt;
            if (explosiones[i].frameTime > 0.1f) {
                explosiones[i].frameTime = 0.f;
                explosiones[i].currentFrame++;
                if (explosiones[i].currentFrame >= (int)expTex.size()) {
                    explosiones.erase(explosiones.begin() + i);
                    continue;
                }
                explosiones[i].sprite.setTexture(expTex[explosiones[i].currentFrame]);
            }
            ++i;
        }
        // Fuego
        for (auto& f : fuegos) {
            f.frameTime += dt;
            if (f.frameTime > 0.1f) {
                f.frameTime = 0.f;
                f.currentFrame = (f.currentFrame + 1) % fueTex.size();
                f.sprite.setTexture(fueTex[f.currentFrame]);
            }
        }
        // Particulas
        for (size_t i = 0; i < particulas.size(); ) {
            particulas[i].pos += particulas[i].vel;
            particulas[i].vel.y += 0.15f;
            particulas[i].angulo += particulas[i].velAngulo * dt;
            particulas[i].vida -= dt * 0.8f;
            particulas[i].escala = particulas[i].vida;
            if (particulas[i].vida <= 0.f) particulas.erase(particulas.begin() + i);
            else ++i;
        }
    }

    void draw(sf::RenderWindow& win) {
        for (const auto& f : fuegos) win.draw(f.sprite);
        for (const auto& e : explosiones) win.draw(e.sprite);
        
        sf::RectangleShape pixel({3.f, 3.f});
        pixel.setOrigin({1.5f, 1.5f});
        for (const auto& p : particulas) {
            pixel.setPosition(p.pos);
            pixel.setRotation(sf::degrees(p.angulo));
            pixel.setScale({p.escala, p.escala});
            sf::Color c = p.color;
            c.a = static_cast<std::uint8_t>(p.vida * 255);
            pixel.setFillColor(c);
            win.draw(pixel);
        }
    }
    
    void clearFuegos() { fuegos.clear(); }
};