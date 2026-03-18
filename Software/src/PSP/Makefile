# Nome del file binario finale
TARGET = main

# File sorgenti da compilare
CPP_SOURCES = \
main.cpp \
PlantConditioner.cpp \
AudioEngine.cpp \
../../libs/PoliTeKDSP/Utilities/DataFilter/IIR/iir.cpp \
../../libs/PoliTeKDSP/Utilities/DataFilter/MF/MF.cpp

# Percorsi delle librerie standard di libDaisy e DaisySP
LIBDAISY_DIR = ../../libs/PoliTeKDSP/libs/libDaisy
DAISYSP_DIR = ../../libs/PoliTeKDSP/libs/DaisySP

# Flag del linker per abilitare la stampa dei float (utile se aggiungi log via seriale)
LDFLAGS = -u _printf_float

# Inclusione del Makefile generico di libDaisy (Core)
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile