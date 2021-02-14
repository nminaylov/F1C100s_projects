#
# Machine makefile
#

BUILDDIR = build
	
# toolchain
TOOLCHAIN    = arm-none-eabi-
CC           = $(TOOLCHAIN)gcc
CP           = $(TOOLCHAIN)objcopy
SZ           = $(TOOLCHAIN)size
AS           = $(TOOLCHAIN)gcc -x assembler-with-cpp

# user specific
SRCS += \
	../_drivers_/src/f1c100s_pwm.c \
	../_drivers_/src/f1c100s_timer.c \
	../_drivers_/src/f1c100s_de.c \
	../_drivers_/src/f1c100s_clock.c \
	../_drivers_/src/f1c100s_gpio.c \
	../_drivers_/src/f1c100s_intc.c \
	../_drivers_/src/f1c100s_uart.c \
	../_lib_/src/printf.c \
	../_lib_/src/syscalls.c
	
SRCS += \
	../_arm926_/src/start.S \
	../_arm926_/src/cache-v5.S
	

# include
LIBS 		+= -lgcc -lm -lc_nano 
INCLUDES	+= -I../_arm926_/inc -I../_drivers_/inc -I../_lib_/inc

ELF = $(BUILDDIR)/$(PROJECT_NAME).elf
BIN = $(BUILDDIR)/$(PROJECT_NAME).bin

# defines
DEFS += -D__ARM32_ARCH__=5 -D__ARM926EJS__

OBJS := $(addprefix $(BUILDDIR)/,$(notdir $(SRCS:.c=.o)))
OBJS := $(addprefix $(BUILDDIR)/,$(notdir $(OBJS:.S=.o)))
vpath %.c $(sort $(dir $(SRCS)))
vpath %.S $(sort $(dir $(SRCS)))

MC_FLAGS = -march=armv5te -mtune=arm926ej-s -mfloat-abi=soft -marm -mno-thumb-interwork -g -ggdb -Wall -O0 -fdata-sections -ffunction-sections -ffreestanding -std=gnu99

ASFLAGS += $(MC_FLAGS) $(DEFS)
CFLAGS += $(MC_FLAGS) $(OPT) -fomit-frame-pointer -Wall -fverbose-asm $(DEFS)
LDFLAGS += -nostartfiles -Xlinker --gc-sections -T$(LINK_SCRIPT) -Wl,-Map=$(BUILDDIR)/$(PROJECT_NAME).map,--cref,--no-warn-mismatch

#
# makefile rules
#
$(BIN): $(ELF)
	$(CP) -O binary $< $@
	@echo   ---------------------------------------------------
	@$(SZ) $(ELF)

$(ELF): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(BUILDDIR)/%.o: %.c
	"mkdir" -p $(dir $@)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILDDIR)/%.o: %.S
	$(CC) -c $(ASFLAGS) $(INCLUDES) $< -o $@

size:
	@echo   ---------------------------------------------------
	@$(SZ) $(ELF)

clean:
	rm -rf $(BUILDDIR)/
