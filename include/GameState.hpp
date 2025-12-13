#pragma once
#include <SFML/Graphics.hpp>
#include "EstadoJuego.hpp"
#include "Jugador.hpp"

namespace GameState {

    struct StateData {
        EstadoJuego estado = MENSAJE_P1;
        int idGanador = 0;           // 0 = Jugando, >0 = Juego terminado
        int ganadorDetectado = 0;    // Jugador que ganó temporalmente
        float timerVictoria = 0.f;   // Cronómetro para retraso de animación
        
        bool juegoPausado = false;
        bool moviendo = false;
        bool apuntando = false;
        bool modoRadar = false;
        bool modoNotas = false;
        bool lanzandoUAV = false;
        bool lanzandoUAVRefuerzo = false;
        bool cargandoDisparo = false;
        bool enZonaEnemiga = false;
        
        int sel = -1;  // Índice de barco seleccionado
        float potenciaActual = 0.f;
        float errorTimer = 0.f;
        sf::String msgError = "";
        
        // Fase de ataque
        int faseAtaqueAereo = 0;    // 0 = Inactivo, 1 = Espera, 2 = Volando
        int faseDisparoNormal = 0;  // 0 = Inactivo, 1 = Reproduciendo sonido
        
        sf::Vector2f origenDisparoReal = {0.f, 0.f};
        
        // Audio menu
        int opcionMenuSeleccionada = 0;  // 0 = Música, 1 = Efectos
        float volMusica = 50.f;
        float volEfectos = 50.f;
    };

    // Verificar si un jugador perdió todos sus barcos
    inline bool verificarVictoria(const Jugador& jugador) {
        const auto& flota = jugador.getFlota();
        for (const auto& barco : flota) {
            if (!barco.destruido) return false;
        }
        return true;
    }

    // Procesar transición de turno
    inline void transicionarTurno(StateData& data, Jugador& jugadorActual, Jugador& jugadorEnemigo) {
        if (data.estado == TURNO_P1) {
            data.estado = MENSAJE_P2;
        } else if (data.estado == TURNO_P2) {
            data.estado = MENSAJE_P1;
        }
        
        data.sel = -1;
        data.moviendo = false;
        data.apuntando = false;
        data.cargandoDisparo = false;
        data.modoRadar = false;
        data.modoNotas = false;
        data.lanzandoUAV = false;
        data.faseAtaqueAereo = 0;
        data.errorTimer = 0.f;
    }

    // Actualizar timer de victoria
    inline void actualizarVictoria(StateData& data, float deltaTime) {
        if (data.ganadorDetectado != 0) {
            data.timerVictoria -= deltaTime;
            if (data.timerVictoria <= 0.f) {
                data.idGanador = data.ganadorDetectado;
                data.ganadorDetectado = 0;
            }
        }
    }

    // Resetear estado de juego después de victoria
    inline void resetearJuego(StateData& data) {
        data.estado = MENSAJE_P1;
        data.idGanador = 0;
        data.ganadorDetectado = 0;
        data.timerVictoria = 0.f;
        data.juegoPausado = false;
        data.moviendo = false;
        data.apuntando = false;
        data.modoRadar = false;
        data.modoNotas = false;
        data.lanzandoUAV = false;
        data.lanzandoUAVRefuerzo = false;
        data.cargandoDisparo = false;
        data.enZonaEnemiga = false;
        data.sel = -1;
        data.potenciaActual = 0.f;
        data.errorTimer = 0.f;
        data.msgError = "";
        data.faseAtaqueAereo = 0;
        data.faseDisparoNormal = 0;
        data.opcionMenuSeleccionada = 0;
    }
}
