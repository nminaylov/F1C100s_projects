#
# Project makefile
#

PROJECT_NAME = lcd_test

# link file
LINK_SCRIPT = ../f1c100s_dram.ld

# source files
SRCS += \
	src/system.c \
	src/exception.c \
	src/main.c \
	src/display.c \
	src/lcd.c \
	src/lcd_font.c \
	src/lcd_image.c
	
INCLUDES += -I. -Iinc

# Define optimisation level here
OPT = -Og

#DEFS += -DEGON_HEADER # add eGON.BT0 header

include ../f1c100s_common.mk
