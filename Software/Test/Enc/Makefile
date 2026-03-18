# Nome del file binario finale
TARGET = test_encoder

# File sorgente (solo il main di test per ora)
CPP_SOURCES = main.cpp

# Percorsi delle librerie (Verifica che siano corretti rispetto alla tua cartella!)
LIBDAISY_DIR = ../../libs/libDaisy
DAISYSP_DIR = ../../libs/DaisySP

# Flag per abilitare la stampa dei float (necessario per test_delta e test_curve)
LDFLAGS = -u _printf_float

# Core location e inclusione del Makefile generico di libDaisy
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile