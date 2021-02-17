#
# Project makefile
#

PROJECT_NAME = simple_loader

# link file
LINK_SCRIPT = ../f1c100s_sram.ld

# source files
SRCS += \
	src/system.c \
	src/main.c \
	src/sys_dram.c \
	src/spi_flash.c
	
INCLUDES += -I. -Iinc

# Define optimisation level here
OPT = -O0

DEFS += -DEGON_HEADER # add eGON.BT0 header

include ../f1c100s_common.mk
