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

include $(PROJROOT)/f1c100s_common.mk
