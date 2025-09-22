# Project Name

TARGET = main
#TARGET = main

# Sources

CPP_SOURCES = main.cpp ../../Classes/FIIR/CapFir.cpp ../../Classes/Temperamento/PlantConditioner.cpp


# Library Locations
LIBDAISY_DIR = ../../libs/libDaisy
DAISYSP_DIR = ../../libs/DaisySP

LDFLAGS = -u _printf_float

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
