# Script de compilación para Batalla Naval
# Compila main.cpp a bin/main.exe y 3_launcher.cpp a bin/JuegoProyecto.exe

Write-Host "=== Compilando Batalla Naval ===" -ForegroundColor Cyan

# Compilar main.exe
Write-Host "`nCompilando main.cpp..." -ForegroundColor Yellow
g++ -Wall -Wextra -std=c++17 -I.\include -I.\sfml\include src\main.cpp -o .\bin\main.exe -L.\sfml\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 2>&1 | Select-String 'error|warning'

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ main.exe compilado exitosamente" -ForegroundColor Green
} else {
    Write-Host "✗ Error compilando main.exe" -ForegroundColor Red
}

# Compilar JuegoProyecto.exe
Write-Host "`nCompilando 3_launcher.cpp..." -ForegroundColor Yellow
g++ -Wall -Wextra -std=c++17 -I.\include -I.\sfml\include src\3_launcher.cpp -o .\bin\JuegoProyecto.exe -L.\sfml\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 2>&1 | Select-String 'error|warning'

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ JuegoProyecto.exe compilado exitosamente" -ForegroundColor Green
} else {
    Write-Host "✗ Error compilando JuegoProyecto.exe" -ForegroundColor Red
}

Write-Host "`n=== Compilación completada ===" -ForegroundColor Cyan
Get-ChildItem .\bin -Filter '*.exe' | Select-Object Name, @{Name='Tamaño (KB)';Expression={[math]::Round($_.Length/1024, 2)}}
