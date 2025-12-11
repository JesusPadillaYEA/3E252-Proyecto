# --- AGREGA ESTO AL INICIO DEL MAKEFILE ---
# Ajusta esta ruta a donde tengas tu SFML
SFML_PATH := ./sfml

# Directorios
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include

# Compilador y flags
CXX := g++
# AQUI AGREGAMOS EL INCLUDE DE SFML (-I)
CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR) -I$(SFML_PATH)/include

# Librerías SFML
# AQUI AGREGAMOS LA RUTA DE LIBRERIAS (-L)
SFML_LIBS := -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# ... el resto del makefile sigue igual ...

# Directorios
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include
ASSETS_DIR := assets

# Compilador y flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)

# Librerías SFML
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Obtener todos los archivos .cpp en el directorio de origen
CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)

# Generar los nombres de los archivos .exe en el directorio de destino
EXE_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.exe,$(CPP_FILES))

# Regla por defecto
all: $(EXE_FILES)

# Regla para compilar cada archivo .cpp y generar el archivo .exe correspondiente
$(BIN_DIR)/%.exe: $(SRC_DIR)/%.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(SFML_LIBS)
	@echo "✓ Compilado: $@"

# Regla para ejecutar un archivo específico (ej: make run0 para 0_Ventana.exe)
run%: $(BIN_DIR)/%.exe
	@echo "Ejecutando $<..."
	./$<

# Regla para compilar y ejecutar el mapa (1_mapa.exe)
runmap: $(BIN_DIR)/1_mapa.exe
	@echo "Ejecutando 1_mapa.exe..."
	./$<

# Regla para compilar y ejecutar la ventana (0_Ventana.exe)
runwindow: $(BIN_DIR)/0_Ventana.exe
	@echo "Ejecutando 0_Ventana.exe..."
	./$<

# Regla para compilar y ejecutar visualización de unidades (2_visualizacion_unidades.exe)
rununidades: $(BIN_DIR)/2_visualizacion_unidades.exe
	@echo "Ejecutando 2_visualizacion_unidades.exe..."
	./$<

# Regla explícita para run2
run2: $(BIN_DIR)/2_visualizacion_unidades.exe
	@echo "Ejecutando 2_visualizacion_unidades.exe..."
	cd "$(CURDIR)" && ./$(BIN_DIR)/2_visualizacion_unidades.exe

# Regla explícita para run3 - Launcher
run3: $(BIN_DIR)/3_launcher.exe
	@echo "Ejecutando 3_launcher.exe..."
	cd "$(CURDIR)" && ./$(BIN_DIR)/3_launcher.exe

# Regla para compilar y ejecutar la interacción (4_interaccion.exe)
run4: $(BIN_DIR)/4_interaccion.exe
	@echo "Ejecutando demostración de interacción..."
	cd "$(CURDIR)" && ./$(BIN_DIR)/4_interaccion.exe

# Regla para ejecutar el launcher
launcher: $(BIN_DIR)/3_launcher.exe
	@echo "Ejecutando Launcher..."
	cd "$(CURDIR)" && ./$(BIN_DIR)/3_launcher.exe

# Regla para limpiar los archivos generados
clean:
	@rm -f $(EXE_FILES)
	@echo "✓ Archivos compilados eliminados"

# Regla para ver los archivos compilables
list:
	@echo "Archivos fuente encontrados:"
	@echo $(CPP_FILES)
	@echo ""
	@echo "Ejecutables que se generarán:"
	@echo $(EXE_FILES)

# Regla para help
help:
	@echo "Comandos disponibles:"
	@echo "  make all          - Compila todos los archivos .cpp"
	@echo "  make run0         - Compila y ejecuta 0_Ventana.exe"
	@echo "  make run1         - Compila y ejecuta 1_mapa.exe"
	@echo "  make run2         - Compila y ejecuta 2_visualizacion_unidades.exe"
	@echo "  make runmap       - Compila y ejecuta 1_mapa.exe"
	@echo "  make runwindow    - Compila y ejecuta 0_Ventana.exe"
	@echo "  make rununidades  - Compila y ejecuta 2_visualizacion_unidades.exe"
	@echo "  make clean        - Elimina todos los ejecutables"
	@echo "  make list         - Muestra los archivos fuente y ejecutables"
	@echo "  make help         - Muestra esta ayuda"

.PHONY: all clean list help
.PHONY: run% runmap runwindow
