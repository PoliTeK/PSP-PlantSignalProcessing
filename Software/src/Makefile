# Nome del progetto (questo sarà il nome del file .bin finale)
TARGET = PlantSynth
OPT = -Os
#APP_TYPE = BOOT_QSPI

# Flag di ottimizzazione e riduzione dimensione
C_FLAGS += -flto
CXX_FLAGS += -flto -fno-exceptions -fno-rtti
LDFLAGS += -flto

# I file sorgente C++ del tuo progetto
CPP_SOURCES = \
main.cpp \
Display/displayHandler.cpp \
Display/MenuManager.cpp \
PSP/PlantConditioner.cpp \
PSP/AudioEngine.cpp \
../libs/PoliTeKDSP/Utilities/DataFilter/MF/MF.cpp \
../libs/PoliTeKDSP/Utilities/DataFilter/IIR/iir.cpp \
../libs/PoliTeKDSP/Oscillators/oscillator.cpp \
../libs/PoliTeKDSP/Effects/AnalogDelay/src/AnalogDelay.cpp\
../libs/PoliTeKDSP/Modulators/XorM/XorM.cpp

# Percorsi alle librerie
LIBDAISY_DIR = ../libs/PoliTeKDSP/libs/libDaisy
DAISYSP_DIR = ../libs/PoliTeKDSP/libs/DaisySP

USE_DAISYSP_LGPL = 1

# Percorso ai file di sistema e inclusione del Makefile core di Daisy
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile