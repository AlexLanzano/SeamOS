# Build Configuration
export SEAMOS_CONFIG_PATH=$(PWD)
export SEAMOS_LIB ?= $(PWD)/SeamOS.a
SEAMOS_PATH ?= /home/alex/Repos/SeamOS
GCC_PATH = /home/alex/Repos/field-programmable-smartwatch/compiler/cross/bin/
GCC_PREFIX ?= arm-none-eabi-
GCC = $(GCC_PATH)/$(GCC_PREFIX)gcc
LD = $(GCC_PATH)/$(GCC_PREFIX)ld
AR = $(GCC_PATH)/$(GCC_PREFIX)ar
OBJCOPY = $(GCC_PATH)/$(GCC_PREFIX)objcopy
CPU = cortex-m4
MCU = stm32wb55xx
LINKER_SCRIPT = linker.ld

# SeamOS Configuration
CONFIG_LOG_LEVEL = LOG_LEVEL_DEBUG
CONFIG_CLOCK_SPEED = 16000000
