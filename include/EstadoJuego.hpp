#pragma once

// Enum simple para controlar el flujo del juego
enum EstadoJuego {
    MENSAJE_P1, // Turno Jugador 1 (Pantalla de aviso)
    TURNO_P1,   // Turno Jugador 1 (Jugando)
    MENSAJE_P2, // Turno Jugador 2 (Pantalla de aviso)
    TURNO_P2    // Turno Jugador 2 (Jugando)
};