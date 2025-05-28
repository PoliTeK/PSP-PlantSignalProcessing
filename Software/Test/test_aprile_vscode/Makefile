# Project Name
TARGET = test_aprile_vscode

# Sources
Cmake
PP_SOURCES = main.cpp AnalogDelay.cpp pentaPianta.cpp

# Library Locations
LIBDAISY_DIR = ../../../libs/libDaisy/
DAISYSP_DIR = ../../../libs/DaisySP/

# Flag for printing floats in serial 
LDFLAGS = -u _printf_float


# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
