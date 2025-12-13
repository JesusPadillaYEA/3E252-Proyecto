#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace TutorialSystem {
    enum TutorialStep {
        INICIO,              // Pantalla inicial
        MENU_PAUSA,          // Aprender menú de pausa
        SELECCIONAR_BARCO,   // Click en barco
        MOVIMIENTO,          // Toggle de movimiento
        ATAQUE,              // Ataque completo (unificado)
        RADAR,               // UAV + Radar
        NOTAS_VER,           // Ver Notas
        PORTAVIONES_CAMBIO,  // Cambio a Portaviones
        AIRSTRIKE,           // Air Strike completo
        COMPLETADO           // Tutorial completado
    };

    struct TutorialData {
        TutorialStep pasoActual = INICIO;
        
        // Flags para detección automática
        bool barcoSeleccionado = false;
        bool movimientoToggled = false;
        bool ataqueCompletado = false;
        bool radarCerrado = false;
        bool notasCerradas = false;
        bool portavionesActivo = false;
        bool airstrikeCompletado = false;
        bool menuPausaVisto = false;
        
        // Estados del tutorial
        float potenciaActual = 0.f;
        bool apuntando = false;
        bool cargandoDisparo = false;
        bool radarActivo = false;
        bool notasActivas = false;
        bool moviendo = false;
    };

    inline std::string obtenerInstruccion(TutorialStep paso) {
        switch (paso) {
            case INICIO:
                return "BIENVENIDO AL TUTORIAL\n\nAprenderas las mecanicas basicas del juego\nPresiona ENTER para comenzar";
            case MENU_PAUSA:
                return "PASO 1: MENU DE PAUSA\n\nPresiona ESC para ver el menu\nAqui puedes ajustar volumen y salir\nPresiona ENTER para continuar";
            case SELECCIONAR_BARCO:
                return "PASO 2: SELECCIONAR BARCO\n\nHaz CLIC en uno de tus barcos (izquierda)";
            case MOVIMIENTO:
                return "PASO 3: MOVIMIENTO\n\nClic en MOVER para activar movimiento\nUsa FLECHAS para mover el barco\nClic MOVER de nuevo para desactivar";
            case ATAQUE:
                return "PASO 4: ATACAR\n\nClic ATACAR -> Apunta al enemigo (derecha)\nMANTEN clic para cargar -> SUELTA para disparar";
            case RADAR:
                return "PASO 5: RADAR\n\nClic en RADAR\nObserva el UAV despegar y detectar enemigos\nClic RADAR de nuevo para cerrar";
            case NOTAS_VER:
                return "PASO 6: NOTAS\n\nClic en NOTAS para ver historial de ataques\nRevisa la informacion registrada\nClic NOTAS de nuevo para cerrar";
            case PORTAVIONES_CAMBIO:
                return "PASO 7: PORTAVIONES\n\nSelecciona tu PORTAVIONES\n(el barco mas grande)";
            case AIRSTRIKE:
                return "PASO 8: ATAQUE AEREO\n\nClic ATACAR con el portaviones seleccionado\nObserva el avion despegar y atacar";
            case COMPLETADO:
                return "TUTORIAL COMPLETADO!\n\nYa dominas las mecanicas basicas del juego\nPresiona ENTER para comenzar a jugar";
            default:
                return "";
        }
    }

    inline void avanzarPaso(TutorialData& tut) {
        switch (tut.pasoActual) {
            case INICIO: tut.pasoActual = MENU_PAUSA; break;
            case MENU_PAUSA: tut.pasoActual = SELECCIONAR_BARCO; break;
            case SELECCIONAR_BARCO: tut.pasoActual = MOVIMIENTO; break;
            case MOVIMIENTO: tut.pasoActual = ATAQUE; break;
            case ATAQUE: tut.pasoActual = RADAR; break;
            case RADAR: tut.pasoActual = NOTAS_VER; break;
            case NOTAS_VER: tut.pasoActual = PORTAVIONES_CAMBIO; break;
            case PORTAVIONES_CAMBIO: tut.pasoActual = AIRSTRIKE; break;
            case AIRSTRIKE: tut.pasoActual = COMPLETADO; break;
            case COMPLETADO: break;
        }
    }

    inline void resetearTutorial(TutorialData& tut) {
        tut.pasoActual = INICIO;
        tut.barcoSeleccionado = false;
        tut.movimientoToggled = false;
        tut.ataqueCompletado = false;
        tut.radarCerrado = false;
        tut.notasCerradas = false;
        tut.portavionesActivo = false;
        tut.airstrikeCompletado = false;
        tut.menuPausaVisto = false;
        tut.potenciaActual = 0.f;
        tut.apuntando = false;
        tut.cargandoDisparo = false;
        tut.radarActivo = false;
        tut.notasActivas = false;
        tut.moviendo = false;
    }
}
