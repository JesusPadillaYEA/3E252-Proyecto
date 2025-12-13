#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "GameState.hpp"
#include "Jugador.hpp"
#include "Boton.hpp"
#include "AnimationSystem.hpp"
#include "RenderManager.hpp"
#include "UIManager.hpp"

namespace RenderSystem {

    struct RadarVisuals {
        sf::VertexArray radarBg;
        std::vector<sf::CircleShape> radarCircles;
        sf::RectangleShape axisX;
        sf::RectangleShape axisY;
        sf::RectangleShape sweepLine;
        sf::RectangleShape sweepGlow;
        float sweepAngle = 0.f;
        
        const sf::Color neonGreen = {0, 255, 100};
        const sf::Color neonRed = {255, 50, 50};
    };

    struct NotasVisuals {
        sf::VertexArray fondoNotas;
        std::vector<sf::RectangleShape> lineasCuaderno;
        sf::RectangleShape flechaPalo;
        sf::CircleShape flechaPunta;
        sf::Color papelClaro = {245, 235, 200};
        sf::Color papelOscuro = {100, 70, 40};
    };

    // Inicializar visuales del radar
    inline RadarVisuals inicializarRadar(const sf::Vector2u& windowSize) {
        RadarVisuals radar;
        
        sf::Vector2f radarCenter = {windowSize.x / 2.f, windowSize.y / 2.f};
        
        radar.radarBg = sf::VertexArray(sf::PrimitiveType::TriangleStrip, 4);
        radar.radarBg[0] = sf::Vertex{{0.f, 0.f}, sf::Color::Black};
        radar.radarBg[1] = sf::Vertex{{(float)windowSize.x, 0.f}, sf::Color::Black};
        radar.radarBg[2] = sf::Vertex{{0.f, (float)windowSize.y}, sf::Color(10, 25, 40)};
        radar.radarBg[3] = sf::Vertex{{(float)windowSize.x, (float)windowSize.y}, sf::Color(10, 25, 40)};

        for (int i = 1; i <= 4; ++i) {
            float r = i * 120.f;
            sf::CircleShape c(r);
            c.setOrigin({r, r});
            c.setPosition(radarCenter);
            c.setFillColor(sf::Color::Transparent);
            c.setOutlineThickness(1.f);
            c.setOutlineColor(sf::Color(radar.neonGreen.r, radar.neonGreen.g, radar.neonGreen.b, 60));
            radar.radarCircles.push_back(c);
        }

        radar.axisX = sf::RectangleShape({(float)windowSize.x, 1.f});
        radar.axisX.setPosition({0.f, radarCenter.y});
        radar.axisX.setFillColor(sf::Color(radar.neonGreen.r, radar.neonGreen.g, radar.neonGreen.b, 30));

        radar.axisY = sf::RectangleShape({1.f, (float)windowSize.y});
        radar.axisY.setPosition({radarCenter.x, 0.f});
        radar.axisY.setFillColor(sf::Color(radar.neonGreen.r, radar.neonGreen.g, radar.neonGreen.b, 30));

        float largoLinea = radar.radarCircles.back().getRadius() + 100.f;
        radar.sweepLine = sf::RectangleShape({largoLinea, 2.f});
        radar.sweepLine.setOrigin({0.f, 1.f});
        radar.sweepLine.setPosition(radarCenter);
        radar.sweepLine.setFillColor(radar.neonGreen);

        radar.sweepGlow = sf::RectangleShape({largoLinea, 15.f});
        radar.sweepGlow.setOrigin({0.f, 7.5f});
        radar.sweepGlow.setPosition(radarCenter);
        radar.sweepGlow.setFillColor(sf::Color(radar.neonGreen.r, radar.neonGreen.g, radar.neonGreen.b, 30));

        return radar;
    }

    // Inicializar visuales de notas
    inline NotasVisuals inicializarNotas(const sf::Vector2u& windowSize) {
        NotasVisuals notas;
        
        sf::Vector2f radarCenter = {windowSize.x / 2.f, windowSize.y / 2.f};
        
        notas.fondoNotas = sf::VertexArray(sf::PrimitiveType::TriangleFan, 6);
        notas.fondoNotas[0] = sf::Vertex{radarCenter, notas.papelClaro};
        notas.fondoNotas[1] = sf::Vertex{{0.f, 0.f}, notas.papelOscuro};
        notas.fondoNotas[2] = sf::Vertex{{(float)windowSize.x, 0.f}, notas.papelOscuro};
        notas.fondoNotas[3] = sf::Vertex{{(float)windowSize.x, (float)windowSize.y}, notas.papelOscuro};
        notas.fondoNotas[4] = sf::Vertex{{0.f, (float)windowSize.y}, notas.papelOscuro};
        notas.fondoNotas[5] = sf::Vertex{{0.f, 0.f}, notas.papelOscuro};

        for (float y = 80.f; y < windowSize.y; y += 40.f) {
            sf::RectangleShape linea({(float)windowSize.x, 1.f});
            linea.setPosition({0.f, y});
            linea.setFillColor(sf::Color(0, 0, 0, 40));
            notas.lineasCuaderno.push_back(linea);
        }

        notas.flechaPalo = sf::RectangleShape({25.f, 6.f});
        notas.flechaPalo.setFillColor(sf::Color::Black);
        notas.flechaPalo.setOrigin({0.f, 3.f});

        notas.flechaPunta = sf::CircleShape(10.f, 3);
        notas.flechaPunta.setFillColor(sf::Color::Black);
        notas.flechaPunta.setOrigin({10.f, 10.f});
        notas.flechaPunta.setRotation(sf::degrees(-90));

        return notas;
    }

