#
# Project makefile
#

PROJECT_NAME = hello_led

# link file
LINK_SCRIPT = ../f1c100s_dram.ld

# source files
SRCS += \
	src/system.c \
	src/exception.c \
	src/main.c 
	
INCLUDES += -I. -Iinc

# Define optimisation level here
OPT = -Os

DEFS += -DLOAD_HEADER # add header for simple_loader

include ../f1c100s_common.mk
