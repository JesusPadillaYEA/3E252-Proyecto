# Directorios
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include

# Compilador y flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)

# Librerías SFML
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Obtener todos los archivos .cpp
CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)

# Generar nombres de ejecutables
EXE_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.exe,$(CPP_FILES))

# Regla por defecto
all: $(EXE_FILES)

# Regla para compilar
$(BIN_DIR)/%.exe: $(SRC_DIR)/%.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(SFML_LIBS)
	@echo "Compilado: $@"

# Ejecutar programas
run0: $(BIN_DIR)/0_Ventana.exe
	cmd /c bin\0_Ventana.exe

run1: $(BIN_DIR)/1_mapa.exe
	cmd /c bin\1_mapa.exe

run2: $(BIN_DIR)/2_visualizacion_unidades.exe
	cmd /c bin\2_visualizacion_unidades.exe

run3: $(BIN_DIR)/3_launcher.exe
	cmd /c bin\3_launcher.exe

run4: $(BIN_DIR)/4_interaccion.exe
	cmd /c bin\4_interaccion.exe

# Alias
runmap: run1
rununidades: run2
launcher: run3

# Limpiar
clean:
	del /Q $(BIN_DIR)\*.exe 2>nul || true

# Ayuda
help:
	@echo Comandos: make run0, run1, run2, run3, make clean

.PHONY: all clean help run0 run1 run2 run3 run4 runmap rununidades launcher

