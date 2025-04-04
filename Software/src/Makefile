# Project Name
TARGET = Ramo

# Sources (inserisci tutti i file .cpp da compilare)
CPP_SOURCES = Classes/Ramo.cpp  Classes/main.cpp 



# Library Locations
LIBDAISY_DIR = ../libs/libDaisy
DAISYSP_DIR = ../libs/DaisySP

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