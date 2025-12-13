#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace AnimationSystem {

    // Estructura para explosiones animadas
    struct ExplosionVisual {
        sf::Sprite sprite;
        float frameTime = 0.f;
        int currentFrame = 0;

        ExplosionVisual(const sf::Texture& texture) : sprite(texture) {
            sf::FloatRect bounds = sprite.getLocalBounds();
            sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
            sprite.setScale({1.0f, 1.0f});
        }
    };

    // Estructura para fuego persistente
    struct FuegoVisual {
        sf::Sprite sprite;
        float frameTime = 0.f;
        int currentFrame = 0;

        FuegoVisual(const sf::Texture& texture, sf::Vector2f pos) : sprite(texture) {
            sprite.setPosition(pos);
            sf::FloatRect bounds = sprite.getLocalBounds();
            sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
            
            currentFrame = std::rand() % 7;
            frameTime = 0.f;
            
            float escala = 1.0f + static_cast<float>(std::rand() % 5) / 10.0f;
            sprite.setScale({escala, escala});
        }
    };

    // Estructura para partículas de fuegos artificiales
    struct Particula {
        sf::Vector2f pos;
        sf::Vector2f vel;
        float vida = 1.0f;
        sf::Color color;
        float angulo = 0.f;
        float velAngulo = 0.f;
        float escala = 1.0f;

        Particula(sf::Vector2f p, sf::Color c) : pos(p), color(c) {
            vida = 1.0f;
            escala = 1.0f;
            
            float a = (std::rand() % 360) * 3.14159f / 180.f;
            float v = (std::rand() % 60 + 40) / 10.f;
            vel = { std::cos(a) * v, std::sin(a) * v };

            angulo = static_cast<float>(std::rand() % 360);
            velAngulo = static_cast<float>((std::rand() % 200) - 100);
        }
    };

    // Actualizar animación de explosiones
    inline void actualizarExplosiones(std::vector<ExplosionVisual>& explosiones,
                                      const std::vector<sf::Texture>& animTextures,
                                      float deltaTime, bool animOk) {
        if (!animOk) return;
        
        for (size_t i = 0; i < explosiones.size(); ) {
            explosiones[i].frameTime += deltaTime;
            
            if (explosiones[i].frameTime > 0.1f) {
                explosiones[i].frameTime = 0.f;
                explosiones[i].currentFrame++;
                
                if (explosiones[i].currentFrame >= 6) {
                    explosiones.erase(explosiones.begin() + i);
                    continue;
                } else {
                    explosiones[i].sprite.setTexture(animTextures[explosiones[i].currentFrame]);
                }
            }
            ++i;
        }
    }

    // Actualizar animación de fuego
    inline void actualizarFuegos(std::vector<FuegoVisual>& fuegos,
                                 const std::vector<sf::Texture>& animTextures,
                                 float deltaTime) {
        for (auto& fuego : fuegos) {
            fuego.frameTime += deltaTime;
            
            if (fuego.frameTime > 0.1f) {
                fuego.frameTime = 0.f;
                fuego.currentFrame++;
                
                if (fuego.currentFrame >= 7) {
                    fuego.currentFrame = 0;
                }
                
                fuego.sprite.setTexture(animTextures[fuego.currentFrame]);
            }
        }
    }

    // Actualizar partículas de fuegos artificiales
    inline void actualizarParticulas(std::vector<Particula>& particulas, float deltaTime) {
        for (size_t i = 0; i < particulas.size(); ) {
            particulas[i].pos += particulas[i].vel;
            particulas[i].vel.y += 0.15f;  // Gravedad
            
            particulas[i].angulo += particulas[i].velAngulo * deltaTime;
            particulas[i].vida -= deltaTime * 0.8f;
            particulas[i].escala = particulas[i].vida;

            if (particulas[i].vida <= 0.f) {
                particulas.erase(particulas.begin() + i);
            } else {
                ++i;
            }
        }
    }

    // Generar explosión de partículas
    inline void generarExplosion(std::vector<Particula>& particulas, 
                                 const sf::Vector2u& windowSize) {
        if (std::rand() % 20 == 0) {
            float x = static_cast<float>(std::rand() % windowSize.x);
            float y = static_cast<float>(std::rand() % (windowSize.y / 2 + 100));
            
            std::vector<sf::Color> paleta = {
                sf::Color::Red, sf::Color::Green, sf::Color::Cyan,
                sf::Color::Magenta, sf::Color::Yellow, sf::Color::White,
                sf::Color(255, 165, 0)
            };
            sf::Color colorExplosion = paleta[std::rand() % paleta.size()];

            for (int i = 0; i < 50; ++i) {
                particulas.emplace_back(sf::Vector2f(x, y), colorExplosion);
            }
        }
    }

    // Limpiar todas las animaciones
    inline void limpiar(std::vector<ExplosionVisual>& explosiones,
                       std::vector<FuegoVisual>& fuegos,
                       std::vector<Particula>& particulas) {
        explosiones.clear();
        fuegos.clear();
        particulas.clear();
    }
}
