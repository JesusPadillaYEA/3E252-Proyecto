#include "Game.hpp"
#include "GeneradorFlotas.hpp"
#include "MovementManager.hpp"
#include "AttackManager.hpp"
#include "IndicatorManager.hpp"
#include "UIManager.hpp"
#include "RenderManager.hpp"
#include <cmath>
#include <iostream>

Game::Game() 
    : WINDOW_SIZE(1000, 700),
      window(sf::VideoMode(WINDOW_SIZE), "Batalla Naval - Bitacora (SFML 3.0)"),
      p1(1, {0.f, 0.f}), p2(2, {700.f, 0.f})
{
    window.setFramerateLimit(60);
    
    // Inicializar primitivas
    estado = MENSAJE_P1;
    juegoPausado = false;
    opcionMenuSeleccionada = 0;
    volMusica = 50.f; volEfectos = 50.f;
    sel = -1;
    moviendo = false; apuntando = false; cargandoDisparo = false;
    potenciaActual = 0.f;
    modoRadar = false; modoNotas = false; 
    lanzandoUAV = false; lanzandoUAVRefuerzo = false;
    errorTimer = 0.f;
    faseAtaqueAereo = 0;
    idGanador = 0; ganadorDetectado = 0; timerVictoria = 0.f;
    faseDisparoNormal = 0;
    sonidoDisparoActual = nullptr;
    offset[0] = 0.f; offset[1] = 0.f;
    radarSweepAngle = 0.f;

    // 1. Cargar Recursos
    if (!resources.loadResources()) {
        std::cerr << "Error critico cargando recursos." << std::endl;
        exit(-1);
    }

    // 2. Inicializar Sprites
    fondo.emplace(resources.tMar);
    fondo->setTextureRect(sf::IntRect({0, 0}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));

    uavSprite.emplace(resources.tUAV);
    if(resources.uavLoaded) {
        sf::FloatRect b = uavSprite->getLocalBounds();
        uavSprite->setOrigin({b.size.x/2.f, b.size.y/2.f});
    } else {
        uavShapeFallback.setRadius(5.f); uavShapeFallback.setPointCount(3);
        uavShapeFallback.setFillColor(sf::Color::Cyan); uavShapeFallback.setOrigin({5.f, 5.f});
    }

    sAvionAtaque.emplace(resources.tAvionAtaque);
    if(resources.avionLoaded) {
        sf::FloatRect b = sAvionAtaque->getLocalBounds();
        sAvionAtaque->setOrigin({b.size.x/2.f, b.size.y/2.f});
    }

    // 3. Inicializar Audio (Ahora con Buffer)
    sRadar.emplace(resources.bufRadar); sRadar->setLooping(true);
    sUAV.emplace(resources.bufUAV);
    sButton.emplace(resources.bufButton);
    sAirStrike.emplace(resources.bufAirStrike);
    sNotas.emplace(resources.bufNotas);
    sError.emplace(resources.bufError);
    sShotFail.emplace(resources.bufShotFail);
    sShotDone.emplace(resources.bufShotDone);
    sDestruccion.emplace(resources.bufDestruccion);

    if (musicaFondo.openFromFile("assets/music/main.ogg")) {
        musicaFondo.setLooping(true);
        musicaFondo.setVolume(volMusica);
        musicaFondo.play();
    }
    actualizarVolumen();

    // 4. Inicializar Botones
    btnAtacar.emplace(resources.font, "ATACAR", sf::Color(200, 50, 50));
    btnMover.emplace(resources.font, "MOVER", sf::Color(50, 150, 50));
    btnRadar.emplace(resources.font, "RADAR", sf::Color(60, 70, 80));
    btnNotas.emplace(resources.font, "NOTAS", sf::Color(139, 100, 60));
    btnSalirMenu.emplace(resources.font, "SALIR AL MENU", sf::Color(250, 50, 50));

    // Configurar Flotas
    GeneradorFlotas::inicializarFlota(p1, resources.tDest, resources.tPort, resources.tSub);
    GeneradorFlotas::inicializarFlota(p2, resources.tDest, resources.tPort, resources.tSub);

    inicializarUI();
}

