#
# Project makefile
#

PROJECT_NAME = lvgl_demo

PROJROOT := ../..

# link file
LINK_SCRIPT = $(PROJROOT)/f1c100s/f1c100s_dram.ld

# source files
SRCS += $(wildcard *.c)
	
INCLUDES += -I.
INCLUDES += -Ilvgl

# Define optimisation level here
OPT = -Os

# include sub makefiles
LVGL_DIR		= .
LVGL_DIR_NAME	= lvgl
include lvgl/lvgl.mk

SRCS += $(CSRCS)

DEFS += -DLOAD_HEADER # add header for simple_loader

include $(PROJROOT)/f1c100s_common.mk
