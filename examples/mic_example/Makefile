export MCU = stm32wb55xx
export SEAMOS_LIB = $(PWD)/SeamOS.a
export CONFIG = $(PWD)/config
SEAMOS_PATH = ../../
GCC = $(COMPILER)gcc
LD = $(COMPILER)ld
OBJCOPY = $(COMPILER)objcopy
LINKER_SCRIPT = linker.ld

include $(SEAMOS_PATH)/mcu/$(MCU)/config.mk

INCLUDE = -I$(SEAMOS_PATH) -I$(SEAMOS_PATH)/mcu/$(MCU)/include -I$(PWD)
CFLAGS ?= -Wall -Werror -c -ffreestanding -nostdlib $(MCU_CFLAGS) $(INCLUDE) -g3 \
          -DLOG_LEVEL=$(CONFIG_LOG_LEVEL) -MMD -MF $(DEPDIR)/$*.d
LDFLAGS ?= --omagic -static

DEPDIR = .deps/

MIC_EXAMPLE_SOURCE = $(wildcard *.c)
MIC_EXAMPLE_OBJECTS = $(patsubst %.c,%.o,$(MIC_EXAMPLE_SOURCE))

DEPENDS = $(patsubst %.c,$(DEPDIR)/%.d,$(MIC_EXAMPLE_SOURCE))

.SECONDARY: mic_example.elf $(MIC_EXAMPLE_OBJECTS)
all: mic_example.bin

.PHONY: $(SEAMOS_LIB)
$(SEAMOS_LIB):
	$(MAKE) -C $(SEAMOS_PATH)

%.d:
	@mkdir -p $(@D)

%.o: %.c Makefile
	$(GCC) $(CFLAGS) -o $@ $<

%.elf: $(SEAMOS_LIB) $(MIC_EXAMPLE_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $(MIC_EXAMPLE_OBJECTS) $(SEAMOS_LIB)

%.bin: %.elf
	$(OBJCOPY) $^ -O binary $@

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(MIC_EXAMPLE_OBJECTS) *.bin *.elf
	$(MAKE) -C $(SEAMOS_PATH) clean

.PHONY: flash
flash:
	dfu-util -a 0 -i 0 -s 0x08000000:leave -D mic_example.bin

include $(DEPENDS)
