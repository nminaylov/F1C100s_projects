#
# Project makefile
#

PROJECT_NAME = hello_led

PROJROOT := ../..

# link file
LINK_SCRIPT = $(PROJROOT)/f1c100s/f1c100s_dram.ld

# source files
SRCS += $(wildcard *.c)
	
INCLUDES += -I.

# Define optimisation level here
OPT = -Os

DEFS += -DLOAD_HEADER # add header for simple_loader

# DRAM size: 32M for F1C100s, 64M for F1C200s
DRAM_SIZE = 32M

include $(PROJROOT)/f1c100s_common.mk
