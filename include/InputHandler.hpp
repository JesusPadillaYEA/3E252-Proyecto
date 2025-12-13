#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "GameState.hpp"
#include "Jugador.hpp"
#include "Boton.hpp"
#include "ResourceManager.hpp"

namespace InputHandler {

    // Información sobre zonas del mapa
    struct MapZones {
        sf::FloatRect zonaIzquierda = {{0.f, 0.f}, {500.f, 700.f}};
        sf::FloatRect zonaDerecha = {{500.f, 0.f}, {500.f, 700.f}};
    };

    // Procesar eventos de teclado del menú de pausa
    inline void procesarMenuPausa(const sf::Event::KeyPressed* k, GameState::StateData& data,
                                   ResourceManager::Resources& recursos, sf::Music& musicaFondo) {
        // Cerrar menú con Escape
        if (k->code == sf::Keyboard::Key::Escape) {
            data.juegoPausado = false;
            return;
        }

        if (k->code == sf::Keyboard::Key::Up || k->code == sf::Keyboard::Key::Down) {
            recursos.sButton.play();
            data.opcionMenuSeleccionada = (data.opcionMenuSeleccionada == 0) ? 1 : 0;
        }

        if (k->code == sf::Keyboard::Key::Left) {
            if (data.opcionMenuSeleccionada == 0) {
                data.volMusica -= 5.f;
                if (data.volMusica < 0.f) data.volMusica = 0.f;
                musicaFondo.setVolume(data.volMusica);
                recursos.sButton.play();
            } else {
                data.volEfectos -= 5.f;
                if (data.volEfectos < 0.f) data.volEfectos = 0.f;
                AudioManager::actualizarVolumenEfectos(data.volEfectos,
                    recursos.sRadar, recursos.sUAV, recursos.sButton,
                    recursos.sAirStrike, recursos.sNotas, recursos.sError,
                    recursos.sShotFail, recursos.sShotDone, recursos.sDestruccion);
                recursos.sButton.play();
            }
        } else if (k->code == sf::Keyboard::Key::Right) {
            if (data.opcionMenuSeleccionada == 0) {
                data.volMusica += 5.f;
                if (data.volMusica > 100.f) data.volMusica = 100.f;
                musicaFondo.setVolume(data.volMusica);
                recursos.sButton.play();
            } else {
                data.volEfectos += 5.f;
                if (data.volEfectos > 100.f) data.volEfectos = 100.f;
                AudioManager::actualizarVolumenEfectos(data.volEfectos,
                    recursos.sRadar, recursos.sUAV, recursos.sButton,
                    recursos.sAirStrike, recursos.sNotas, recursos.sError,
                    recursos.sShotFail, recursos.sShotDone, recursos.sDestruccion);
                recursos.sButton.play();
            }
        }
    }

    // Procesar tecla Escape
    inline void procesarEscape(GameState::StateData& data, ResourceManager::Resources& recursos,
                               Boton& btnAtacar, Boton& btnMover, Boton& btnRadar, Boton& btnNotas) {
        if (data.idGanador != 0) return;  // Se maneja en main

        if (data.cargandoDisparo || data.apuntando || data.moviendo || data.sel != -1 || 
            data.modoRadar || data.modoNotas || data.faseAtaqueAereo > 0) {
            data.cargandoDisparo = false;
            data.apuntando = false;
            data.moviendo = false;
            data.sel = -1;
            data.modoRadar = false;
            data.modoNotas = false;
            data.faseAtaqueAereo = 0;
            data.lanzandoUAV = false;
            data.lanzandoUAVRefuerzo = false;
            btnAtacar.resetColor();
            btnMover.resetColor();
            btnRadar.resetColor();
            btnNotas.resetColor();
            AudioManager::detenerTodosSonidos(recursos.sRadar, recursos.sUAV, recursos.sButton,
                                               recursos.sAirStrike, recursos.sNotas, recursos.sError,
                                               recursos.sShotFail, recursos.sShotDone, recursos.sDestruccion);
        } else {
            data.juegoPausado = !data.juegoPausado;
        }
    }

    // Procesar selección de barco
    inline void procesarSeleccionBarco(Jugador* jugadorActual, const sf::Vector2f& mousePos,
                                       Boton& btnAtacar, Boton& btnMover,
                                       Boton& btnRadar, Boton& btnNotas,
                                       GameState::StateData& data) {
        if (!jugadorActual) return;

        if (!btnAtacar.esClickeado(mousePos) && !btnMover.esClickeado(mousePos) &&
            !btnRadar.esClickeado(mousePos) && !btnNotas.esClickeado(mousePos)) {
            
            auto& flota = jugadorActual->getFlota();
            for (size_t i = 0; i < flota.size(); ++i) {
                if (flota[i].destruido) continue;
                if (flota[i].sprite.getGlobalBounds().contains(mousePos)) {
                    data.sel = (int)i;
                    // NO desactivar modo movimiento - permite cambiar de barco mientras se mueve
                    data.apuntando = false;
                    btnAtacar.resetColor();
                    return;
                }
            }
            // Solo limpiar sel si no estamos moviendo
            if (!data.moviendo) {
                data.sel = -1;
            }
        }
    }

    // Determinar zona objetivo para disparo
    inline sf::FloatRect* obtenerZonaObjetivo(GameState::StateData& data, Jugador* jugador, 
                                              const MapZones& zonas) {
        if (data.sel != -1 && jugador) {
            float barcoX = jugador->getFlota()[data.sel].sprite.getPosition().x;
            return (barcoX < 500.f) ? (sf::FloatRect*)&zonas.zonaDerecha : (sf::FloatRect*)&zonas.zonaIzquierda;
        }
        return nullptr;
    }
}