void Game::run() {
    sf::Clock dtClock;
    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    while (const auto ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) window.close();
        
        if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::Escape) {
                if (idGanador != 0) { ejecutarLauncher(); return; }
                if (cargandoDisparo || apuntando || moviendo || sel != -1 || modoRadar || modoNotas || faseAtaqueAereo > 0) {
                    cargandoDisparo = false; apuntando = false; moviendo = false; sel = -1;
                    modoRadar = false; modoNotas = false; faseAtaqueAereo = 0;
                    lanzandoUAV = false; lanzandoUAVRefuerzo = false;
                    
                    if(btnAtacar) btnAtacar->resetColor(); 
                    if(btnMover) btnMover->resetColor();
                    if(btnRadar) btnRadar->resetColor();
                    if(btnNotas) btnNotas->resetColor();
                    
                    sRadar->stop(); sUAV->stop(); sAirStrike->stop();
                } else {
                    juegoPausado = !juegoPausado;
                }
            }
            if (juegoPausado) {
               if (k->code == sf::Keyboard::Key::Up || k->code == sf::Keyboard::Key::Down) {
                   sButton->play(); opcionMenuSeleccionada = !opcionMenuSeleccionada;
               }
               if (k->code == sf::Keyboard::Key::Left) {
                   sButton->play();
                   if (opcionMenuSeleccionada == 0) volMusica = std::max(0.f, volMusica - 5.f);
                   else volEfectos = std::max(0.f, volEfectos - 5.f);
                   actualizarVolumen();
               }
               if (k->code == sf::Keyboard::Key::Right) {
                   sButton->play();
                   if (opcionMenuSeleccionada == 0) volMusica = std::min(100.f, volMusica + 5.f);
                   else volEfectos = std::min(100.f, volEfectos + 5.f);
                   actualizarVolumen();
               }
            }
        }

        if (juegoPausado) continue;

        if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
            if (ev->is<sf::Event::MouseButtonPressed>() || ev->is<sf::Event::KeyPressed>()) {
                manejarTurno();
            }
            continue;
        }

        if (lanzandoUAV || lanzandoUAVRefuerzo || faseAtaqueAereo > 0 || faseDisparoNormal > 0 || idGanador != 0) continue;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        Jugador* jugadorActual = (estado == TURNO_P1) ? &p1 : &p2;

        if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left) {
                if (btnRadar && btnRadar->esClickeado(mousePos) && !modoNotas && !apuntando) {
                    sButton->play();
                    if(!modoRadar) {
                        if(jugadorActual->cooldownRadar > 0) { sError->play(); msgError = "ENFRIANDO..."; errorTimer = 2.f; }
                        else { 
                            lanzandoUAV = true; sUAV->play(); uavTimer.restart(); jugadorActual->cooldownRadar = 2;
                            bool carrier = false;
                            for(auto& b : jugadorActual->getFlota()) if(b.nombre.find("Portaviones")!=std::string::npos && !b.destruido) {
                                uavSprite->setPosition(b.sprite.getPosition()); carrier=true; break;
                            }
                            if(!carrier) { lanzandoUAV = false; sUAV->stop(); sError->play(); msgError = "PORTAVIONES DESTRUIDO"; errorTimer=2.f; }
                            else { btnRadar->setColor(sf::Color::Red); }
                        }
                    } else { modoRadar = false; sRadar->stop(); btnRadar->resetColor(); }
                }
                else if (btnNotas && btnNotas->esClickeado(mousePos) && !modoRadar && !apuntando) {
                    sButton->play(); sNotas->play(); modoNotas = !modoNotas;
                    if(modoNotas) btnNotas->setColor(sf::Color::White); else btnNotas->resetColor();
                }
                else if (!modoRadar && !modoNotas) {
                    if (sel != -1 && !apuntando) {
                        if (btnAtacar && btnAtacar->esClickeado(mousePos)) {
                            bool esCarrier = (jugadorActual->getFlota()[sel].nombre.find("Portaviones") != std::string::npos);
                            if(esCarrier && jugadorActual->cooldownAtaqueAereo == 0) {
                                faseAtaqueAereo = 1; timerAtaqueAereo.restart(); sAirStrike->play();
                                sAvionAtaque->setPosition(jugadorActual->getFlota()[sel].sprite.getPosition());
                                colObjetivoX = sAvionAtaque->getPosition().x;
                                jugadorActual->cooldownAtaqueAereo = 5;
                                sel = -1;
                                btnAtacar->setColor(sf::Color::Red);
                            } else if (esCarrier) {
                                sError->play(); msgError = "ESCUADRON NO DISPONIBLE"; errorTimer=2.f;
                            } else {
                                apuntando = true; btnAtacar->setColor({255, 69, 0});
                            }
                        } else if (btnMover && btnMover->esClickeado(mousePos)) {
                            if(moviendo) { IndicatorManager::actualizarTurnos(*jugadorActual); estado = (estado==TURNO_P1)?MENSAJE_P2:MENSAJE_P1; moviendo=false; sel=-1; btnMover->resetColor(); }
                            else { moviendo = true; btnMover->setColor({255,140,0}); }
                        }
                    }
                    if (!apuntando && !moviendo) {
                        bool hit = false;
                        for(size_t i=0; i<jugadorActual->getFlota().size(); ++i) {
                            if(!jugadorActual->getFlota()[i].destruido && jugadorActual->getFlota()[i].sprite.getGlobalBounds().contains(mousePos)) {
                                sel = (int)i; hit=true; 
                                if(btnMover) btnMover->resetColor(); 
                                if(btnAtacar) btnAtacar->resetColor();
                                break;
                            }
                        }
                        if(!hit) sel = -1;
                    }
                }
            }
        }
        
        if (const auto* m = ev->getIf<sf::Event::MouseButtonReleased>()) {
            if (m->button == sf::Mouse::Button::Left && cargandoDisparo) {
                cargandoDisparo = false; apuntando = false; sel = -1; 
                if(btnAtacar) btnAtacar->resetColor();
                Jugador* enemigo = (estado == TURNO_P1) ? &p2 : &p1;
                
                sf::Vector2f diff = mousePos - origenDisparoReal;
                float ang = std::atan2(diff.y, diff.x);
                sf::Vector2f impacto = origenDisparoReal + sf::Vector2f(std::cos(ang)*potenciaActual, std::sin(ang)*potenciaActual);
                
                std::vector<sf::Vector2f> bajas;
                bool hit = AttackManager::procesarDisparo(impacto, origenDisparoReal, *enemigo, bajas);
                for(auto& p : bajas) enemigo->explosionesPendientes.push_back(p);
                
                if(hit) { sShotDone->play(); sonidoDisparoActual = &(*sShotDone); }
                else { sShotFail->play(); sonidoDisparoActual = &(*sShotFail); }
                faseDisparoNormal = 1;
            }
        }
    }
}

void Game::update(float dt) {
    if (errorTimer > 0.f) errorTimer -= dt;
    
    effects.update(dt, resources.animExplosionTex, resources.animFuegoTex);

    if (lanzandoUAV || lanzandoUAVRefuerzo) {
        if(uavSprite) uavSprite->move({0.f, -3.f});
        else uavShapeFallback.move({0.f, -3.f});

        float y = uavSprite ? uavSprite->getPosition().y : uavShapeFallback.getPosition().y;
        if(y < -50) {
            lanzandoUAV = false; lanzandoUAVRefuerzo = false;
            modoRadar = true; sRadar->play(); sUAV->stop();
            if(btnRadar) btnRadar->setColor(sf::Color::Red);
        }
    }

    if (faseAtaqueAereo > 0 && sAvionAtaque) {
        if(faseAtaqueAereo == 1 && timerAtaqueAereo.getElapsedTime().asSeconds() > 1.f) faseAtaqueAereo = 2;
        if(faseAtaqueAereo == 2) {
            sAvionAtaque->move({0.f, -4.f});
            if(sAvionAtaque->getPosition().y < -100.f) {
                Jugador* enemigo = (estado == TURNO_P1) ? &p2 : &p1;
                std::vector<sf::Vector2f> bajas;
                AttackManager::procesarAtaqueAereo(colObjetivoX, *enemigo, bajas);
                for(auto& p : bajas) enemigo->explosionesPendientes.push_back(p);
                faseAtaqueAereo = 0;
                estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
            }
        }
    }

    if(faseDisparoNormal == 1 && sonidoDisparoActual && sonidoDisparoActual->getStatus() == sf::Sound::Status::Stopped) {
        faseDisparoNormal = 0; sonidoDisparoActual = nullptr;
        IndicatorManager::actualizarTurnos((estado == TURNO_P1) ? p1 : p2);
        estado = (estado == TURNO_P1) ? MENSAJE_P2 : MENSAJE_P1;
    }

    if (cargandoDisparo) potenciaActual = std::min(1500.f, potenciaActual + 5.f);
    
    if (ganadorDetectado != 0) {
        timerVictoria -= dt;
        if (timerVictoria <= 0) { idGanador = ganadorDetectado; ganadorDetectado = 0; }
    }
    
    if (idGanador != 0 && rand() % 20 == 0) {
        float x = static_cast<float>(rand() % WINDOW_SIZE.x);
        float y = static_cast<float>(rand() % (WINDOW_SIZE.y/2));
        effects.addParticulas({x, y}, 50);
    }
    
    if (!modoRadar && !modoNotas) {
        offset[0] += 0.5f; offset[1] += 0.25f;
        if(offset[0] > WINDOW_SIZE.x) offset[0] = 0; if(offset[1] > WINDOW_SIZE.y) offset[1] = 0;
        if(fondo) fondo->setTextureRect(sf::IntRect({(int)offset[0], (int)offset[1]}, {(int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y}));
    }
}

void Game::render() {
    window.clear();
    if (estado == MENSAJE_P1 || estado == MENSAJE_P2) {
        if(fondo) window.draw(*fondo);
        UIManager::dibujarTooltipTurno(window, resources.font, estado);
    } 
    else if (idGanador != 0) {
        window.draw(fondoMenu);
        effects.draw(window); 
        if(txtVictoria) window.draw(*txtVictoria);
        if(txtSubVictoria) window.draw(*txtSubVictoria);
    }
    else {
        Jugador* actual = (estado == TURNO_P1) ? &p1 : &p2;
        if (modoRadar) {
            window.draw(radarBg); window.draw(axisX); window.draw(axisY);
            for(auto& c : radarCircles) window.draw(c);
            window.draw(sweepLine);
            for(auto& pos : actual->memoriaRadar) {
                sf::CircleShape blip(5.f); blip.setPosition(pos); blip.setFillColor(sf::Color::Red); window.draw(blip);
            }
            if(btnRadar) btnRadar->dibujar(window);
        }
        else if (modoNotas) {
            window.draw(fondoNotas);
            for(const auto& l : lineasCuaderno) window.draw(l);
            if(btnNotas) btnNotas->dibujar(window);
        }
        else {
            if(fondo) window.draw(*fondo);
            sf::Color borde = moviendo ? sf::Color(255,140,0) : sf::Color(50,150,50);
            if (apuntando) borde = sf::Color::Red;
            RenderManager::renderizarFlota(window, actual->getFlota(), sel, estado, moviendo, borde);
            effects.draw(window); 
            
            if (apuntando && sel != -1) {
                sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                IndicatorManager::dibujarVectorApuntado(window, origenDisparoReal, mouse, cargandoDisparo ? potenciaActual : 50.f, resources.font);
            }
            
            if (lanzandoUAV || lanzandoUAVRefuerzo) {
                if(uavSprite) window.draw(*uavSprite); else window.draw(uavShapeFallback);
            }
            if (faseAtaqueAereo > 0 && sAvionAtaque) window.draw(*sAvionAtaque);
            
            if(!cargandoDisparo && !lanzandoUAV && !lanzandoUAVRefuerzo && faseAtaqueAereo == 0) {
                if(sel!=-1 && !actual->getFlota()[sel].destruido) {
                    auto& b = actual->getFlota()[sel];
                    sf::Vector2f p = b.sprite.getPosition();
                    float yBtn = p.y + b.sprite.getGlobalBounds().size.y + 10.f;
                    if(btnAtacar) { btnAtacar->setPosition({p.x, yBtn}); btnAtacar->dibujar(window); }
                    if(btnMover) { btnMover->setPosition({p.x + 110.f, yBtn}); btnMover->dibujar(window); }
                }
                if(btnRadar) btnRadar->dibujar(window); 
                if(btnNotas) btnNotas->dibujar(window);
            }
            if(errorTimer > 0 && txtInfo) {
                txtInfo->setString(msgError); window.draw(*txtInfo);
            }
        }
    }
    
    if (juegoPausado) {
        window.draw(fondoMenu); window.draw(cajaMenu);
        if(txtMenuTitulo) window.draw(*txtMenuTitulo); 
        if(lblMusica) window.draw(*lblMusica); 
        window.draw(barraMusicaBg); window.draw(barraMusicaFill); 
        
        if(btnMenosMusica) window.draw(*btnMenosMusica); 
        if(btnMasMusica) window.draw(*btnMasMusica); 
        if(valMusicaTxt) window.draw(*valMusicaTxt);
        
        if(lblEfectos) window.draw(*lblEfectos); 
        window.draw(barraEfectosBg); window.draw(barraEfectosFill);
        
        if(btnMenosEfectos) window.draw(*btnMenosEfectos); 
        if(btnMasEfectos) window.draw(*btnMasEfectos); 
        if(valEfectosTxt) window.draw(*valEfectosTxt);
        
        if(txtInstruccionesVol) window.draw(*txtInstruccionesVol);

        if(btnSalirMenu) btnSalirMenu->dibujar(window);
    }
    
    window.display();
}

void Game::manejarTurno() {
    EstadoJuego nextState = (estado == MENSAJE_P1) ? TURNO_P1 : TURNO_P2;
    Jugador* nextPlayer = (nextState == TURNO_P1) ? &p1 : &p2;
    Jugador* prevPlayer = (nextState == TURNO_P1) ? &p2 : &p1;
    
    effects.clearFuegos();
    if (nextPlayer->columnaFuegoX > 0) {
        effects.addFuego(resources.animFuegoTex, nextPlayer->columnaFuegoX, (float)WINDOW_SIZE.y);
    }
    if (!nextPlayer->explosionesPendientes.empty()) {
        sDestruccion->play();
        for(auto& pos : nextPlayer->explosionesPendientes) {
            effects.addExplosion(resources.animExplosionTex[0], pos);
        }
        nextPlayer->explosionesPendientes.clear();
    }
    if (verificarVictoria(*nextPlayer)) {
        ganadorDetectado = (nextPlayer->id == 1) ? 2 : 1;
        timerVictoria = 4.0f;
    }
    if(nextPlayer->cooldownRadar > 0) nextPlayer->cooldownRadar--;
    if(nextPlayer->cooldownAtaqueAereo > 0) nextPlayer->cooldownAtaqueAereo--;
    prevPlayer->columnaFuegoX = -1.f;
    estado = nextState;
}

bool Game::verificarVictoria(const Jugador& enemigo) {
    for (const auto& barco : enemigo.getFlota()) {
        if (!barco.destruido) return false;
    }
    return true;
}

void Game::actualizarVolumen() {
    musicaFondo.setVolume(volMusica);
    if(sRadar) sRadar->setVolume(volEfectos);
    if(sUAV) sUAV->setVolume(volEfectos);
    if(sButton) sButton->setVolume(volEfectos);
    if(sAirStrike) sAirStrike->setVolume(volEfectos);
    if(sNotas) sNotas->setVolume(volEfectos);
    if(sError) sError->setVolume(volEfectos);
    if(sShotFail) sShotFail->setVolume(volEfectos);
    if(sShotDone) sShotDone->setVolume(volEfectos);
    if(sDestruccion) sDestruccion->setVolume(volEfectos);
    
    barraMusicaFill.setSize({volMusica, 10.f});
    if(valMusicaTxt) valMusicaTxt->setString(std::to_string((int)volMusica) + "%");
    barraEfectosFill.setSize({volEfectos, 10.f});
    if(valEfectosTxt) valEfectosTxt->setString(std::to_string((int)volEfectos) + "%");
}

void Game::inicializarUI() {
    if(btnRadar) btnRadar->setPosition({850.f, 20.f});
    if(btnNotas) btnNotas->setPosition({730.f, 20.f});
    if(btnSalirMenu) btnSalirMenu->setPosition({(float)WINDOW_SIZE.x/2.f - 125.f, (float)WINDOW_SIZE.y/2.f + 80.f});

    txtInfo.emplace(resources.font, "", 20); txtInfo->setFillColor(sf::Color::Yellow);
    txtVictoria.emplace(resources.font, "", 60); txtVictoria->setFillColor(sf::Color::Yellow);
    txtSubVictoria.emplace(resources.font, "", 24);
    
    // Inicializar Menu (Fondo y Textos)
    fondoMenu.setSize({(float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y}); fondoMenu.setFillColor(sf::Color(0,0,0,200));
    cajaMenu.setSize({500.f, 300.f}); cajaMenu.setOrigin({250.f, 150.f}); 
    cajaMenu.setPosition({(float)WINDOW_SIZE.x/2.f, (float)WINDOW_SIZE.y/2.f});
    cajaMenu.setFillColor(sf::Color(40,44,52)); cajaMenu.setOutlineThickness(3.f); cajaMenu.setOutlineColor(sf::Color(0,255,100));

    // Inicializar Textos Menu
    txtMenuTitulo.emplace(resources.font, "CONFIGURACION", 28);
    sf::FloatRect tb = txtMenuTitulo->getLocalBounds();
    txtMenuTitulo->setOrigin({tb.size.x/2.f, tb.size.y/2.f});
    txtMenuTitulo->setPosition({cajaMenu.getPosition().x, cajaMenu.getPosition().y - 110.f});

    lblMusica.emplace(resources.font, "MUSICA", 30); lblMusica->setPosition({cajaMenu.getPosition().x - 200.f, cajaMenu.getPosition().y - 50.f});
    barraMusicaBg.setSize({100.f, 10.f}); barraMusicaBg.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y - 40.f}); barraMusicaBg.setFillColor(sf::Color(20,20,20));
    barraMusicaFill.setFillColor(sf::Color::Cyan); barraMusicaFill.setPosition(barraMusicaBg.getPosition());
    
    lblEfectos.emplace(resources.font, "EFECTOS", 30); lblEfectos->setPosition({cajaMenu.getPosition().x - 200.f, cajaMenu.getPosition().y + 20.f});
    barraEfectosBg.setSize({100.f, 10.f}); barraEfectosBg.setPosition({cajaMenu.getPosition().x + 50.f, cajaMenu.getPosition().y + 30.f}); barraEfectosBg.setFillColor(sf::Color(20,20,20));
    barraEfectosFill.setFillColor(sf::Color::Yellow); barraEfectosFill.setPosition(barraEfectosBg.getPosition());

    btnMenosMusica.emplace(resources.font, "<", 30); btnMenosMusica->setPosition({cajaMenu.getPosition().x + 20.f, cajaMenu.getPosition().y - 60.f});
    btnMasMusica.emplace(resources.font, ">", 30); btnMasMusica->setPosition({cajaMenu.getPosition().x + 160.f, cajaMenu.getPosition().y - 60.f});
    valMusicaTxt.emplace(resources.font, "", 20); valMusicaTxt->setPosition({cajaMenu.getPosition().x + 190.f, cajaMenu.getPosition().y - 50.f});

    btnMenosEfectos.emplace(resources.font, "<", 30); btnMenosEfectos->setPosition({cajaMenu.getPosition().x + 20.f, cajaMenu.getPosition().y + 10.f});
    btnMasEfectos.emplace(resources.font, ">", 30); btnMasEfectos->setPosition({cajaMenu.getPosition().x + 160.f, cajaMenu.getPosition().y + 10.f});
    valEfectosTxt.emplace(resources.font, "", 20); valEfectosTxt->setPosition({cajaMenu.getPosition().x + 190.f, cajaMenu.getPosition().y + 20.f});

    txtInstruccionesVol.emplace(resources.font, "FLECHAS: Ajustar | ESC: Volver", 14);
    sf::FloatRect ib = txtInstruccionesVol->getLocalBounds();
    txtInstruccionesVol->setOrigin({ib.size.x/2.f, ib.size.y/2.f});
    txtInstruccionesVol->setPosition({cajaMenu.getPosition().x, cajaMenu.getPosition().y + 110.f});

    // Inicializar Geometría Radar (Usando llaves {})
    radarBg.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    radarBg.resize(4);
    radarBg[0] = sf::Vertex{sf::Vector2f(0.f, 0.f), sf::Color::Black};
    radarBg[1] = sf::Vertex{sf::Vector2f((float)WINDOW_SIZE.x, 0.f), sf::Color::Black};
    radarBg[2] = sf::Vertex{sf::Vector2f(0.f, (float)WINDOW_SIZE.y), sf::Color(10, 25, 40)};
    radarBg[3] = sf::Vertex{sf::Vector2f((float)WINDOW_SIZE.x, (float)WINDOW_SIZE.y), sf::Color(10, 25, 40)};
    
    sf::Vector2f center((float)WINDOW_SIZE.x/2.f, (float)WINDOW_SIZE.y/2.f);
    axisX.setSize({(float)WINDOW_SIZE.x, 1.f}); axisX.setPosition({0.f, center.y}); axisX.setFillColor(sf::Color(0, 255, 100, 30));
    axisY.setSize({1.f, (float)WINDOW_SIZE.y}); axisY.setPosition({center.x, 0.f}); axisY.setFillColor(sf::Color(0, 255, 100, 30));
    sweepLine.setSize({400.f, 2.f}); sweepLine.setPosition(center); sweepLine.setFillColor(sf::Color(0, 255, 100));
}

void Game::ejecutarLauncher() {
    window.close();
    system("start ./JuegoProyecto.exe");
}