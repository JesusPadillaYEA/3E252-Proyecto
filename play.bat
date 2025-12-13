@echo off
REM Script para compilar main.exe y JuegoProyecto.exe, y luego ejecutar el launcher

echo === Compilando Batalla Naval ===

REM Compilar main.exe
echo.
echo Compilando main.cpp...
g++ -Wall -Wextra -std=c++17 -I.\include -I.\sfml\include src\main.cpp -o .\bin\main.exe -L.\sfml\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 2>&1 | find /V ""

if %ERRORLEVEL% equ 0 (
    echo [OK] main.exe compilado exitosamente
) else (
    echo [ERROR] Error compilando main.exe
)

REM Compilar JuegoProyecto.exe
echo.
echo Compilando 3_launcher.cpp...
g++ -Wall -Wextra -std=c++17 -I.\include -I.\sfml\include src\3_launcher.cpp -o .\bin\JuegoProyecto.exe -L.\sfml\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 2>&1 | find /V ""

if %ERRORLEVEL% equ 0 (
    echo [OK] JuegoProyecto.exe compilado exitosamente
) else (
    echo [ERROR] Error compilando JuegoProyecto.exe
)

echo.
echo === Compilacion completada ===
echo.
echo Ejecutando Launcher...
.\bin\JuegoProyecto.exe
