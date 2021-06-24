include mcu/$(MCU)/config.mk

GCC = $(COMPILER)gcc
AR = $(COMPILER)ar

CONFIG ?= config
SEAMOS_LIB ?= SeamOS.a

INCLUDE = -I. -Imcu/$(MCU)/include
CFLAGS = -Wall -Werror -c -ffreestanding -nostdlib $(MCU_CFLAGS) $(INCLUDE) -g \
         -MMD -MF $(DEPDIR)/$*.d

DEPDIR = .deps/

SEAMOS_KERNEL_SOURCE ?= $(wildcard kernel/*/*.c)
SEAMOS_DRIVER_SOURCE ?= $(wildcard drivers/*/*.c) \
                        $(wildcard drivers/*/*/*.c)
SEAMOS_MCU_SOURCE ?= $(wildcard mcu/$(MCU)/*.c)
SEAMOS_LIBRARIES_SOURCE ?= $(wildcard libraries/*.c)
SEAMOS_ARCH_SOURCE ?= $(wildcard arch/$(ARCH)/*.c)

SEAMOS_OBJECTS = $(patsubst %.c,%.o,$(SEAMOS_KERNEL_SOURCE)) \
                 $(patsubst %.c,%.o,$(SEAMOS_DRIVER_SOURCE)) \
                 $(patsubst %.c,%.o,$(SEAMOS_MCU_SOURCE)) \
                 $(patsubst %.c,%.o,$(SEAMOS_LIBRARIES_SOURCE)) \
                 $(patsubst %.c,%.o,$(SEAMOS_ARCH_SOURCE))

DEPENDS = $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_KERNEL_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_DRIVER_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_MCU_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_LIBRARIES_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(SEAMOS_ARCH_SOURCE)) \
          $(CONFIG)

all: config.h $(SEAMOS_LIB)

config.h: $(CONFIG)
	scripts/generate_config_header.py $(CONFIG)

%.d:
	@mkdir -p $(@D)

%.o: %.c Makefile
	$(GCC) $(CFLAGS) -o $@ $<

$(SEAMOS_LIB): $(SEAMOS_OBJECTS)
	$(AR) -rc $@ $^

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(SEAMOS_OBJECTS) $(SEAMOS_LIB) config.h

include $(DEPENDS)
