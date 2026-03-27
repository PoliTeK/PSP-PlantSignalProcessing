# Nome del progetto (questo sarà il nome del file .bin finale)
TARGET = PlantSynth

# I file sorgente C++ del tuo progetto
# (NESSUN file di libDaisy o DaisySP deve essere inserito qui)
CPP_SOURCES = \
main.cpp \
Display/displayHandler.cpp \
Display/MenuManager.cpp \
PSP/PlantConditioner.cpp \
PSP/AudioEngine.cpp\
../libs/PoliTeKDSP/Utilities/DataFilter/MF/MF.cpp \
../libs/PoliTeKDSP/Utilities/DataFilter/IIR/iir.cpp



# Percorsi alle librerie (basati sui tuoi #include)
LIBDAISY_DIR = ../libs/libDaisy
DAISYSP_DIR = ../libs/DaisySP

USE_DAISYSP_LGPL = 1

# Percorso ai file di sistema e inclusione del Makefile core di Daisy
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile