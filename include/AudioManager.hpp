#pragma once
#include <SFML/Audio.hpp>

namespace AudioManager {

    struct AudioState {
        sf::Sound* sonidoDisparoActual = nullptr;
    };

    // Reproducir sonido de forma segura
    inline void reproducirSonido(sf::Sound& sonido) {
        if (sonido.getStatus() == sf::Sound::Status::Playing) {
            sonido.stop();
        }
        sonido.play();
    }

    // Detener todos los sonidos de efectos (excepto música)
    inline void detenerTodosSonidos(sf::Sound& sRadar, sf::Sound& sUAV, sf::Sound& sButton,
                                    sf::Sound& sAirStrike, sf::Sound& sNotas, sf::Sound& sError,
                                    sf::Sound& sShotFail, sf::Sound& sShotDone, sf::Sound& sDestruccion) {
        sRadar.stop();
        sUAV.stop();
        sButton.stop();
        sAirStrike.stop();
        sNotas.stop();
        sError.stop();
        sShotFail.stop();
        sShotDone.stop();
        sDestruccion.stop();
    }

    // Actualizar volumen global de efectos
    inline void actualizarVolumenEfectos(float nuevoVolumen, 
                                         sf::Sound& sRadar, sf::Sound& sUAV, sf::Sound& sButton,
                                         sf::Sound& sAirStrike, sf::Sound& sNotas, sf::Sound& sError,
                                         sf::Sound& sShotFail, sf::Sound& sShotDone, sf::Sound& sDestruccion) {
        sRadar.setVolume(nuevoVolumen);
        sUAV.setVolume(nuevoVolumen);
        sButton.setVolume(nuevoVolumen);
        sAirStrike.setVolume(nuevoVolumen);
        sNotas.setVolume(nuevoVolumen);
        sError.setVolume(nuevoVolumen);
        sShotFail.setVolume(nuevoVolumen);
        sShotDone.setVolume(nuevoVolumen);
        sDestruccion.setVolume(nuevoVolumen);
    }
}
