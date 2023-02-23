#
# Project makefile
#

PROJECT_NAME = lcd_test

PROJROOT := ../..

# link file
LINK_SCRIPT = $(PROJROOT)/f1c100s/f1c100s_dram.ld

# source files
SRCS += $(wildcard src/*.c)
	
INCLUDES += -I. -Iinc

# Define optimisation level here
OPT = -Og

DEFS += -DLOAD_HEADER # add header for simple_loader

include $(PROJROOT)/f1c100s_common.mk
