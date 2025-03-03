# Project Name
TARGET = AnalogDelay

# Sources
CPP_SOURCES = src/main.cpp src/AnalogDelay.cpp

# Library Locations
LIBDAISY_DIR = ../libDaisy
DAISYSP_DIR = ../DaisySP

# Include directories
C_INCLUDES += -I.

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

# Build libraries if needed
.PHONY: libs
libs:
	@$(MAKE) -C $(LIBDAISY_DIR)
	@$(MAKE) -C $(DAISYSP_DIR)

# Default target
.DEFAULT_GOAL := all
all: libs
	@$(MAKE) -f $(SYSTEM_FILES_DIR)/Makefile

include $(SYSTEM_FILES_DIR)/Makefile