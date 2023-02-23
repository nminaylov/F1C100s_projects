#
# Project makefile
#

PROJECT_NAME = simple_loader

PROJROOT := ../..

# link file
LINK_SCRIPT = $(PROJROOT)/f1c100s/f1c100s_sram.ld

# source files
SRCS += $(wildcard src/*.c)
	
INCLUDES += -I. -Iinc

# Define optimisation level here
OPT = -O0

DEFS += -DEGON_HEADER # add eGON.BT0 header

include $(PROJROOT)/f1c100s_common.mk
