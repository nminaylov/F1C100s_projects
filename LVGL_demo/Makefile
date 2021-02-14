#
# Project makefile
#

PROJECT_NAME = LVGL_demo

# link file
LINK_SCRIPT = ../f1c100s_dram.ld

# source files
SRCS += \
	src/system.c \
	src/exception.c \
	src/main.c \
	src/display.c \
	src/lv_demo_widgets.c
	
INCLUDES += -I. -Iinc
INCLUDES += -Ilvgl

# Define optimisation level here
OPT = -Os

# include sub makefiles
LVGL_DIR		= .
LVGL_DIR_NAME	= lvgl
include lvgl/lvgl.mk

SRCS += $(CSRCS)

#DEFS += -DEGON_HEADER # add eGON.BT0 header

include ../f1c100s_common.mk
