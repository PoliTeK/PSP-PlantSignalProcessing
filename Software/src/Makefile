# Project Name

TARGET = PSP
#TARGET = main

# Sources

CPP_SOURCES = PSP.cpp classes/ControlPanel.cpp ../Classes/SensorFilters/FIIR/CapFir.cpp ../Classes/SensorFilters/IIR/iir.cpp ../Classes/SensorFilters/MF/MF.cpp classes/PlantConditioner.cpp  ../Classes/Effects/AnalogDelay/src/AnalogDelay.cpp


# Library Locations
LIBDAISY_DIR = ../libs/libDaisy
DAISYSP_DIR = ../libs/DaisySP

LDFLAGS = -u _printf_float
CFLAGS += -DDEBUG_MODE
CPPFLAGS += -DDEBUG_MODE

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
