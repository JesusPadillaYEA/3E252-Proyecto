#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <map>
#include <string>

class ResourceManager {
public:
    sf::Font font;
    
    // Texturas
    sf::Texture tDest, tSub, tMar, tPort, tUAV, tAvionAtaque;
    std::vector<sf::Texture> animExplosionTex;
    std::vector<sf::Texture> animFuegoTex;

    // Buffers de Audio (SFML 3 requiere que el buffer persista mientras el sonido existe)
    sf::SoundBuffer bufRadar, bufUAV, bufButton, bufAirStrike, bufNotas;
    sf::SoundBuffer bufError, bufShotFail, bufShotDone, bufDestruccion;

    // Música (No es buffer, es stream)
    // La música se maneja mejor en la clase Game porque es única, 
    // pero aquí podríamos guardar rutas si quisiéramos.

    bool loaded = false;
    bool uavLoaded = false;
    bool avionLoaded = false;

    ResourceManager() {
        animExplosionTex.resize(6);
        animFuegoTex.resize(7);
    }

    bool loadResources() {
        if (!font.openFromFile("assets/fonts/Ring.ttf")) return false;

        bool assetsOk = true;
        if (!tDest.loadFromFile("assets/images/destructor .png")) assetsOk = false;
        if (!tSub.loadFromFile("assets/images/submarino.png")) assetsOk = false;
        if (!tMar.loadFromFile("assets/images/mar.png")) assetsOk = false;
        if (!tPort.loadFromFile("assets/images/portaviones.png")) tPort = tDest; // Fallback
        tMar.setRepeated(true);

        uavLoaded = tUAV.loadFromFile("assets/images/UAV.png");
        avionLoaded = tAvionAtaque.loadFromFile("assets/images/avion1.png");

        // Cargar Animaciones
        for (int i = 0; i < 6; ++i) {
            if (!animExplosionTex[i].loadFromFile("assets/animations/explosion/" + std::to_string(i) + ".png")) 
                std::cerr << "Error explosion " << i << std::endl;
        }
        for (int i = 0; i < 7; ++i) {
            if (!animFuegoTex[i].loadFromFile("assets/animations/fuego/" + std::to_string(i) + ".png"))
                std::cerr << "Error fuego " << i << std::endl;
        }

        // Cargar Audio Buffers
        loadSound(bufRadar, "assets/sounds/radar.ogg");
        loadSound(bufUAV, "assets/sounds/uav.ogg");
        loadSound(bufButton, "assets/sounds/button.ogg");
        loadSound(bufAirStrike, "assets/sounds/airstrike.ogg");
        loadSound(bufNotas, "assets/sounds/notas.ogg");
        loadSound(bufError, "assets/sounds/err.ogg");
        loadSound(bufShotFail, "assets/sounds/shot_fail.ogg");
        loadSound(bufShotDone, "assets/sounds/shot_done.ogg");
        loadSound(bufDestruccion, "assets/sounds/s_done.ogg");

        loaded = assetsOk;
        return assetsOk;
    }

private:
    void loadSound(sf::SoundBuffer& buf, const std::string& path) {
        if (!buf.loadFromFile(path)) std::cerr << "Error loading " << path << std::endl;
    }
};