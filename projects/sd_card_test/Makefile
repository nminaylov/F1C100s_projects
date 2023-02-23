#
# Project makefile
#

PROJECT_NAME = sd_card_test

PROJROOT := ../..

# link file
LINK_SCRIPT = $(PROJROOT)/f1c100s/f1c100s_dram.ld

# source files
SRCS += \
	$(wildcard *.c) \
	fatfs/ff.c \

SRCS += display/display.c
# SRCS += display/display_7010b.c	
	
INCLUDES += -I. -Idisplay -Ifatfs

# Define optimisation level here
OPT = -Og

DEFS += -DLOAD_HEADER # add header for simple_loader

include $(PROJROOT)/f1c100s_common.mk
