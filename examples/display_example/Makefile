export MCU = stm32wb55xx
export SEAMOS_LIB = $(PWD)/SeamOS.a
export CONFIG = $(PWD)/config.h
SEAMOS_PATH = $(PWD)/../../
GCC = $(COMPILER)gcc
LD = $(COMPILER)ld
OBJCOPY = $(COMPILER)objcopy
LINKER_SCRIPT = linker.ld

include $(SEAMOS_PATH)/mcu/$(MCU)/config.mk

INCLUDE = -I$(SEAMOS_PATH) -I$(SEAMOS_PATH)/mcu/$(MCU)/include -I$(PWD)
CFLAGS ?= -Wall -Werror -c -ffreestanding -nostdlib $(MCU_CFLAGS) $(INCLUDE) -g3 \
          -MMD -MF $(DEPDIR)/$*.d
LDFLAGS ?= --omagic -static

DEPDIR = .deps/

export SEAMOS_DRIVER_SOURCE = $(SEAMOS_PATH)/drivers/serial/log.c \
                              $(SEAMOS_PATH)/drivers/display/st7789.c
export SEAMOS_MCU_INTERFACES_SOURCE = $(SEAMOS_PATH)/mcu/$(MCU)/interfaces/gpio.c \
                                      $(SEAMOS_PATH)/mcu/$(MCU)/interfaces/uart.c \
                                      $(SEAMOS_PATH)/mcu/$(MCU)/interfaces/system_timer.c \
                                      $(SEAMOS_PATH)/mcu/$(MCU)/interfaces/spi.c \
                                      $(SEAMOS_PATH)/mcu/$(MCU)/interfaces/dma.c

DISPLAY_EXAMPLE_SOURCE = $(wildcard applications/*.c) \
                       $(wildcard init/*.c)
DISPLAY_EXAMPLE_OBJECTS = $(patsubst %.c,%.o,$(DISPLAY_EXAMPLE_SOURCE))

DEPENDS = $(patsubst %.c,$(DEPDIR)/%.d,$(DISPLAY_EXAMPLE_SOURCE))

.SECONDARY: display_example.elf $(DISPLAY_EXAMPLE_OBJECTS)
all: display_example.bin

.PHONY: $(SEAMOS_LIB)
$(SEAMOS_LIB):
	$(MAKE) -C $(SEAMOS_PATH)

%.d:
	@mkdir -p $(@D)

%.o: %.c Makefile
	$(GCC) $(CFLAGS) -o $@ $<

%.elf: $(SEAMOS_LIB) $(DISPLAY_EXAMPLE_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $(DISPLAY_EXAMPLE_OBJECTS) $(SEAMOS_LIB)

%.bin: %.elf
	$(OBJCOPY) $^ -O binary $@

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(DISPLAY_EXAMPLE_OBJECTS) *.bin *.elf
	$(MAKE) -C $(SEAMOS_PATH) clean

.PHONY: flash
flash:
	dfu-util -a 0 -i 0 -s 0x08000000:leave -D display_example.bin

include $(DEPENDS)
