# 3E252-Proyecto
proyecto de programación avanzada (24310401 y 24310402)

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

#### ⚔️ Combate Básico
* **Turnos Alternados**: El juego se desarrolla por turnos. Cada jugador puede realizar una acción de movimiento o ataque por turno.
* **Ataque Normal**: Disponible para todas las unidades. Selecciona un barco, presiona "ATACAR" y arrastra el mouse desde el barco para definir potencia y ángulo. El proyectil tiene un radio de explosión al impactar.
* **Movimiento**: Las unidades pueden moverse distancias cortas para esquivar ataques futuros, consumiendo el turno.

#### 🚢 Habilidades Especiales (Clase Portaviones)
El **Portaviones** es la unidad táctica central. Mientras esté operativo, permite el uso de soporte avanzado:

1.  **🛩️ Ataque Aéreo (Air Strike)**
    * **Efecto**: Lanza un caza que bombardea una columna vertical completa en el campo enemigo. Ideal para golpear sin apuntar con precisión.
    * **Requisito**: Portaviones activo.
    * **Enfriamiento (Cooldown)**: **5 turnos** de recarga.
    * **Alerta**: Deja una zona de fuego residual visible durante el turno del oponente.

2.  **📡 Radar (UAV)**
    * **Efecto**: Despliega un dron de reconocimiento que revela momentáneamente la posición de la flota enemiga y guarda "ecos" en la Bitácora.
    * **Requisito**: Portaviones activo. Si el portaviones es destruido, se perderá la conexión y se requerirá un turno extra para solicitar refuerzos aéreos externos.
    * **Enfriamiento (Cooldown)**: **2 turnos** de recarga.

#### 📝 Sistema de Notas y Bitácora
* Accede mediante el botón **"NOTAS"**.
* Despliega un mapa táctico estilo "papel" que muestra las últimas posiciones confirmadas por el Radar.
* **Uso Libre**: Consultar las notas no consume el turno, permitiendo planear la estrategia antes de actuar.

### 🏆 Condiciones de Victoria
* **Aniquilación Total**: La partida termina inmediatamente cuando todos los barcos de un jugador han sido destruidos.

### 👥 Equipo

- **Líder**: Jesús David Padilla Castellanos (@JesusPadillaYEA-github)
- **Integrante 2**: Joseph Emmanuel Torres Acosta (@JosephDevCETI-github)

### 🛠️ Tecnologías

- Motor/Framework: SFML.
- Lenguaje: C++/CMake.
- Librerías adicionales: libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll, sfml-audio-3.dll, sfml-...
