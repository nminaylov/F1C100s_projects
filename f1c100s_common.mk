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
OPENOCD 	 = openocd

OPENOCD_ADAPTER = interface/ftdi/digilent-hs1.cfg
OPENOCD_OPTS = -f $(OPENOCD_ADAPTER) -f $(PROJROOT)/allwinner_f1c100s.cfg -c "init"

# user specific
SRCS += \
	$(PROJROOT)/f1c100s/arm926/src/image.S \
	$(PROJROOT)/f1c100s/arm926/src/vectors.S \
	$(PROJROOT)/f1c100s/arm926/src/cache-v5.S

SRCS += $(wildcard $(PROJROOT)/f1c100s/drivers/src/*.c)

SRCS += $(PROJROOT)/lib/printf/printf.c \
		$(PROJROOT)/lib/syscalls/syscalls.c

# include
LIBS 		+= -lgcc -lm -lc_nano 
INCLUDES	+= -I$(PROJROOT)/f1c100s/arm926/inc -I$(PROJROOT)/f1c100s/drivers/inc -I$(PROJROOT)/lib/printf

ELF = $(BUILDDIR)/$(PROJECT_NAME).elf
BIN = $(BUILDDIR)/$(PROJECT_NAME).bin

# defines
DEFS += -D__ARM32_ARCH__=5 -D__ARM926EJS__

DEFS += -DPRINTF_ALIAS_STANDARD_FUNCTION_NAMES=1 -DPRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD=1

OBJS := $(addprefix $(BUILDDIR)/,$(notdir $(SRCS:.c=.o)))
OBJS := $(addprefix $(BUILDDIR)/,$(notdir $(OBJS:.S=.o)))
vpath %.c $(sort $(dir $(SRCS)))
vpath %.S $(sort $(dir $(SRCS)))

MC_FLAGS = -march=armv5te -mtune=arm926ej-s -mfloat-abi=soft -marm -mno-thumb-interwork -g -ggdb -Wall -fdata-sections -ffunction-sections -ffreestanding -std=gnu99

ASFLAGS += $(MC_FLAGS) $(DEFS)
CFLAGS += $(MC_FLAGS) $(OPT) -fomit-frame-pointer -Wall -fverbose-asm $(DEFS)
LDFLAGS += -nostartfiles -Xlinker --gc-sections -T$(LINK_SCRIPT) -Wl,--defsym=DRAM_SIZE=$(DRAM_SIZE),-Map=$(BUILDDIR)/$(PROJECT_NAME).map,--cref,--no-warn-mismatch

#
# makefile rules
#
$(BIN): $(ELF)
	$(CP) -O binary $< $@
	@$(SZ) $(ELF)

$(ELF): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(BUILDDIR)/%.o: %.c
	"mkdir" -p $(dir $@)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILDDIR)/%.o: %.S
	$(CC) -c $(ASFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(BUILDDIR)/

# Start openocd
openocd_start:
	$(OPENOCD) $(OPENOCD_OPTS)

# Perform a reset using watchdog
reset:
	$(OPENOCD) $(OPENOCD_OPTS) -c "halt" -c "mww 0x01C20CB4 0x1" -c "mww 0x01C20CB8 0x1" -c "resume" -c "sleep 3000" -c "exit"

# Load and execute code in SRAM
load_sram: $(BIN)
	$(OPENOCD) $(OPENOCD_OPTS) -c "halt" -c "load_image $(BIN) 0x00000000" -c "reg pc 0x00000000" -c "resume" -c "exit"

# Load and execute code in DRAM (DRAM should be initialized first!)
load_dram: $(BIN)
	$(OPENOCD) $(OPENOCD_OPTS) -c "halt" -c "load_image $(BIN) 0x80000000" -c "reg pc 0x80000000" -c "resume" -c "exit"

# Load and execute DRAM initialization code (should be built first)
dram_init: 
	$(OPENOCD) $(OPENOCD_OPTS) -c "halt" -c "load_image $(PROJROOT)/projects/dram_init/build/dram_init.bin 0x00000000" -c "reg pc 0x00000000" -c "resume" -c "exit"

# Reset SOC, initialize DRAM and load code
# Works only if there is no bootable code in spi flash
load: reset dram_init load_dram
