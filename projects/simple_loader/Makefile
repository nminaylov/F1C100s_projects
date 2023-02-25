#
# Project makefile
#

PROJECT_NAME = simple_loader

PROJROOT := ../..

# link file
LINK_SCRIPT = $(PROJROOT)/f1c100s/f1c100s_sram.ld

# source files
SRCS += $(wildcard *.c)
	
INCLUDES += -I.

# Define optimisation level here
OPT = -Os

DEFS += -DEGON_HEADER # add eGON.BT0 header

# DRAM size: 32M for F1C100s, 64M for F1C200s
DRAM_SIZE = 32M

include $(PROJROOT)/f1c100s_common.mk