    // Actualizar radar
    inline void actualizarRadar(RadarVisuals& radar, float deltaTime) {
        radar.sweepAngle += 2.0f;
        if (radar.sweepAngle >= 360.f) radar.sweepAngle -= 360.f;
        radar.sweepLine.setRotation(sf::degrees(radar.sweepAngle));
        radar.sweepGlow.setRotation(sf::degrees(radar.sweepAngle));
    }

    // Dibujar pantalla de menú
    inline void dibujarMenuPausa(sf::RenderWindow& window, const sf::Font& font,
                                 const GameState::StateData& data, 
                                 const sf::Vector2u& windowSize) {
        sf::RectangleShape fondoMenu({(float)windowSize.x, (float)windowSize.y});
        fondoMenu.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(fondoMenu);

        sf::RectangleShape cajaMenu({500.f, 300.f});
        cajaMenu.setOrigin({250.f, 150.f});
        cajaMenu.setPosition({(float)windowSize.x / 2.f, (float)windowSize.y / 2.f});
        cajaMenu.setFillColor(sf::Color(40, 44, 52));
        cajaMenu.setOutlineThickness(3.f);
        cajaMenu.setOutlineColor(sf::Color(0, 255, 100));
        window.draw(cajaMenu);

        // Título
        sf::Text txtMenuTitulo(font);
        txtMenuTitulo.setString("CONFIGURACION DE SISTEMAS");
        txtMenuTitulo.setCharacterSize(28);
        txtMenuTitulo.setFillColor(sf::Color::White);
        sf::FloatRect tb = txtMenuTitulo.getLocalBounds();
        txtMenuTitulo.setOrigin({tb.size.x/2.f, tb.size.y/2.f});
        txtMenuTitulo.setPosition({cajaMenu.getPosition().x, cajaMenu.getPosition().y - 110.f});
        window.draw(txtMenuTitulo);

        // Colores de selección
        sf::Color colSel = sf::Color(0, 255, 100);
        sf::Color colNorm = sf::Color::White;

        // === MUSICA ===
        sf::Text lblMusica(font, "MUSICA (BGM)", 20);
        lblMusica.setPosition({cajaMenu.getPosition().x - 200.f, cajaMenu.getPosition().y - 50.f});
        lblMusica.setFillColor((data.opcionMenuSeleccionada == 0) ? colSel : colNorm);
        window.draw(lblMusica);

        sf::Text btnMenosMusica(font, "<", 30);
        btnMenosMusica.setPosition({cajaMenu.getPosition().x + 20.f, cajaMenu.getPosition().y - 60.f});
        btnMenosMusica.setFillColor((data.opcionMenuSeleccionada == 0) ? colSel : colNorm);
        window.draw(btnMenosMusica);

        sf::RectangleShape barraMusicaBg({100.f, 10.f});
        barraMusicaBg.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y - 40.f});
        barraMusicaBg.setFillColor(sf::Color(20, 20, 20));
        window.draw(barraMusicaBg);

        sf::RectangleShape barraMusicaFill({data.volMusica, 10.f});
        barraMusicaFill.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y - 40.f});
        barraMusicaFill.setFillColor(sf::Color::Cyan);
        window.draw(barraMusicaFill);

        sf::Text btnMasMusica(font, ">", 30);
        btnMasMusica.setPosition({cajaMenu.getPosition().x + 160.f, cajaMenu.getPosition().y - 60.f});
        btnMasMusica.setFillColor((data.opcionMenuSeleccionada == 0) ? colSel : colNorm);
        window.draw(btnMasMusica);

        sf::Text valMusicaTxt(font, std::to_string((int)data.volMusica) + "%", 20);
        valMusicaTxt.setPosition({cajaMenu.getPosition().x + 190.f, cajaMenu.getPosition().y - 50.f});
        window.draw(valMusicaTxt);

        // === EFECTOS ===
        sf::Text lblEfectos(font, "EFECTOS (SFX)", 20);
        lblEfectos.setPosition({cajaMenu.getPosition().x - 200.f, cajaMenu.getPosition().y + 20.f});
        lblEfectos.setFillColor((data.opcionMenuSeleccionada == 1) ? colSel : colNorm);
        window.draw(lblEfectos);

        sf::Text btnMenosEfectos(font, "<", 30);
        btnMenosEfectos.setPosition({cajaMenu.getPosition().x + 20.f, cajaMenu.getPosition().y + 10.f});
        btnMenosEfectos.setFillColor((data.opcionMenuSeleccionada == 1) ? colSel : colNorm);
        window.draw(btnMenosEfectos);

        sf::RectangleShape barraEfectosBg({100.f, 10.f});
        barraEfectosBg.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y + 30.f});
        barraEfectosBg.setFillColor(sf::Color(20, 20, 20));
        window.draw(barraEfectosBg);

        sf::RectangleShape barraEfectosFill({data.volEfectos, 10.f});
        barraEfectosFill.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y + 30.f});
        barraEfectosFill.setFillColor(sf::Color::Yellow);
        window.draw(barraEfectosFill);

        sf::Text btnMasEfectos(font, ">", 30);
        btnMasEfectos.setPosition({cajaMenu.getPosition().x + 160.f, cajaMenu.getPosition().y + 10.f});
        btnMasEfectos.setFillColor((data.opcionMenuSeleccionada == 1) ? colSel : colNorm);
        window.draw(btnMasEfectos);

        sf::Text valEfectosTxt(font, std::to_string((int)data.volEfectos) + "%", 20);
        valEfectosTxt.setPosition({cajaMenu.getPosition().x + 190.f, cajaMenu.getPosition().y + 20.f});
        window.draw(valEfectosTxt);

        // === BOTON SALIR ===
        sf::Text btnSalir(font, "SALIR", 20);
        btnSalir.setPosition({cajaMenu.getPosition().x - 25.f, cajaMenu.getPosition().y + 80.f});
        btnSalir.setFillColor((data.opcionMenuSeleccionada == 2) ? colSel : colNorm);
        window.draw(btnSalir);

        // Instrucciones
        sf::Text txtInstruccionesVol(font, "ARRIBA/ABAJO: Seleccionar  |  IZQ/DER: Ajustar  |  ESC: Volver  |  ENTER: Salir", 14);
        txtInstruccionesVol.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect ib = txtInstruccionesVol.getLocalBounds();
        txtInstruccionesVol.setOrigin({ib.size.x/2.f, ib.size.y/2.f});
        txtInstruccionesVol.setPosition({cajaMenu.getPosition().x, cajaMenu.getPosition().y + 110.f});
        window.draw(txtInstruccionesVol);
    }

    // Dibujar pantalla de victoria
    inline void dibujarVictoria(sf::RenderWindow& window, const sf::Font& font,
                               int idGanador, const std::vector<AnimationSystem::Particula>& particulas,
                               const sf::Vector2u& windowSize) {
        // Fondo oscuro
        sf::RectangleShape fondoMenu({(float)windowSize.x, (float)windowSize.y});
        fondoMenu.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(fondoMenu);

        // Dibujar partículas
        sf::RectangleShape pixel({3.f, 3.f});
        pixel.setOrigin({1.5f, 1.5f});
        
        for (const auto& p : particulas) {
            pixel.setPosition(p.pos);
            pixel.setRotation(sf::degrees(p.angulo));
            pixel.setScale({p.escala, p.escala});
            
            sf::Color c = p.color;
            c.a = static_cast<std::uint8_t>(p.vida * 255);
            pixel.setFillColor(c);
            window.draw(pixel);
        }

        // Texto victoria
        sf::Text txtVictoria(font);
        std::wstring strGanador = (idGanador == 1) ? L"¡JUGADOR 1 GANA!" : L"¡JUGADOR 2 GANA!";
        txtVictoria.setString(strGanador);
        txtVictoria.setCharacterSize(60);
        txtVictoria.setStyle(sf::Text::Bold);
        txtVictoria.setFillColor(sf::Color::Yellow);
        txtVictoria.setOutlineThickness(4.f);
        txtVictoria.setOutlineColor(sf::Color::Black);
        
        sf::FloatRect b1 = txtVictoria.getLocalBounds();
        txtVictoria.setOrigin({b1.size.x / 2.f, b1.size.y / 2.f});
        txtVictoria.setPosition({windowSize.x / 2.f, windowSize.y / 2.f - 50.f});
        window.draw(txtVictoria);

        // Subtítulo
        sf::Text txtSubVictoria(font);
        txtSubVictoria.setString(L"FLOTA ENEMIGA HUNDIDA\nPresiona ESC para volver al Menu");
        txtSubVictoria.setCharacterSize(24);
        txtSubVictoria.setFillColor(sf::Color::White);
        
        sf::FloatRect b2 = txtSubVictoria.getLocalBounds();
        txtSubVictoria.setOrigin({b2.size.x / 2.f, b2.size.y / 2.f});
        txtSubVictoria.setPosition({windowSize.x / 2.f, windowSize.y / 2.f + 50.f});
        window.draw(txtSubVictoria);
    }

    // Dibujar modo radar
    inline void dibujarRadar(sf::RenderWindow& window, const sf::Font& font,
                            RadarVisuals& radar, const Jugador& jugador, 
                            const Boton& btnRadar, const sf::Vector2u& windowSize) {
        window.draw(radar.radarBg);
        window.draw(radar.axisX);
        window.draw(radar.axisY);
        
        for (const auto& circle : radar.radarCircles) {
            window.draw(circle);
        }

        for (const auto& pos : jugador.memoriaRadar) {
            sf::CircleShape haloOut(15.f);
            haloOut.setOrigin({15.f, 15.f});
            haloOut.setPosition(pos);
            haloOut.setFillColor(sf::Color(radar.neonRed.r, radar.neonRed.g, radar.neonRed.b, 50));
            window.draw(haloOut);

            sf::CircleShape core(5.f);
            core.setOrigin({5.f, 5.f});
            core.setPosition(pos);
            core.setFillColor(radar.neonRed);
            window.draw(core);
        }

        window.draw(radar.sweepGlow);
        window.draw(radar.sweepLine);

        sf::Text txtInfo(font);
        txtInfo.setString("ESCANEANDO... GUARDANDO EN BITACORA");
        txtInfo.setCharacterSize(20);
        txtInfo.setOutlineThickness(2.f);
        txtInfo.setFillColor(radar.neonGreen);
        txtInfo.setPosition({windowSize.x / 2.f - 200.f, 50.f});
        window.draw(txtInfo);

        btnRadar.dibujar(window);
    }

    // Dibujar modo notas
    inline void dibujarNotas(sf::RenderWindow& window, const sf::Font& font,
                            NotasVisuals& notas, const Jugador& jugadorActual,
                            const Jugador& jugadorEnemigo, const Boton& btnNotas,
                            const sf::Vector2u& windowSize) {
        window.draw(notas.fondoNotas);
        
        for (const auto& linea : notas.lineasCuaderno) {
            window.draw(linea);
        }

        for (const auto& barco : jugadorActual.getFlota()) {
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

        for (const auto& pos : jugadorActual.memoriaRadar) {
            sf::CircleShape mancha1(20.f);
            mancha1.setOrigin({20.f, 20.f});
            mancha1.setPosition(pos);
            mancha1.setFillColor(sf::Color(180, 0, 0, 30));
            window.draw(mancha1);

            sf::CircleShape nucleo(7.f);
            nucleo.setOrigin({7.f, 7.f});
            nucleo.setPosition(pos);
            nucleo.setFillColor(sf::Color(160, 0, 0, 255));
            window.draw(nucleo);
        }

        for (const auto& barco : jugadorEnemigo.getFlota()) {
            if (barco.destruido) {
                sf::FloatRect b = barco.sprite.getGlobalBounds();
                sf::Vector2f centro = {b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f};
                
                sf::RectangleShape linea1({60.f, 8.f});
                linea1.setOrigin({30.f, 4.f});
                linea1.setPosition(centro);
                linea1.setFillColor(sf::Color(139, 0, 0, 220));
                linea1.setRotation(sf::degrees(45));
                
                sf::RectangleShape linea2({60.f, 8.f});
                linea2.setOrigin({30.f, 4.f});
                linea2.setPosition(centro);
                linea2.setFillColor(sf::Color(139, 0, 0, 220));
                linea2.setRotation(sf::degrees(-45));
                
                window.draw(linea1);
                window.draw(linea2);
            }
        }

        sf::Text txtInfo(font);
        txtInfo.setString("BITACORA DE CAPITAN - REPORTE TACTICO");
        txtInfo.setCharacterSize(20);
        txtInfo.setFillColor(sf::Color(60, 40, 20));
        txtInfo.setPosition({300.f, 30.f});
        window.draw(txtInfo);

        btnNotas.dibujar(window);

        sf::Rect<float> btnBounds = btnNotas.getBounds();
        sf::Vector2f btnCenter = {btnBounds.position.x + btnBounds.size.x / 2.f, 
                                  btnBounds.position.y + btnBounds.size.y / 2.f};
        notas.flechaPalo.setPosition({btnCenter.x + 10.f, btnCenter.y});
        notas.flechaPunta.setPosition({btnCenter.x - 15.f, btnCenter.y});
        window.draw(notas.flechaPalo);
        window.draw(notas.flechaPunta);
    }
}
