# Project Name
TARGET = Name

# Sources (inserisci tutti i file .cpp da compilare)
CPP_SOURCES = src/main.cpp \
				$(CLASSES_DIR)/Effect/effectWrapper.cpp \
				$(CLASSES_DIR)/Effect/sigChain_handler.cpp \
				$(CLASSES_DIR)/Effect/effectInterface.cpp \
				src/AnalogDelay.cpp


# Library Locations
LIBDAISY_DIR = ../../../libs/libDaisy
DAISYSP_DIR = ../../../libs/DaisySP

# Include directories
C_INCLUDES += -I.

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

include $(SYSTEM_FILES_DIR)/Makefile