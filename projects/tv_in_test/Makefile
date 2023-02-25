#
# Project makefile
#

PROJECT_NAME = tv_in_test

PROJROOT := ../..

# link file
LINK_SCRIPT = $(PROJROOT)/f1c100s/f1c100s_dram.ld

# source files
SRCS += $(wildcard *.c) \
		$(wildcard display/*.c)
	
INCLUDES += -I. -Idisplay

# Define optimisation level here
OPT = -O0

DEFS += -DLOAD_HEADER # add header for simple_loader

# DRAM size: 32M for F1C100s, 64M for F1C200s
DRAM_SIZE = 32M

include $(PROJROOT)/f1c100s_common.mk
