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
CFLAGS ?= -Wall -Werror -c -ffreestanding -nostdlib $(MCU_CFLAGS) $(INCLUDE) \
          -MMD -MF $(DEPDIR)/$*.d
LDFLAGS ?= --omagic -static

DEPDIR = .deps/

BLINK_EXAMPLE_SOURCE = $(wildcard applications/*.c) \
                       $(wildcard init/*.c)
BLINK_EXAMPLE_OBJECTS = $(patsubst %.c,%.o,$(BLINK_EXAMPLE_SOURCE))

DEPENDS = $(patsubst %.c,$(DEPDIR)/%.d,$(BLINK_EXAMPLE_SOURCE))

.SECONDARY: blink_example.elf $(BLINK_EXAMPLE_OBJECTS)
all: blink_example.bin

.PHONY: $(SEAMOS_LIB)
$(SEAMOS_LIB):
	$(MAKE) -C $(SEAMOS_PATH)

%.d:
	@mkdir -p $(@D)

%.o: %.c Makefile
	$(GCC) $(CFLAGS) -o $@ $<

%.elf: $(SEAMOS_LIB) $(BLINK_EXAMPLE_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $(BLINK_EXAMPLE_OBJECTS) $(SEAMOS_LIB)

%.bin: %.elf
	$(OBJCOPY) $^ -O binary $@

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(BLINK_EXAMPLE_OBJECTS) *.bin *.elf
	$(MAKE) -C $(SEAMOS_PATH) clean

.PHONY: flash
flash:
	dfu-util -a 0 -i 0 -s 0x08000000:leave -D blink_example.bin

include $(DEPENDS)
