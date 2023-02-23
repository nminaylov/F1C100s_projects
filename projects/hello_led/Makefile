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

include $(PROJROOT)/f1c100s_common.mk
