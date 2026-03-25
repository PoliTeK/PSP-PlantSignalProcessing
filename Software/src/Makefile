# Project Name

#TARGET = PSP
TARGET = main

# Sources

CPP_SOURCES = main.cpp Display/displayHandler.cpp PSP/AudioEngine.cpp PSP/PlantConditioner.cpp ../libs/PoliTeKDSP/Utilities/DataFilter/FIIR/CapFir.cpp ../libs/PoliTeKDSP/Utilities/DataFilter/IIR/iir.cpp ../libs/PoliTeKDSP/Utilities/DataFilter/MF/MF.cpp ../libs/PoliTeKDSP/Effects/AnalogDelay/src/AnalogDelay.cpp 



# Library Locations
LIBDAISY_DIR = ../libs/PoliTeKDSP/libs/libDaisy
DAISYSP_DIR = ../libs/PoliTeKDSP/libs/DaisySP

LDFLAGS = -u _printf_float
CFLAGS += -DDEBUG_MODE
CPPFLAGS += -DDEBUG_MODE

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
