#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <memory>

namespace ResourceManager {

    struct Resources {
        // Fuentes
        sf::Font font;
        
        // Texturas de barcos
        sf::Texture tDest;
        sf::Texture tSub;
        sf::Texture tMar;
        sf::Texture tPort;
        
        // Texturas especiales
        sf::Texture tUAV;
        sf::Texture tAvionAtaque;
        
        // Animaciones
        std::vector<sf::Texture> animExplosionTex;
        std::vector<sf::Texture> animFuegoTex;
        
        // Música
        sf::Music musicaFondo;
        
        // Buffers de sonido (almacenamiento permanente)
        sf::SoundBuffer bufferRadar;
        sf::SoundBuffer bufferUAV;
        sf::SoundBuffer bufferButton;
        sf::SoundBuffer bufferAirStrike;
        sf::SoundBuffer bufferNotas;
        sf::SoundBuffer bufferError;
        sf::SoundBuffer bufferShotFail;
        sf::SoundBuffer bufferShotDone;
        sf::SoundBuffer bufferDestruccion;
        
        // Sonidos (inicializados con buffers dummy)
        sf::Sound sRadar{bufferRadar};
        sf::Sound sUAV{bufferUAV};
        sf::Sound sButton{bufferButton};
        sf::Sound sAirStrike{bufferAirStrike};
        sf::Sound sNotas{bufferNotas};
        sf::Sound sError{bufferError};
        sf::Sound sShotFail{bufferShotFail};
        sf::Sound sShotDone{bufferShotDone};
        sf::Sound sDestruccion{bufferDestruccion};
        
        bool assetsOk = true;
        bool uavLoaded = false;
        bool avionLoaded = false;
        bool animOk = false;
    };

    inline Resources cargarRecursos() {
        Resources res;
        
        // === FUENTES ===
        if (!res.font.openFromFile("assets/fonts/Ring.ttf")) {
            res.assetsOk = false;
        }
        
        // === TEXTURAS DE BARCOS ===
        if (!res.tDest.loadFromFile("assets/images/destructor .png")) {
            res.assetsOk = false;
        }
        if (!res.tSub.loadFromFile("assets/images/submarino.png")) {
            res.assetsOk = false;
        }
        if (!res.tMar.loadFromFile("assets/images/mar.png")) {
            res.assetsOk = false;
        }
        if (!res.tPort.loadFromFile("assets/images/portaviones.png")) {
            res.tPort = res.tDest;
        }
        
        res.tMar.setRepeated(true);
        
        // === TEXTURAS ESPECIALES ===
        res.uavLoaded = res.tUAV.loadFromFile("assets/images/UAV.png");
        res.avionLoaded = res.tAvionAtaque.loadFromFile("assets/images/avion1.png");
        
        // === ANIMACIONES ===
        res.animExplosionTex.resize(6);
        res.animOk = true;
        for (int i = 0; i < 6; ++i) {
            if (!res.animExplosionTex[i].loadFromFile("assets/animations/explosion/" + std::to_string(i) + ".png")) {
                res.animOk = false;
            }
        }
        
        res.animFuegoTex.resize(7);
        for (int i = 0; i < 7; ++i) {
            if (!res.animFuegoTex[i].loadFromFile("assets/animations/fuego/" + std::to_string(i) + ".png")) {
                // Error silencioso
            }
        }
        
        // === MÚSICA ===
        if (!res.musicaFondo.openFromFile("assets/music/main.ogg")) {
            // Error silencioso
        } else {
            res.musicaFondo.setLooping(true);
            res.musicaFondo.setVolume(50.f);
        }
        
        // === SONIDOS ===
        // Radar
        if (!res.bufferRadar.loadFromFile("assets/sounds/radar.ogg")) {
            // Error silencioso
        }
        res.sRadar = sf::Sound(res.bufferRadar);
        res.sRadar.setLooping(true);
        
        // UAV
        if (!res.bufferUAV.loadFromFile("assets/sounds/uav.ogg")) {
            // Error silencioso
        }
        res.sUAV = sf::Sound(res.bufferUAV);
        
        // Botón
        if (!res.bufferButton.loadFromFile("assets/sounds/button.ogg")) {
            // Error silencioso
        }
        res.sButton = sf::Sound(res.bufferButton);
        
        // Air Strike
        if (!res.bufferAirStrike.loadFromFile("assets/sounds/airstrike.ogg")) {
            // Error silencioso
        }
        res.sAirStrike = sf::Sound(res.bufferAirStrike);
        
        // Notas
        if (!res.bufferNotas.loadFromFile("assets/sounds/notas.ogg")) {
            // Error silencioso
        }
        res.sNotas = sf::Sound(res.bufferNotas);
        
        // Error
        if (!res.bufferError.loadFromFile("assets/sounds/err.ogg")) {
            // Error silencioso
        }
        res.sError = sf::Sound(res.bufferError);
        
        // Disparo - Fallo
        if (!res.bufferShotFail.loadFromFile("assets/sounds/shot_fail.ogg")) {
            // Error silencioso
        }
        res.sShotFail = sf::Sound(res.bufferShotFail);
        
        // Disparo - Acierto
        if (!res.bufferShotDone.loadFromFile("assets/sounds/shot_done.ogg")) {
            // Error silencioso
        }
        res.sShotDone = sf::Sound(res.bufferShotDone);
        
        // Destrucción
        if (!res.bufferDestruccion.loadFromFile("assets/sounds/s_done.ogg")) {
            // Error silencioso
        }
        res.sDestruccion = sf::Sound(res.bufferDestruccion);
        
        return res;
    }

