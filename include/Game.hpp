#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include "ResourceManager.hpp"
#include "EffectManager.hpp"
#include "Jugador.hpp"
#include "Boton.hpp"
#include "EstadoJuego.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void inicializarUI();
    void manejarTurno();
    bool verificarVictoria(const Jugador& enemigo);
    void ejecutarLauncher();
    void actualizarVolumen();

    sf::RenderWindow window;
    const sf::Vector2u WINDOW_SIZE;
    ResourceManager resources;
    EffectManager effects;

    // Entidades
    Jugador p1;
    Jugador p2;
    
    // Sprites opcionales (se crean tras cargar texturas)
    std::optional<sf::Sprite> fondo;
    std::optional<sf::Sprite> uavSprite;
    std::optional<sf::Sprite> sAvionAtaque;
    
    sf::CircleShape uavShapeFallback;
    float offset[2];

    // Audio - SFML 3: sf::Sound no tiene constructor por defecto
    sf::Music musicaFondo; // Music sí tiene constructor default
    std::optional<sf::Sound> sRadar;
    std::optional<sf::Sound> sUAV;
    std::optional<sf::Sound> sButton;
    std::optional<sf::Sound> sAirStrike;
    std::optional<sf::Sound> sNotas;
    std::optional<sf::Sound> sError;
    std::optional<sf::Sound> sShotFail;
    std::optional<sf::Sound> sShotDone;
    std::optional<sf::Sound> sDestruccion;
    
    sf::Sound* sonidoDisparoActual; // Puntero para controlar el audio activo

    // UI - sf::Text no tiene constructor por defecto
    std::optional<Boton> btnAtacar;
    std::optional<Boton> btnMover;
    std::optional<Boton> btnRadar;
    std::optional<Boton> btnNotas;
    std::optional<Boton> btnSalirMenu;

    std::optional<sf::Text> txtInfo;
    std::optional<sf::Text> txtVictoria;
    std::optional<sf::Text> txtSubVictoria;
    
    // Menú
    sf::RectangleShape fondoMenu, cajaMenu;
    std::optional<sf::Text> txtMenuTitulo;
    std::optional<sf::Text> lblMusica;
    std::optional<sf::Text> lblEfectos;
    std::optional<sf::Text> btnMenosMusica;
    std::optional<sf::Text> btnMasMusica;
    std::optional<sf::Text> valMusicaTxt;
    std::optional<sf::Text> btnMenosEfectos;
    std::optional<sf::Text> btnMasEfectos;
    std::optional<sf::Text> valEfectosTxt;
    std::optional<sf::Text> txtInstruccionesVol;
    
    sf::RectangleShape barraMusicaBg, barraMusicaFill, barraEfectosBg, barraEfectosFill;

    // Estado
    EstadoJuego estado;
    bool juegoPausado;
    int opcionMenuSeleccionada;
    float volMusica, volEfectos;
    int sel;
    bool moviendo, apuntando, cargandoDisparo;
    float potenciaActual;
    bool modoRadar, modoNotas, lanzandoUAV, lanzandoUAVRefuerzo;
    float errorTimer;
    sf::String msgError;
    sf::Clock uavTimer, timerAtaqueAereo;
    
    int faseAtaqueAereo; 
    sf::Vector2f posAvionAtaque;
    float colObjetivoX;
    
    int idGanador, ganadorDetectado;
    float timerVictoria;
    int faseDisparoNormal;
    bool enZonaEnemiga;
    sf::Vector2f origenDisparoReal;
    sf::Vector2f impactoVisual;

    // Estéticos
    sf::VertexArray radarBg;
    std::vector<sf::CircleShape> radarCircles;
    sf::RectangleShape axisX, axisY, sweepLine, sweepGlow;
    float radarSweepAngle;
    sf::VertexArray fondoNotas;
    std::vector<sf::RectangleShape> lineasCuaderno;
    sf::RectangleShape flechaPalo;
    sf::CircleShape flechaPunta;
};