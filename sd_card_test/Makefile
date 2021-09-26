#
# Project makefile
#

PROJECT_NAME = sd_card_test

# link file
LINK_SCRIPT = ../f1c100s_dram.ld

# source files
SRCS += \
	src/system.c \
	src/exception.c \
	src/main.c \
	src/display.c \
	src/sdcard.c \
	src/diskio.c \
	fatfs/ff.c \
	
INCLUDES += -I. -Iinc -Ifatfs

# Define optimisation level here
OPT = -Og

DEFS += -DLOAD_HEADER # add header for simple_loader

include ../f1c100s_common.mk