    inline void establecerVolumenes(Resources& res, float volMusica, float volEfectos) {
        res.musicaFondo.setVolume(volMusica);
        res.sRadar.setVolume(volEfectos);
        res.sUAV.setVolume(volEfectos);
        res.sButton.setVolume(volEfectos);
        res.sAirStrike.setVolume(volEfectos);
        res.sNotas.setVolume(volEfectos);
        res.sError.setVolume(volEfectos);
        res.sShotFail.setVolume(volEfectos);
        res.sShotDone.setVolume(volEfectos);
        res.sDestruccion.setVolume(volEfectos);
    }

    inline void pausarAudio(Resources& res) {
        res.musicaFondo.pause();
        res.sRadar.pause();
        res.sUAV.pause();
        res.sButton.pause();
        res.sAirStrike.pause();
        res.sNotas.pause();
        res.sError.pause();
        res.sShotFail.pause();
        res.sShotDone.pause();
        res.sDestruccion.pause();
    }

    inline void reanudarAudio(Resources& res) {
        res.musicaFondo.play();
        if (res.sRadar.getStatus() == sf::Sound::Status::Paused) res.sRadar.play();
        if (res.sUAV.getStatus() == sf::Sound::Status::Paused) res.sUAV.play();
        if (res.sButton.getStatus() == sf::Sound::Status::Paused) res.sButton.play();
        if (res.sAirStrike.getStatus() == sf::Sound::Status::Paused) res.sAirStrike.play();
        if (res.sNotas.getStatus() == sf::Sound::Status::Paused) res.sNotas.play();
        if (res.sError.getStatus() == sf::Sound::Status::Paused) res.sError.play();
        if (res.sShotFail.getStatus() == sf::Sound::Status::Paused) res.sShotFail.play();
        if (res.sShotDone.getStatus() == sf::Sound::Status::Paused) res.sShotDone.play();
        if (res.sDestruccion.getStatus() == sf::Sound::Status::Paused) res.sDestruccion.play();
    }

    inline void detenerSonidosEfectos(Resources& res) {
        res.sRadar.stop();
        res.sUAV.stop();
        res.sButton.stop();
        res.sAirStrike.stop();
        res.sNotas.stop();
        res.sError.stop();
        res.sShotFail.stop();
        res.sShotDone.stop();
        res.sDestruccion.stop();
    }
}
