# 3E252-Proyecto
proyecto de programación avanzada (24310401 y 24310402)

### 🚀 Compilación y Ejecución

#### Opción 1: Script Automático (Recomendado)
Ejecuta el script `play.bat` para compilar ambos ejecutables y lanzar el juego:
```bash
.\play.bat
```
Este comando compila `main.exe` y `JuegoProyecto.exe` (launcher), y automáticamente abre el launcher.

#### Opción 2: Desde la Consola
Compila manualmente usando:
```bash
powershell -ExecutionPolicy Bypass -File .\compile.ps1
```
Luego ejecuta el launcher:
```bash
.\bin\JuegoProyecto.exe
```

### 📚 Tutorial Interactivo

El juego incluye un **Tutorial Interactivo** que te enseña los controles básicos de una forma divertida y fácil.

**Para acceder al tutorial:**
1. Abre el launcher (`JuegoProyecto.exe`)
2. Haz clic en el botón **"TUTORIAL"** (botón azul)
3. Sigue las instrucciones paso a paso:
   - Paso 1: Aprende a mover barcos con flechas
   - Paso 2: Selecciona barcos con clicks
   - Paso 3: Apunta tus disparos arrastrando
   - Paso 4: Dispara a los enemigos
   - Paso 5: Abre el menú de pausa

El tutorial es **100% interactivo**: completa cada acción para avanzar al siguiente paso. ¡Muy fácil para que nadie se atore!

Una vez completes el tutorial, volverás automáticamente al launcher listo para jugar.

### 🎯 Objetivo del Juego

Juego de estrategia donde dos jugadores deben aniquilar la flota del otro, varios tipos de barcos repartidos en el océano donde un disparo mal planeado podría definir todo la partida

### 🎮 Controles

Lista los controles:

- **Flechas**: Movimiento (unidades de combate) / movimiento de configuración (música y sonido)
- **ESC**: Menú de pausa y volúmenes / Salir de modos especiales
- **Mouse**: 
  - **Clic Izquierdo**: Seleccionar unidad / Activar botones / Disparar
  - **Arrastrar**: Apuntar disparo (fuerza y dirección)

### ⚙️ Mecánicas y Reglas

#### ⚔️ Sistema de Turnos y Acciones
El juego se rige por una regla estricta de acciones: **Solo atacar finaliza tu turno**.

* **🏃 Movimiento Táctico (Acción Libre)**: 
    * Mover tus barcos **NO consume el turno**. 
    * Puedes reacomodar tu flota libremente para esquivar o mejorar tu ángulo antes de decidir atacar.
    
* **💥 Ataque (Finaliza el Turno)**: 
    * Cualquier forma de agresión (Disparo Normal o Ataque Aéreo) termina inmediatamente tu turno.
    * **Ataque Normal**: Disponible para todas las unidades. Arrastra el mouse para definir potencia y ángulo.

#### 🚢 Habilidades Especiales (Clase Portaviones)
El **Portaviones** es la unidad vital para el soporte táctico. Si es destruido, pierdes acceso inmediato a estas herramientas.

1.  **📡 Radar / UAV (Acción Libre)**
    * **Efecto**: Escanea el campo enemigo y revela posiciones en la Bitácora.
    * **Costo**: **NO consume el turno**. Puedes usar el radar, ver dónde están los enemigos y luego disparar en el mismo turno.
    * **Requisito**: Portaviones activo. Si el portaviones es destruido, se perderá la conexión y se requerirá un turno extra para solicitar refuerzos aéreos externos.
    * **Enfriamiento**: **2 turnos**.

2.  **🛩️ Ataque Aéreo / Air Strike (Finaliza el Turno)**
    * **Efecto**: Bombardeo vertical masivo en una zona enemiga.
    * **Costo**: Al ser un ataque, **SÍ consume el turno**.
    * **Requisito**: Portaviones activo.
    * **Enfriamiento**: **5 turnos**.
    * **Alerta**: Deja rastro visual (fuego) en el turno del oponente.

#### 📝 Bitácora y Notas (Acción Libre)
* Accede con el botón **"NOTAS"**.
* Muestra un registro visual ("ecos" del radar y posiciones pasadas).
* **Uso**: Abrir y cerrar las notas **NO consume el turno**. Úsalo para planear tu estrategia con calma.

### 🏆 Condiciones de Victoria
* **Aniquilación Total**: Gana el jugador que logre hundir el 100% de la flota enemiga.

### 👥 Equipo

- **Líder**: Jesús David Padilla Castellanos (@JesusPadillaYEA-github)
- **Integrante 2**: Joseph Emmanuel Torres Acosta (@JosephDevCETI-github)

### 🛠️ Tecnologías

- Motor/Framework: SFML.
- Lenguaje: C++/CMake.
- Librerías adicionales: libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll, sfml-audio-3.dll, sfml-graphics-3.dll, sfml-system-3.dll, sfml-window-3.dll

### 📜 Créditos

- Inspirado en el juego de mesa "Battleship" (Hasbro Gaming)
- Tipografía: https://www.dafont.com/es/impacted.font
