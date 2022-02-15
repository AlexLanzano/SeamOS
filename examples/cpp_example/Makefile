export MCU = stm32wb55xx
export SEAMOS_LIB = $(PWD)/SeamOS.a
export CONFIG = $(PWD)/config
SEAMOS_PATH = ../../
GCC = $(COMPILER)gcc
GPP = $(COMPILER)g++
LD = $(COMPILER)ld
OBJCOPY = $(COMPILER)objcopy
LINKER_SCRIPT = linker.ld

include $(SEAMOS_PATH)/mcu/$(MCU)/config.mk

INCLUDE = -I$(SEAMOS_PATH) -I$(SEAMOS_PATH)/mcu/$(MCU)/include -I$(PWD)
CPPFLAGS ?= -Wall -Werror -c -fno-exceptions -ffreestanding -nostdlib $(MCU_CFLAGS) $(INCLUDE) -g \
          -MMD -MF $(DEPDIR)/$*.d
LDFLAGS ?= --omagic -static

DEPDIR = .deps/

CPP_EXAMPLE_SOURCE = $(wildcard applications/*.cpp) \
                     $(wildcard *.cpp)
CPP_EXAMPLE_OBJECTS = $(patsubst %.cpp,%.o,$(CPP_EXAMPLE_SOURCE))

DEPENDS = $(patsubst %.cpp,$(DEPDIR)/%.d,$(CPP_EXAMPLE_SOURCE))

.SECONDARY: cpp_example.elf $(CPP_EXAMPLE_OBJECTS)
all: cpp_example.bin

.PHONY: $(SEAMOS_LIB)
$(SEAMOS_LIB):
	$(MAKE) -C $(SEAMOS_PATH)

%.d:
	@mkdir -p $(@D)

%.o: %.cpp Makefile
	$(GPP) $(CPPFLAGS) -o $@ $<

%.elf: $(SEAMOS_LIB) $(CPP_EXAMPLE_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $(CPP_EXAMPLE_OBJECTS) $(SEAMOS_LIB)

%.bin: %.elf
	$(OBJCOPY) $^ -O binary $@

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(CPP_EXAMPLE_OBJECTS) *.bin *.elf
	$(MAKE) -C $(SEAMOS_PATH) clean

.PHONY: flash
flash:
	dfu-util -a 0 -i 0 -s 0x08000000:leave -D cpp_example.bin

include $(DEPENDS)
