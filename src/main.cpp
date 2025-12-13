#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string> 
#include <SFML/Audio.hpp>

#include "EstadoJuego.hpp"
#include "Boton.hpp"
#include "CollisionManager.hpp"
#include "MovementManager.hpp"
#include "UIManager.hpp"
#include "Jugador.hpp"
#include "GeneradorFlotas.hpp"
#include "RenderManager.hpp"
#include "AttackManager.hpp"
#include "IndicatorManager.hpp"

int main() {
    const sf::Vector2u WINDOW_SIZE(1000, 700);
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Bitacora (SFML 3.0)");
    window.setFramerateLimit(60);


    // --- RECURSOS ---
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Ring.ttf")) return -1;

    sf::Texture tDest, tSub, tMar, tPort, tUAV;
    bool assetsOk = true;
    if (!tDest.loadFromFile("assets/images/destructor .png")) assetsOk = false;
    if (!tSub.loadFromFile("assets/images/submarino.png")) assetsOk = false;
    if (!tMar.loadFromFile("assets/images/mar.png")) assetsOk = false;
    if (!tPort.loadFromFile("assets/images/portaviones.png")) tPort = tDest; 
    
    bool uavLoaded = tUAV.loadFromFile("assets/images/UAV.png");
    
    // Recurso Avion de Ataque
    sf::Texture tAvionAtaque;
    bool avionLoaded = tAvionAtaque.loadFromFile("assets/images/avion1.png");
    sf::Sprite sAvionAtaque(tAvionAtaque);
    if (avionLoaded) {
        sf::FloatRect bounds = sAvionAtaque.getLocalBounds();
        sAvionAtaque.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    }

    if (!assetsOk) return -1;

    tMar.setRepeated(true);
    sf::Sprite fondo(tMar);
    float offset[2] = {0.f, 0.f};
    fondo.setTextureRect(sf::IntRect({0, 0}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

    // --- SPRITE DEL UAV ---
    sf::Sprite uavSprite(tUAV); 
    sf::CircleShape uavShapeFallback(5.f, 3);
    if (uavLoaded) {
        sf::FloatRect uavBounds = uavSprite.getLocalBounds();
        uavSprite.setOrigin({uavBounds.size.x / 2.f, uavBounds.size.y / 2.f});
    } else {
        uavShapeFallback.setFillColor(sf::Color::Cyan);
        uavShapeFallback.setOrigin({5.f, 5.f});
    }

    // ... (Después de cargar texturas y fuentes) ...

    // --- MUSICA DE FONDO ---
    sf::Music musicaFondo;
    // Intentar cargar el audio. Si falla, solo muestra error en consola pero el juego sigue.
    if (!musicaFondo.openFromFile("assets/music/main.ogg")) {
        std::cerr << "Error: No se pudo cargar assets/music/main.ogg" << std::endl;
    } else {
        musicaFondo.setLooping(true); // Repetir en bucle
        musicaFondo.setVolume(50.f);  // Volumen inicial al 50%
        musicaFondo.play();
    }
    
    // --- CONFIGURACIÓN DE AUDIO Y MENU ---
    bool juegoPausado = false;
    int opcionMenuSeleccionada = 0; // 0 = Musica, 1 = Efectos
    float volMusica = 50.f;
    float volEfectos = 50.f; // Preparado para futuros sonidos

    // Aplicar volumen inicial
    musicaFondo.setVolume(volMusica);

    // >>> NUEVO: CARGAR SONIDO RADAR (CORREGIDO PARA SFML 3) <<<
    sf::SoundBuffer bufferRadar;
    
    // 1. Intentamos cargar el archivo primero
    if (!bufferRadar.loadFromFile("assets/sounds/radar.ogg")) {
        std::cerr << "Error: No se pudo cargar assets/sounds/radar.ogg" << std::endl;
    } 
    
    // 2. Declaramos el Sound pasando el buffer como argumento (Obligatorio en SFML 3)
    sf::Sound sRadar(bufferRadar);
    
    // 3. Configuramos sus propiedades
    sRadar.setLooping(true); 
    sRadar.setVolume(volEfectos);
    // >>> FIN NUEVO <<<

    // Aplicar volumen inicial
    musicaFondo.setVolume(volMusica);

    // ... (Después de cargar músicaFondo) ...

    // >>> NUEVO: SONIDO UAV <<<
    sf::SoundBuffer bufferUAV;
    if (!bufferUAV.loadFromFile("assets/sounds/uav.ogg")) {
        std::cerr << "Error: No se pudo cargar assets/sounds/uav.ogg" << std::endl;
    }
    sf::Sound sUAV(bufferUAV);
    sUAV.setVolume(volEfectos);

    // >>> NUEVO: SONIDO DE INTERFAZ (BOTONES) <<<
    sf::SoundBuffer bufferButton;
    if (!bufferButton.loadFromFile("assets/sounds/button.ogg")) {
        std::cerr << "Error: No se pudo cargar assets/sounds/button.ogg" << std::endl;
    }
    sf::Sound sButton(bufferButton);
    sButton.setVolume(volEfectos);

    // >>> NUEVO: SONIDO AIR STRIKE <<<
    sf::SoundBuffer bufferAirStrike;
    if (!bufferAirStrike.loadFromFile("assets/sounds/airstrike.ogg")) {
        std::cerr << "Error: No se pudo cargar assets/sounds/airstrike.ogg" << std::endl;
    }
    sf::Sound sAirStrike(bufferAirStrike);
    sAirStrike.setVolume(volEfectos);

    sf::SoundBuffer bufferNotas;
    if (!bufferNotas.loadFromFile("assets/sounds/notas.ogg")) std::cerr << "Error loading notas.ogg" << std::endl;
    sf::Sound sNotas(bufferNotas);
    sNotas.setVolume(volEfectos);

    // 2. Sonido Error
    sf::SoundBuffer bufferError;
    if (!bufferError.loadFromFile("assets/sounds/err.ogg")) std::cerr << "Error loading err.ogg" << std::endl;
    sf::Sound sError(bufferError);
    sError.setVolume(volEfectos);

    // 3. Sonidos Disparo (Acierto/Fallo)
    sf::SoundBuffer bufferShotFail, bufferShotDone;
    if (!bufferShotFail.loadFromFile("assets/sounds/shot_fail.ogg")) std::cerr << "Error loading shot_fail.ogg" << std::endl;
    if (!bufferShotDone.loadFromFile("assets/sounds/shot_done.ogg")) std::cerr << "Error loading shot_done.ogg" << std::endl;
    
    sf::Sound sShotFail(bufferShotFail);
    sf::Sound sShotDone(bufferShotDone);
    sShotFail.setVolume(volEfectos);
    sShotDone.setVolume(volEfectos);

    // VARIABLE DE CONTROL PARA ESPERAR EL SONIDO DEL DISPARO
    int faseDisparoNormal = 0; // 0 = Inactivo, 1 = Sonando
    sf::Sound* sonidoDisparoActual = nullptr; // Puntero para saber cuál esperar

    sf::SoundBuffer bufferDestruccion;
    if (!bufferDestruccion.loadFromFile("assets/sounds/s_done.ogg")) {
        std::cerr << "Error: No se pudo cargar assets/sounds/s_done.ogg" << std::endl;
    }
    sf::Sound sDestruccion(bufferDestruccion);
    sDestruccion.setVolume(volEfectos);

    // >>> NUEVO: TEXTURAS DE ANIMACION DE EXPLOSION <<<
    std::vector<sf::Texture> animExplosionTex(6);
    bool animOk = true;
    for (int i = 0; i < 6; ++i) {
        if (!animExplosionTex[i].loadFromFile("assets/animations/explosion/" + std::to_string(i) + ".png")) {
            std::cerr << "Error cargando frame de explosion " << i << std::endl;
            animOk = false;
        }
    }

    // Estructura para gestionar animaciones activas
    struct ExplosionVisual {
        sf::Sprite sprite;
        float frameTime; 
        int currentFrame;

        // --- AGREGAR ESTE CONSTRUCTOR ---
        ExplosionVisual(const sf::Texture& texture) : sprite(texture) {
            frameTime = 0.f;
            currentFrame = 0;
        }
    };
    std::vector<ExplosionVisual> explosionesActivas;

    // --- UI DEL MENU ---
    // Fondo oscuro
    sf::RectangleShape fondoMenu({(float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y});
    fondoMenu.setFillColor(sf::Color(0, 0, 0, 200)); 

    // Caja contenedora
    sf::RectangleShape cajaMenu({500.f, 300.f});
    cajaMenu.setOrigin({250.f, 150.f});
    cajaMenu.setPosition({(float)WINDOW_SIZE.x / 2.f, (float)WINDOW_SIZE.y / 2.f});
    cajaMenu.setFillColor(sf::Color(40, 44, 52));
    cajaMenu.setOutlineThickness(3.f);
    cajaMenu.setOutlineColor(sf::Color(0, 255, 100)); // Verde tech

    // Título
    sf::Text txtMenuTitulo(font);
    txtMenuTitulo.setString("CONFIGURACION DE SISTEMAS");
    txtMenuTitulo.setCharacterSize(28);
    txtMenuTitulo.setFillColor(sf::Color::White);
    sf::FloatRect tb = txtMenuTitulo.getLocalBounds();
    txtMenuTitulo.setOrigin({tb.size.x/2.f, tb.size.y/2.f});
    txtMenuTitulo.setPosition({cajaMenu.getPosition().x, cajaMenu.getPosition().y - 110.f});

    // --- ELEMENTOS FILA 1: MUSICA ---
    sf::Text lblMusica(font, "MUSICA (BGM)", 20);
    lblMusica.setPosition({cajaMenu.getPosition().x - 200.f, cajaMenu.getPosition().y - 50.f});
    
    sf::Text btnMenosMusica(font, "<", 30); // Botón visual izquierdo
    btnMenosMusica.setPosition({cajaMenu.getPosition().x + 20.f, cajaMenu.getPosition().y - 60.f});
    
    sf::RectangleShape barraMusicaBg({100.f, 10.f});
    barraMusicaBg.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y - 40.f});
    barraMusicaBg.setFillColor(sf::Color(20, 20, 20));
    
    sf::RectangleShape barraMusicaFill({50.f, 10.f}); // Ancho dinámico
    barraMusicaFill.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y - 40.f});
    barraMusicaFill.setFillColor(sf::Color::Cyan);

    sf::Text btnMasMusica(font, ">", 30); // Botón visual derecho
    btnMasMusica.setPosition({cajaMenu.getPosition().x + 160.f, cajaMenu.getPosition().y - 60.f});

    sf::Text valMusicaTxt(font, "50%", 20);
    valMusicaTxt.setPosition({cajaMenu.getPosition().x + 190.f, cajaMenu.getPosition().y - 50.f});

    // --- ELEMENTOS FILA 2: EFECTOS ---
    sf::Text lblEfectos(font, "EFECTOS (SFX)", 20);
    lblEfectos.setPosition({cajaMenu.getPosition().x - 200.f, cajaMenu.getPosition().y + 20.f});

    sf::Text btnMenosEfectos(font, "<", 30);
    btnMenosEfectos.setPosition({cajaMenu.getPosition().x + 20.f, cajaMenu.getPosition().y + 10.f});

    sf::RectangleShape barraEfectosBg({100.f, 10.f});
    barraEfectosBg.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y + 30.f});
    barraEfectosBg.setFillColor(sf::Color(20, 20, 20));

    sf::RectangleShape barraEfectosFill({50.f, 10.f});
    barraEfectosFill.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y + 30.f});
    barraEfectosFill.setFillColor(sf::Color::Yellow);

    sf::Text btnMasEfectos(font, ">", 30);
    btnMasEfectos.setPosition({cajaMenu.getPosition().x + 160.f, cajaMenu.getPosition().y + 10.f});

    sf::Text valEfectosTxt(font, "50%", 20);
    valEfectosTxt.setPosition({cajaMenu.getPosition().x + 190.f, cajaMenu.getPosition().y + 20.f});

    // Instrucciones pie de pagina
    sf::Text txtInstruccionesVol(font, "ARRIBA/ABAJO: Seleccionar  |  IZQ/DER: Ajustar  |  ESC: Volver", 14);
    txtInstruccionesVol.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect ib = txtInstruccionesVol.getLocalBounds();
    txtInstruccionesVol.setOrigin({ib.size.x/2.f, ib.size.y/2.f});
    txtInstruccionesVol.setPosition({cajaMenu.getPosition().x, cajaMenu.getPosition().y + 110.f});
    // --- JUGADORES ---
    Jugador p1(1, {0.f, 0.f});
    Jugador p2(2, {700.f, 0.f});

    GeneradorFlotas::inicializarFlota(p1, tDest, tPort, tSub);
    GeneradorFlotas::inicializarFlota(p2, tDest, tPort, tSub);

    // --- ZONAS ---
    sf::FloatRect zonaIzquierda({0.f, 0.f}, {500.f, 700.f});
    sf::FloatRect zonaDerecha({500.f, 0.f}, {500.f, 700.f});

    // --- UI ---
    Boton btnAtacar(font, "ATACAR", {200, 50, 50});
    Boton btnMover(font, "MOVER", {50, 150, 50});
    Boton btnRadar(font, "RADAR", sf::Color(60, 70, 80)); 
    btnRadar.setPosition({850.f, 20.f}); 
    Boton btnNotas(font, "NOTAS", sf::Color(139, 100, 60)); 
    btnNotas.setPosition({730.f, 20.f}); 

    sf::Text txtInfo(font);
    txtInfo.setCharacterSize(20);
    txtInfo.setOutlineThickness(2.f);
    txtInfo.setFillColor(sf::Color::Yellow);
    txtInfo.setPosition({350.f, 20.f});

    // ==========================================
    // >>> RECURSOS ESTÉTICOS (RADAR Y NOTAS) <<<
    // ==========================================
    const sf::Vector2f radarCenter((float)WINDOW_SIZE.x / 2.f, (float)WINDOW_SIZE.y / 2.f);
    const sf::Color neonGreen(0, 255, 100);
    const sf::Color neonRed(255, 50, 50);

    sf::VertexArray radarBg(sf::PrimitiveType::TriangleStrip, 4);
    radarBg[0] = sf::Vertex{{0.f, 0.f}, sf::Color::Black};
    radarBg[1] = sf::Vertex{{(float)WINDOW_SIZE.x, 0.f}, sf::Color::Black};
    radarBg[2] = sf::Vertex{{0.f, (float)WINDOW_SIZE.y}, sf::Color(10, 25, 40)}; 
    radarBg[3] = sf::Vertex{{(float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y}, sf::Color(10, 25, 40)};

    std::vector<sf::CircleShape> radarCircles;
    for (int i = 1; i <= 4; ++i) {
        float r = i * 120.f; 
        sf::CircleShape c(r);
        c.setOrigin({r, r});
        c.setPosition(radarCenter);
        c.setFillColor(sf::Color::Transparent);
        c.setOutlineThickness(1.f);
        c.setOutlineColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 60));
        radarCircles.push_back(c);
    }
    
    sf::RectangleShape axisX({(float)WINDOW_SIZE.x, 1.f});
    axisX.setPosition({0.f, radarCenter.y});
    axisX.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));
    
    sf::RectangleShape axisY({1.f, (float)WINDOW_SIZE.y});
    axisY.setPosition({radarCenter.x, 0.f});
    axisY.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));

    float radarSweepAngle = 0.f;
    float largoLinea = radarCircles.back().getRadius() + 100.f;
    sf::RectangleShape sweepLine({largoLinea, 2.f});
    sweepLine.setOrigin({0.f, 1.f}); 
    sweepLine.setPosition(radarCenter);
    sweepLine.setFillColor(neonGreen);
    
    sf::RectangleShape sweepGlow({largoLinea, 15.f});
    sweepGlow.setOrigin({0.f, 7.5f});
    sweepGlow.setPosition(radarCenter);
    sweepGlow.setFillColor(sf::Color(neonGreen.r, neonGreen.g, neonGreen.b, 30));

    // Fondo NOTAS
    sf::Color papelClaro(245, 235, 200); 
    sf::Color papelOscuro(100, 70, 40);  

    sf::VertexArray fondoNotas(sf::PrimitiveType::TriangleFan, 6);
    fondoNotas[0] = sf::Vertex{radarCenter, papelClaro};
    fondoNotas[1] = sf::Vertex{{0.f, 0.f}, papelOscuro};
    fondoNotas[2] = sf::Vertex{{(float)WINDOW_SIZE.x, 0.f}, papelOscuro};
    fondoNotas[3] = sf::Vertex{{(float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y}, papelOscuro};
    fondoNotas[4] = sf::Vertex{{0.f, (float)WINDOW_SIZE.y}, papelOscuro};
    fondoNotas[5] = sf::Vertex{{0.f, 0.f}, papelOscuro}; 

    std::vector<sf::RectangleShape> lineasCuaderno;
    for(float y = 80.f; y < WINDOW_SIZE.y; y += 40.f) {
        sf::RectangleShape linea({(float)WINDOW_SIZE.x, 1.f});
        linea.setPosition({0.f, y});
        linea.setFillColor(sf::Color(0, 0, 0, 40)); 
        lineasCuaderno.push_back(linea);
    }

    sf::RectangleShape flechaPalo({25.f, 6.f});
    flechaPalo.setFillColor(sf::Color::Black);
    flechaPalo.setOrigin({0.f, 3.f});
    sf::CircleShape flechaPunta(10.f, 3); 
    flechaPunta.setFillColor(sf::Color::Black);
    flechaPunta.setOrigin({10.f, 10.f});
    flechaPunta.setRotation(sf::degrees(-90)); 
    // ==========================================

    EstadoJuego estado = MENSAJE_P1;
    int sel = -1;
    bool moviendo = false;
    bool apuntando = false;
    
    bool modoRadar = false;
    bool modoNotas = false; 
    bool lanzandoUAV = false;
    bool lanzandoUAVRefuerzo = false; 
    float errorTimer = 0.f;
    sf::String msgError = "";
    
    sf::Clock uavTimer;

    bool cargandoDisparo = false;
    float potenciaActual = 0.f;
    const float VELOCIDAD_CARGA = 5.0f;
    const float MAX_DISTANCIA = 1500.f;

    bool enZonaEnemiga = false;
    sf::Vector2f origenDisparoReal(0,0); 

    // --- VARIABLES AIR STRIKE ---
    // 0 = Inactivo, 1 = Espera en cubierta, 2 = Volando
    int faseAtaqueAereo = 0; 
    sf::Clock timerAtaqueAereo;
    sf::Vector2f posAvionAtaque;
    float colObjetivoX = 0.f;
    float velocidadAvion = 4.0f; // Más lento

    sf::Clock dtClock;

    while (window.isOpen()) {
        float deltaTime = dtClock.restart().asSeconds();
        if (errorTimer > 0.f) errorTimer -= deltaTime;

        Jugador* jugadorActual = nullptr;
        Jugador* jugadorEnemigo = nullptr;
        sf::FloatRect* zonaObjetivo = nullptr;

        if (estado == TURNO_P1) {
            jugadorActual = &p1; jugadorEnemigo = &p2;
        } else if (estado == TURNO_P2) {
            jugadorActual = &p2; jugadorEnemigo = &p1;
        }

        if (sel != -1 && jugadorActual) {
            float barcoX = jugadorActual->getFlota()[sel].sprite.getPosition().x;
            if (barcoX < 500.f) zonaObjetivo = &zonaDerecha;
            else zonaObjetivo = &zonaIzquierda;
        } else {
            zonaObjetivo = (estado == TURNO_P1) ? &zonaDerecha : &zonaIzquierda;
        }

        while (const auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();
            // --- CONTROL DEL MENU ---
            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::Escape) {
                    // Si hay una acción en juego, cancelarla primero
                    if (cargandoDisparo || apuntando || moviendo || sel != -1 || modoRadar || modoNotas || faseAtaqueAereo > 0) {
                        cargandoDisparo = false; apuntando = false; moviendo = false; sel = -1; 
                        modoRadar = false; modoNotas = false; faseAtaqueAereo = 0;
                        lanzandoUAV = false; lanzandoUAVRefuerzo = false;
                        btnAtacar.resetColor(); btnMover.resetColor(); btnRadar.resetColor(); btnNotas.resetColor();
                        sRadar.stop(); 
                        sUAV.stop();
                        sAirStrike.stop();
                        sShotFail.stop(); sShotDone.stop(); sNotas.stop(); sError.stop();
                    } 
                    else {
                        // Alternar pausa
                        juegoPausado = !juegoPausado;
                    }
                }

                // CONTROLES DENTRO DEL MENU
                if (juegoPausado) {
                    // Navegar entre opciones (Musica vs Efectos)
                    if (k->code == sf::Keyboard::Key::Up || k->code == sf::Keyboard::Key::Down) {
                        sButton.play();
                        opcionMenuSeleccionada = (opcionMenuSeleccionada == 0) ? 1 : 0;
                    }

                    // Ajustar valores
                    if (k->code == sf::Keyboard::Key::Left) {
                        sButton.play();
                        if (opcionMenuSeleccionada == 0) { // Musica
                            volMusica -= 5.f;
                            if (volMusica < 0.f) volMusica = 0.f;
                            musicaFondo.setVolume(volMusica);
                        } else { // Efectos
                            volEfectos -= 5.f;
                            if (volEfectos < 0.f) volEfectos = 0.f;
                            // Aquí actualizarías el volumen de tus efectos futuros
                            sRadar.setVolume(volEfectos);
                            sUAV.setVolume(volEfectos);
                            sButton.setVolume(volEfectos);
                            sAirStrike.setVolume(volEfectos);

                            sNotas.setVolume(volEfectos);
                            sError.setVolume(volEfectos);
                            sShotFail.setVolume(volEfectos);
                            sShotDone.setVolume(volEfectos);
                            sDestruccion.setVolume(volEfectos);
                        }
                    }
                    else if (k->code == sf::Keyboard::Key::Right) {
                        sButton.play();
                        if (opcionMenuSeleccionada == 0) { // Musica
                            volMusica += 5.f;
                            if (volMusica > 100.f) volMusica = 100.f;
                            musicaFondo.setVolume(volMusica);
                        } else { // Efectos
                            volEfectos += 5.f;
                            if (volEfectos > 100.f) volEfectos = 100.f;
                            sRadar.setVolume(volEfectos);
                            sUAV.setVolume(volEfectos);
                            sButton.setVolume(volEfectos);
                            sAirStrike.setVolume(volEfectos);

                            sNotas.setVolume(volEfectos);
                            sError.setVolume(volEfectos);
                            sShotFail.setVolume(volEfectos);
                            sShotDone.setVolume(volEfectos);
                            sDestruccion.setVolume(volEfectos);
                        }
                    }
                }
            }

            // SI EL JUEGO ESTA PAUSADO, IGNORAR CLICKS
            if (juegoPausado) continue;

            // SI EL JUEGO ESTA PAUSADO, IGNORAR RESTO DE INPUTS (CLICKS)
            if (juegoPausado) continue;

            if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
                if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) {
                    EstadoJuego siguienteEstado = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
                    Jugador* siguienteJugador = (siguienteEstado == TURNO_P1) ? &p1 : &p2;

                    // Reducir cooldowns
                    if (siguienteJugador->cooldownRadar > 0) siguienteJugador->cooldownRadar--;
                    if (siguienteJugador->cooldownAtaqueAereo > 0) siguienteJugador->cooldownAtaqueAereo--;
                    
                    // Limpiar la franja roja del jugador anterior (ya pasó su turno de verla)
                    Jugador* jugadorAnterior = (siguienteEstado == TURNO_P1) ? &p2 : &p1;
                    jugadorAnterior->columnaFuegoX = -1.f; 

                    if (siguienteJugador->radarRefuerzoPendiente) {
                        lanzandoUAVRefuerzo = true;
                        sUAV.play();
                        siguienteJugador->radarRefuerzoPendiente = false; 
                        siguienteJugador->cooldownRadar = 2; 
                        
                        sf::Vector2f posInicio = { (float)WINDOW_SIZE.x / 2.f, (float)WINDOW_SIZE.y + 100.f };
                        if (uavLoaded) {
                            uavSprite.setPosition(posInicio);
                            uavSprite.setRotation(sf::degrees(0));
                            uavSprite.setScale({0.15f, 0.15f});
                        } else {
                            uavShapeFallback.setPosition(posInicio);
                        }
                    }

                    estado = siguienteEstado;
                    sel = -1; moviendo = false; apuntando = false; cargandoDisparo = false;
                    modoRadar = false; modoNotas = false; lanzandoUAV = false; 
                    errorTimer = 0.f;
                    btnMover.resetColor(); btnAtacar.resetColor(); btnRadar.resetColor(); btnNotas.resetColor();
                }
                continue;
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            // Bloquear input durante animaciones
            if (lanzandoUAV || lanzandoUAVRefuerzo || faseAtaqueAereo > 0 || faseDisparoNormal > 0) continue;

            if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    
                    if (!apuntando && !cargandoDisparo && jugadorActual) {
                        
                        // --- BOTON RADAR ---
                        if (btnRadar.esClickeado(mousePos) && !modoNotas) {
                            sButton.play();
                            if (!modoRadar) {
                                if (jugadorActual->radarRefuerzoPendiente) {
                                    msgError = "SOLICITUD EN PROCESO. REFUERZOS LLEGAN SIGUIENTE TURNO.";
                                    errorTimer = 2.0f;
                                }
                                else if (jugadorActual->cooldownRadar > 0) {
                                    msgError = "SISTEMA ENFRIANDOSE. ESPERA " + std::to_string(jugadorActual->cooldownRadar) + " TURNO(S).";
                                    sError.play();
                                    errorTimer = 2.0f;
                                }
                                else {
                                    bool carrierFound = false;
                                    sf::Vector2f posInicio;
                                    for (const auto& barco : jugadorActual->getFlota()) {
                                        if (barco.nombre.find("Portaviones") != std::string::npos && !barco.destruido) {
                                            sf::FloatRect b = barco.sprite.getGlobalBounds();
                                            posInicio = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};
                                            carrierFound = true;
                                            break;
                                        }
                                    }
                                    
                                    if (carrierFound) {
                                        if (uavLoaded) {
                                            uavSprite.setPosition(posInicio);
                                            uavSprite.setRotation(sf::degrees(0));
                                            uavSprite.setScale({0.05f, 0.05f}); 
                                        } else {
                                            uavShapeFallback.setPosition(posInicio);
                                        }
                                        lanzandoUAV = true;
                                        sUAV.play();
                                        uavTimer.restart();
                                        jugadorActual->cooldownRadar = 2; 
                                        
                                        btnRadar.setColor(sf::Color::Red);
                                        sel = -1; moviendo = false; 
                                    } 
                                    else {
                                        jugadorActual->radarRefuerzoPendiente = true;
                                        msgError = "PORTAVIONES CAIDO. REFUERZOS LLEGAN EL PROXIMO TURNO.";
                                        errorTimer = 3.0f;
                                    }
                                }
                            } 
                            else {
                                modoRadar = false;
                                btnRadar.resetColor();
                                sRadar.stop();
                            }
                        }

                        // --- BOTON NOTAS ---
                        else if (btnNotas.esClickeado(mousePos) && !modoRadar) {
                            sButton.play();
                            sNotas.play();
                            modoNotas = !modoNotas;
                            if (modoNotas) {
                                btnNotas.setColor(sf::Color::White); 
                                sel = -1; moviendo = false; 
                            } else {
                                btnNotas.resetColor();
                            }
                        }
                    }

                    if (modoRadar || modoNotas) continue; 

                    // JUEGO NORMAL
                    if (apuntando && sel != -1) {
                        cargandoDisparo = true;
                        potenciaActual = 0.f;
                    }
                    
                    if (!apuntando && !cargandoDisparo) {
                        if (sel != -1) {
                            // CLICK EN BOTONES DEL BARCO
                            if (btnAtacar.esClickeado(mousePos)) {
                                sButton.play();
                                bool esPortaviones = (jugadorActual->getFlota()[sel].nombre.find("Portaviones") != std::string::npos);
                                
                                if (esPortaviones) {
                                    // *** LOGICA AIR STRIKE ***
                                    if (jugadorActual->cooldownAtaqueAereo > 0) {
                                        msgError = "ESCUADRON REPOSTANDO. ESPERA " + std::to_string(jugadorActual->cooldownAtaqueAereo) + " TURNO(S).";
                                        sError.play();
                                        errorTimer = 2.0f;
                                    } else {
                                        // INICIAR ANIMACION (Sin flecha, directo)
                                        faseAtaqueAereo = 1; // Fase 1: Espera
                                        timerAtaqueAereo.restart();
                                        sAirStrike.play();
                                        
                                        auto bounds = jugadorActual->getFlota()[sel].sprite.getGlobalBounds();
                                        sf::Vector2f centro = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
                                        posAvionAtaque = centro;
                                        colObjetivoX = centro.x; // Columna fija
                                        
                                        // Configurar sprite
                                        if (avionLoaded) {
                                            sAvionAtaque.setPosition(posAvionAtaque);
                                            sAvionAtaque.setScale({0.05f, 0.05f});
                                            // Orientar hacia arriba o abajo según jugador
                                            if (jugadorActual->id == 1) sAvionAtaque.setRotation(sf::degrees(0)); // P1 dispara derecha? No, mapa es lado a lado.
                                            // Asumimos que disparan hacia el lado enemigo.
                                            // P1 (izq) dispara derecha, P2 (der) dispara izquierda.
                                            // Pero el código de disparo dice "arriba". Vamos a usar rotación estándar.
                                            sAvionAtaque.setRotation(sf::degrees(0)); 
                                        }
                                        
                                        jugadorActual->cooldownAtaqueAereo = 5; // COOLDOWN 5 TURNOS
                                        btnAtacar.setColor(sf::Color::Red);
                                        sel = -1; // Deseleccionar
                                    }
                                } 
                                else {
                                    // *** LOGICA NORMAL ***
                                    apuntando = true; moviendo = false;
                                    btnAtacar.setColor({255, 69, 0}); btnMover.resetColor();
                                }
                            } 
                            else if (btnMover.esClickeado(mousePos)) {
                                sButton.play();
                                if (moviendo) {
                                    IndicatorManager::actualizarTurnos(*jugadorActual);
                                    moviendo = false; sel = -1; btnMover.resetColor();
                                    estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                                } else {
                                    moviendo = true; btnMover.setColor({255, 140, 0});
                                }
                            }
                        }

                        // SELECCION DE BARCO
                        if (!btnAtacar.esClickeado(mousePos) && !btnMover.esClickeado(mousePos) && 
                            !btnRadar.esClickeado(mousePos) && !btnNotas.esClickeado(mousePos) && jugadorActual) {
                            bool hit = false;
                            auto& flota = jugadorActual->getFlota();
                            for (size_t i = 0; i < flota.size(); ++i) {
                                if (flota[i].destruido) continue;
                                if (flota[i].sprite.getGlobalBounds().contains(mousePos)) {
                                    sel = (int)i; hit = true; 
                                    moviendo = false; apuntando = false; 
                                    btnMover.resetColor(); btnAtacar.resetColor();
                                    break;
                                }
                            }
                            if (!hit && !moviendo) sel = -1;
                        }
                    }
                }
            }

            if (const auto* m = ev->getIf<sf::Event::MouseButtonReleased>()) {
                if (m->button == sf::Mouse::Button::Left) {
                    if (cargandoDisparo && apuntando && sel != -1) {
                        cargandoDisparo = false;
                        sf::Vector2f diff = mousePos - origenDisparoReal;
                        float angulo = std::atan2(diff.y, diff.x);
                        sf::Vector2f impacto;
                        impacto.x = origenDisparoReal.x + std::cos(angulo) * potenciaActual;
                        impacto.y = origenDisparoReal.y + std::sin(angulo) * potenciaActual;

                    if (jugadorEnemigo) {
                            // Vector para capturar coordenadas de barcos destruidos
                            std::vector<sf::Vector2f> bajas;

                            // Pasamos 'bajas' a la función
                            bool acerto = AttackManager::procesarDisparo(impacto, origenDisparoReal, *jugadorEnemigo, bajas);
                            
                            // GESTION DE BAJAS (ANIMACION Y SONIDO)
                            if (!bajas.empty()) {
                                    sDestruccion.play(); 
                                    for (const auto& pos : bajas) {
                                        // 1. CREAR CON TEXTURA DIRECTAMENTE
                                        ExplosionVisual ex(animExplosionTex[0]);
                                        
                                        // 2. CONFIGURAR
                                        sf::FloatRect bounds = ex.sprite.getLocalBounds();
                                        ex.sprite.setOrigin({bounds.size.x/2.f, bounds.size.y/2.f});
                                        ex.sprite.setPosition(pos);
                                        ex.sprite.setScale({2.0f, 2.0f}); 
                                        
                                        // 3. GUARDAR
                                        explosionesActivas.push_back(ex);
                                    }
                                }
                            
                            if (acerto) {
                                if (bajas.empty()) sShotDone.play(); // Solo suena impacto si no hubo destrucción total (para no solapar)
                                sonidoDisparoActual = bajas.empty() ? &sShotDone : &sDestruccion;
                            } else {
                                sShotFail.play();
                                sonidoDisparoActual = &sShotFail;
                            }
                            
                            faseDisparoNormal = 1; 
                        }
                        IndicatorManager::actualizarTurnos(*jugadorActual);
                        apuntando = false; sel = -1;
                        btnAtacar.resetColor();
                        
                    }
                }
            }

            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::Escape) {
                    cargandoDisparo = false; apuntando = false; moviendo = false; sel = -1; 
                    modoRadar = false; modoNotas = false;
                    lanzandoUAV = false; lanzandoUAVRefuerzo = false;
                    faseAtaqueAereo = 0; // Cancelar si es posible
                    sRadar.stop();
                    btnAtacar.resetColor(); btnMover.resetColor(); btnRadar.resetColor(); btnNotas.resetColor();
                }
            }
        }

        // --- UPDATE ---
        
        // ANIMACION UAV (RADAR)
        if (lanzandoUAV) {
            if (uavTimer.getElapsedTime().asMilliseconds() > 500) {
                if (uavLoaded) {
                    uavSprite.move({0.f, -3.0f}); 
                    sf::Vector2f scale = uavSprite.getScale();
                    if (scale.x < 0.15f) uavSprite.setScale({scale.x + 0.0005f, scale.y + 0.0005f});
                    if (uavSprite.getPosition().y < -50.f) {
                        lanzandoUAV = false; modoRadar = true; btnRadar.setColor(sf::Color::Red);
                        sRadar.play();
                        sUAV.stop();
                    }
                } else {
                    uavShapeFallback.move({0.f, -3.0f});
                    if (uavShapeFallback.getPosition().y < -50.f) {
                        lanzandoUAV = false; modoRadar = true; btnRadar.setColor(sf::Color::Red);
                    }
                }
            }
        }
        else if (lanzandoUAVRefuerzo) {
            if (uavLoaded) {
                uavSprite.move({0.f, -5.0f}); 
                if (uavSprite.getPosition().y < -50.f) {
                    lanzandoUAVRefuerzo = false; 
                    modoRadar = true;
                    btnRadar.setColor(sf::Color::Red);
                    sRadar.play();
                    sUAV.stop();
                    
                }
            } else {
                uavShapeFallback.move({0.f, -5.0f});
                if (uavShapeFallback.getPosition().y < -50.f) {
                    lanzandoUAVRefuerzo = false; modoRadar = true; btnRadar.setColor(sf::Color::Red);
                    sRadar.play();
                    sUAV.stop();
                }
            }
        }
        // ANIMACION AIR STRIKE
        else if (faseAtaqueAereo > 0) {
            if (faseAtaqueAereo == 1) {
                // Fase 1: Mantenerse en cubierta un momento
                if (timerAtaqueAereo.getElapsedTime().asSeconds() > 1.0f) {
                    faseAtaqueAereo = 2; // Pasar a volar
                } else {
                    // Pequeña vibración o escala
                    float scale = 0.05f + (std::sin(timerAtaqueAereo.getElapsedTime().asMilliseconds() * 0.01f) * 0.002f);
                    sAvionAtaque.setScale({scale, scale});
                }
            }
            else if (faseAtaqueAereo == 2) {
                // Fase 2: Vuelo Lento Vertical (hacia arriba, simulando salida)
                // Ojo: En este juego la vista es lateral/arriba. Vamos a moverlo en Y negativo (arriba pantalla).
                sAvionAtaque.move({0.f, -velocidadAvion});
                
                // Crecer efecto despegue
                sf::Vector2f scale = sAvionAtaque.getScale();
                if (scale.x < 0.25f) sAvionAtaque.setScale({scale.x + 0.001f, scale.y + 0.001f});

                bool avionFuera = (sAvionAtaque.getPosition().y < -100.f);
                bool sonidoTerminado = (sAirStrike.getStatus() == sf::Sound::Status::Stopped);

                if (avionFuera && sonidoTerminado) {
                    faseAtaqueAereo = 0;
                    if (jugadorEnemigo) {
                        std::vector<sf::Vector2f> bajasAereas;
                        
                        // Pasamos 'bajasAereas' a la función
                        AttackManager::procesarAtaqueAereo(colObjetivoX, *jugadorEnemigo, bajasAereas);

                        // Si hubo bajas por airstrike
                        if (!bajasAereas.empty()) {
                            sDestruccion.play();
                            for (const auto& pos : bajasAereas) {
                                // 1. CREAR CON TEXTURA DIRECTAMENTE
                                ExplosionVisual ex(animExplosionTex[0]);

                                // 2. CONFIGURAR
                                sf::FloatRect bounds = ex.sprite.getLocalBounds();
                                ex.sprite.setOrigin({bounds.size.x/2.f, bounds.size.y/2.f});
                                ex.sprite.setPosition(pos);
                                ex.sprite.setScale({2.0f, 2.0f});
                                
                                // 3. GUARDAR
                                explosionesActivas.push_back(ex);
                            }
                        }
                    }
                    IndicatorManager::actualizarTurnos(*jugadorActual);
                    estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
                }
            }
        }else if (faseDisparoNormal == 1) {
            // Verificar si el sonido terminó
            if (sonidoDisparoActual && sonidoDisparoActual->getStatus() == sf::Sound::Status::Stopped) {
                faseDisparoNormal = 0; // Terminar fase
                sonidoDisparoActual = nullptr;

                // AHORA SI, CAMBIAR DE TURNO
                IndicatorManager::actualizarTurnos(*jugadorActual);
                estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
            }
        }
        else if (!modoRadar && !modoNotas) {
            offset[0] += 0.5f; offset[1] += 0.25f;
            if (offset[0] >= (float)WINDOW_SIZE.x) offset[0] = 0; 
            if (offset[1] >= (float)WINDOW_SIZE.y) offset[1] = 0;
            fondo.setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));
        } 
        else if (modoRadar) {
            radarSweepAngle += 2.0f;
            if (radarSweepAngle >= 360.f) radarSweepAngle -= 360.f;
            sweepLine.setRotation(sf::degrees(radarSweepAngle));
            sweepGlow.setRotation(sf::degrees(radarSweepAngle));
        }

        if (cargandoDisparo) {
            potenciaActual += VELOCIDAD_CARGA;
            if (potenciaActual > MAX_DISTANCIA) potenciaActual = MAX_DISTANCIA;
        }

        if (animOk) {
            for (size_t i = 0; i < explosionesActivas.size(); ) {
                explosionesActivas[i].frameTime += deltaTime;
                
                // Cambiar frame cada 0.1 segundos
                if (explosionesActivas[i].frameTime > 0.1f) {
                    explosionesActivas[i].frameTime = 0.f;
                    explosionesActivas[i].currentFrame++;
                    
                    if (explosionesActivas[i].currentFrame >= 6) {
                        // Animación terminada, eliminar
                        explosionesActivas.erase(explosionesActivas.begin() + i);
                        continue; 
                    } else {
                        // Actualizar textura
                        explosionesActivas[i].sprite.setTexture(animExplosionTex[explosionesActivas[i].currentFrame]);
                    }
                }
                ++i;
            }
        }

        if (moviendo && sel != -1 && jugadorActual) {
            auto& barco = jugadorActual->getFlota()[sel];
            if (!barco.destruido) MovementManager::procesarInput(jugadorActual->getFlota(), sel, WINDOW_SIZE);
        }

        if (sel != -1 && jugadorActual && !jugadorActual->getFlota()[sel].destruido) {
             auto& barco = jugadorActual->getFlota()[sel];
             auto bounds = barco.sprite.getGlobalBounds();
             sf::Vector2f p = barco.sprite.getPosition();
             float yBtn = p.y + bounds.size.y + 10.f;
             btnAtacar.setPosition({p.x, yBtn});
             btnMover.setPosition({p.x + 110.f, yBtn});
        }

        sf::Vector2f impactoVisual(0,0);
        if (apuntando && sel != -1 && jugadorActual) {
            auto bounds = jugadorActual->getFlota()[sel].sprite.getGlobalBounds();
            sf::Vector2f centroBarco = {bounds.position.x + bounds.size.x/2.f, bounds.position.y + bounds.size.y/2.f};
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            
            if (zonaObjetivo && zonaObjetivo->contains(mousePos)) {
                enZonaEnemiga = true;
                origenDisparoReal = {centroBarco.x, (float)WINDOW_SIZE.y};
            } else {
                enZonaEnemiga = false;
                origenDisparoReal = centroBarco;
            }

            float pot = cargandoDisparo ? potenciaActual : 50.f;
            sf::Vector2f diff = mousePos - origenDisparoReal;
            float ang = std::atan2(diff.y, diff.x);
            impactoVisual = origenDisparoReal + sf::Vector2f(std::cos(ang)*pot, std::sin(ang)*pot);
        }

        // --- DRAW ---
        window.clear();

        if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
            window.draw(fondo); 
            UIManager::dibujarTooltipTurno(window, font, estado);
        }
        else if (jugadorActual) {
            
            // >>> MODO RADAR <<<
            if (modoRadar && jugadorEnemigo) {
                // (Código radar igual al anterior...)
                jugadorActual->memoriaRadar.clear(); 
                for (const auto& barco : jugadorEnemigo->getFlota()) {
                    if (!barco.destruido) {
                        sf::FloatRect b = barco.sprite.getGlobalBounds();
                        sf::Vector2f blipPos = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};
                        jugadorActual->memoriaRadar.push_back(blipPos);
                    }
                }
                window.draw(radarBg);
                window.draw(axisX); window.draw(axisY);
                for (const auto& circle : radarCircles) window.draw(circle);
                for (const auto& pos : jugadorActual->memoriaRadar) {
                    sf::CircleShape haloOut(15.f); haloOut.setOrigin({15.f, 15.f}); haloOut.setPosition(pos);
                    haloOut.setFillColor(sf::Color(neonRed.r, neonRed.g, neonRed.b, 50));
                    window.draw(haloOut);
                    sf::CircleShape core(5.f); core.setOrigin({5.f, 5.f}); core.setPosition(pos);
                    core.setFillColor(neonRed);
                    window.draw(core);
                }
                window.draw(sweepGlow);
                window.draw(sweepLine);
                txtInfo.setString("ESCANEANDO... GUARDANDO EN BITACORA");
                txtInfo.setFillColor(neonGreen);
                txtInfo.setPosition({WINDOW_SIZE.x / 2.f - 200.f, 50.f}); 
                window.draw(txtInfo);
                btnRadar.dibujar(window);
            }
            // >>> MODO NOTAS <<<
            else if (modoNotas) {
                // (Código notas igual al anterior...)
                window.draw(fondoNotas); 
                for(const auto& linea : lineasCuaderno) window.draw(linea);
                for (const auto& barco : jugadorActual->getFlota()) {
                    if (!barco.destruido) {
                        sf::FloatRect b = barco.sprite.getGlobalBounds();
                        sf::RectangleShape marco({b.size.x + 10.f, b.size.y + 10.f});
                        marco.setOrigin({5.f, 5.f}); 
                        marco.setPosition({b.position.x, b.position.y});
                        marco.setFillColor(sf::Color::Transparent);
                        marco.setOutlineThickness(2.f);
                        marco.setOutlineColor(sf::Color(0, 100, 0, 200)); 
                        window.draw(marco);
                    }
                }
                for (const auto& pos : jugadorActual->memoriaRadar) {
                    sf::CircleShape mancha1(20.f); mancha1.setOrigin({20.f, 20.f}); mancha1.setPosition(pos);
                    mancha1.setFillColor(sf::Color(180, 0, 0, 30)); window.draw(mancha1);
                    sf::CircleShape nucleo(7.f); nucleo.setOrigin({7.f, 7.f}); nucleo.setPosition(pos);
                    nucleo.setFillColor(sf::Color(160, 0, 0, 255)); window.draw(nucleo);
                }
                if (jugadorEnemigo) {
                    for (const auto& barco : jugadorEnemigo->getFlota()) {
                        if (barco.destruido) {
                            sf::FloatRect b = barco.sprite.getGlobalBounds();
                            sf::Vector2f centro = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};
                            sf::RectangleShape linea1({60.f, 8.f}); linea1.setOrigin({30.f, 4.f}); linea1.setPosition(centro);
                            linea1.setFillColor(sf::Color(139, 0, 0, 220)); linea1.setRotation(sf::degrees(45));
                            sf::RectangleShape linea2({60.f, 8.f}); linea2.setOrigin({30.f, 4.f}); linea2.setPosition(centro);
                            linea2.setFillColor(sf::Color(139, 0, 0, 220)); linea2.setRotation(sf::degrees(-45));
                            window.draw(linea1); window.draw(linea2);
                        }
                    }
                }
                txtInfo.setString("BITACORA DE CAPITAN - REPORTE TACTICO");
                txtInfo.setFillColor(sf::Color(60, 40, 20)); txtInfo.setPosition({300.f, 30.f});
                window.draw(txtInfo);
                btnNotas.dibujar(window);
                sf::Rect<float> btnBounds = btnNotas.getBounds();
                sf::Vector2f btnCenter = {btnBounds.position.x + btnBounds.size.x / 2.f, btnBounds.position.y + btnBounds.size.y / 2.f};
                flechaPalo.setPosition({btnCenter.x + 10.f, btnCenter.y});
                flechaPunta.setPosition({btnCenter.x - 15.f, btnCenter.y});
                window.draw(flechaPalo); window.draw(flechaPunta);
            }
            // >>> MODO NORMAL <<<
            else {
                window.draw(fondo); 

                // 1. DIBUJAR FRANJA ROJA (Si recibí ataque aéreo en el turno anterior)
                if (jugadorActual->columnaFuegoX > 0.f) {
                    sf::RectangleShape franja({60.f, (float)WINDOW_SIZE.y});
                    franja.setOrigin({30.f, 0.f});
                    franja.setPosition({jugadorActual->columnaFuegoX, 0.f});
                    franja.setFillColor(sf::Color(255, 0, 0, 100)); // Rojo semitransparente
                    window.draw(franja);

                    txtInfo.setString("! ALERTA ! ZONA BOMBARDEADA");
                    txtInfo.setFillColor(sf::Color::Red);
                    txtInfo.setPosition({jugadorActual->columnaFuegoX + 40.f, 100.f});
                    window.draw(txtInfo);
                }

                if (enZonaEnemiga && apuntando) {
                    sf::RectangleShape overlay(sf::Vector2f((float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y));
                    overlay.setFillColor(sf::Color(255, 0, 0, 40)); 
                    window.draw(overlay);
                    txtInfo.setString("! ZONA ENEMIGA ! SUELTA PARA DISPARAR");
                    txtInfo.setFillColor(sf::Color::Red);
                    float textoY = impactoVisual.y - 40.f;
                    if (textoY < 10) textoY = impactoVisual.y + 20.f;
                    txtInfo.setPosition({impactoVisual.x + 20.f, textoY});
                    window.draw(txtInfo);
                } else {
                    sf::Color cBorde = moviendo ? sf::Color(255, 140, 0) : sf::Color(50, 150, 50);
                    if (apuntando) cBorde = sf::Color::Red;
                    
                    RenderManager::renderizarFlota(window, jugadorActual->getFlota(), sel, estado, moviendo, cBorde);
                    //IndicatorManager::renderizarPistas(window, *jugadorActual, false);
                    
                    // ========================================================
                    // >>> PEGAR AQUÍ EL DIBUJADO DE EXPLOSIONES <<<
                    // ========================================================
                    for (const auto& ex : explosionesActivas) {
                        window.draw(ex.sprite);
                    }
                    // ========================================================

                    if (apuntando) {
                        txtInfo.setString("Arrastra hacia territorio enemigo...");
                        txtInfo.setFillColor(sf::Color::Yellow);
                        txtInfo.setPosition({300.f, 20.f});
                        window.draw(txtInfo);
                    }

                    // Botones
                    if (sel != -1 && !jugadorActual->getFlota()[sel].destruido && !cargandoDisparo && !lanzandoUAV && !lanzandoUAVRefuerzo && faseAtaqueAereo == 0) {
                        btnAtacar.dibujar(window);
                        btnMover.dibujar(window);
                    }
                }

                if (apuntando && sel != -1) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    float pot = cargandoDisparo ? potenciaActual : 50.f;
                    IndicatorManager::dibujarVectorApuntado(window, origenDisparoReal, mousePos, pot, font);
                }

                // Render UAV y Animaciones
                if (lanzandoUAV) { /* ... (igual al anterior) ... */ 
                    if (uavTimer.getElapsedTime().asMilliseconds() <= 500) {
                        txtInfo.setString("PREPARANDO DESPEGUE..."); txtInfo.setFillColor(sf::Color::White);
                        sf::Vector2f pos = uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                        txtInfo.setPosition({pos.x - 80.f, pos.y - 50.f}); window.draw(txtInfo);
                    } else {
                        txtInfo.setString("UAV EN RUTA..."); txtInfo.setFillColor(sf::Color::Cyan);
                        sf::Vector2f pos = uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                        txtInfo.setPosition({pos.x + 20.f, pos.y}); window.draw(txtInfo);
                    }
                    if (uavLoaded) window.draw(uavSprite); else window.draw(uavShapeFallback);
                }

                if (lanzandoUAVRefuerzo) {
                    txtInfo.setString("REFUERZOS LLEGANDO...");
                    txtInfo.setFillColor(sf::Color::Green);
                    sf::Vector2f pos = uavLoaded ? uavSprite.getPosition() : uavShapeFallback.getPosition();
                    txtInfo.setPosition({pos.x + 20.f, pos.y});
                    window.draw(txtInfo);
                    if (uavLoaded) window.draw(uavSprite); else window.draw(uavShapeFallback);
                }

                // ANIMACION ATAQUE AEREO (Dibujar)
                if (faseAtaqueAereo > 0 && avionLoaded) {
                    window.draw(sAvionAtaque);
                    if (faseAtaqueAereo == 1) {
                        txtInfo.setString("MOTORES EN MARCHA...");
                        txtInfo.setFillColor(sf::Color::Yellow);
                        txtInfo.setPosition({sAvionAtaque.getPosition().x + 20.f, sAvionAtaque.getPosition().y});
                        window.draw(txtInfo);
                    }
                }

                if (errorTimer > 0.f) {
                    txtInfo.setString(msgError);
                    txtInfo.setFillColor(sf::Color::Red);
                    txtInfo.setPosition({150.f, 300.f});
                    window.draw(txtInfo);
                }

                if (!apuntando && !cargandoDisparo && !lanzandoUAV && !lanzandoUAVRefuerzo && faseAtaqueAereo == 0) {
                    btnRadar.dibujar(window);
                    btnNotas.dibujar(window); 
                }
            }
        }

        // --- DIBUJAR MENU SUPERPUESTO ---
        if (juegoPausado) {
            window.draw(fondoMenu);
            window.draw(cajaMenu);
            window.draw(txtMenuTitulo);

            // --- ACTUALIZAR VISUALES ---
            
            // Colores de selección (Resaltar fila activa)
            sf::Color colSel = sf::Color(0, 255, 100); // Verde brillante
            sf::Color colNorm = sf::Color::White;

            if (opcionMenuSeleccionada == 0) {
                lblMusica.setFillColor(colSel);
                btnMenosMusica.setFillColor(colSel);
                btnMasMusica.setFillColor(colSel);
                lblEfectos.setFillColor(colNorm);
                btnMenosEfectos.setFillColor(colNorm);
                btnMasEfectos.setFillColor(colNorm);
            } else {
                lblMusica.setFillColor(colNorm);
                btnMenosMusica.setFillColor(colNorm);
                btnMasMusica.setFillColor(colNorm);
                lblEfectos.setFillColor(colSel);
                btnMenosEfectos.setFillColor(colSel);
                btnMasEfectos.setFillColor(colSel);
            }

            // Actualizar barras y textos numéricos
            barraMusicaFill.setSize({volMusica, 10.f}); // 100px max ancho = 100% volumen
            valMusicaTxt.setString(std::to_string((int)volMusica) + "%");

            barraEfectosFill.setSize({volEfectos, 10.f});
            valEfectosTxt.setString(std::to_string((int)volEfectos) + "%");

            // --- DIBUJAR FILA 1 (MUSICA) ---
            window.draw(lblMusica);
            window.draw(btnMenosMusica);
            window.draw(barraMusicaBg);
            window.draw(barraMusicaFill);
            window.draw(btnMasMusica);
            window.draw(valMusicaTxt);

            // --- DIBUJAR FILA 2 (EFECTOS) ---
            window.draw(lblEfectos);
            window.draw(btnMenosEfectos);
            window.draw(barraEfectosBg);
            window.draw(barraEfectosFill);
            window.draw(btnMasEfectos);
            window.draw(valEfectosTxt);

            window.draw(txtInstruccionesVol);
        }

        window.display();
    }
    return 0;
}