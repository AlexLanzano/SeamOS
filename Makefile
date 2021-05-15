include $(SEAMOS_CONFIG_PATH)/config.mk

INCLUDE = -I. -Imcu/$(MCU)/include
CFLAGS = -Wall -Werror -c -ffreestanding -nostdlib -mcpu=$(CPU) $(INCLUDE) \
         -DLOG_LEVEL=$(CONFIG_LOG_LEVEL) -MMD -MF $(DEPDIR)/$*.d

DEPDIR = .deps/

SEAMOS_KERNEL_SOURCE ?= $(wildcard kernel/*/*.c)
SEAMOS_DRIVER_SOURCE ?= $(wildcard drivers/*/*.c) \
                        $(wildcard drivers/*/*/*.c)
SEAMOS_MCU_SOURCE ?= $(wildcard mcu/$(MCU)/*.c)
SEAMOS_LIBRARIES_SOURCE ?= $(wildcard libraries/*.c)

SEAMOS_OBJECTS = $(patsubst %.c,%.o,$(SEAMOS_KERNEL_SOURCE)) \
                 $(patsubst %.c,%.o,$(SEAMOS_DRIVER_SOURCE)) \
                 $(patsubst %.c,%.o,$(SEAMOS_MCU_SOURCE)) \
                 $(patsubst %.c,%.o,$(SEAMOS_LIBRARIES_SOURCE))

DEPENDS = $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_KERNEL_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_DRIVER_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_MCU_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_LIBRARIES_SOURCE))

all: $(SEAMOS_LIB)

%.d:
	@mkdir -p $(@D)

%.o: %.psfu
	$(OBJCOPY) -O elf32-littlearm -B arm -I binary $^ $@

%.o: %.c Makefile
	$(GCC) $(CFLAGS) -o $@ $<

$(SEAMOS_LIB): $(SEAMOS_OBJECTS)
	$(AR) -rc $@ $^

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(SEAMOS_OBJECTS) $(SEAMOS_LIB)

include $(DEPENDS)
