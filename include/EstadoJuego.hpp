#pragma once

enum EstadoJuego {
    MENSAJE_P1, // Pantalla "Turno Jugador 1"
    TURNO_P1,   // P1 mueve sus barcos
    ATACANDO_P1,// P1 selecciona dónde disparar
    
    MENSAJE_P2, // Pantalla "Turno Jugador 2"
    TURNO_P2,   // P2 mueve sus barcos
    ATACANDO_P2 // P2 selecciona dónde disparar
};