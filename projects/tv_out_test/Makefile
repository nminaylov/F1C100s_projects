#
# Project makefile
#

PROJECT_NAME = tv_out_test

PROJROOT := ../..

# link file
LINK_SCRIPT = $(PROJROOT)/f1c100s/f1c100s_dram.ld

# source files
SRCS += $(wildcard *.c) \
		$(wildcard $(PROJROOT)/lib/display_gfx/*.c) \
	
INCLUDES += -I. \
			-I$(PROJROOT)/lib/display_gfx

# Define optimisation level here
OPT = -O0

DEFS += -DLOAD_HEADER # add header for simple_loader

DEFS += -DDISPLAY_W=720 -DDISPLAY_H=480

# DRAM size: 32M for F1C100s, 64M for F1C200s
DRAM_SIZE = 32M

include $(PROJROOT)/f1c100s_common.mk
