# This is a Makefile for compiling AVR projects and deploying them with avr-dude.
# This Makefile was partly inspired/stolen from: https://ece-classes.usc.edu/ee459/library/samples/AVR/Makefile-ATmega328P

###############################################################################
# CONFIG
###############################################################################

DEVICE      = atmega328p
CLOCK       = 8000000
PROGRAMMER  = -c usbtiny -P usb
FUSES       = -U hfuse:w:0xd9:m -U lfuse:w:0xe2:m

SOURCE_DIR  = src
OBJECT_DIR  = build
OUT_NAME = main

# Fuse Low Byte = 0xe2   Fuse High Byte = 0xd9   Fuse Extended Byte = 0xff
# Bit 7: CKDIV8  = 1     Bit 7: RSTDISBL  = 1    Bit 7:
#     6: CKOUT   = 1         6: DWEN      = 1        6:
#     5: SUT1    = 1         5: SPIEN     = 0        5:
#     4: SUT0    = 0         4: WDTON     = 1        4:
#     3: CKSEL3  = 0         3: EESAVE    = 1        3:
#     2: CKSEL2  = 0         2: BOOTSIZ1  = 0        2: BODLEVEL2 = 1
#     1: CKSEL1  = 1         1: BOOTSIZ0  = 0        1: BODLEVEL1 = 1
#     0: CKSEL0  = 0         0: BOOTRST   = 1        0: BODLEVEL0 = 1
# Calibrated clock source, start-up time = 14 clks + 65ms
# Don't output clock on PORTB0, don't divide clock by 8,
# Boot reset vector disabled, boot flash size 2048 bytes,
# Preserve EEPROM disabled, watch-dog timer off
# Serial program downloading enabled, debug wire disabled,
# Reset enabled, brown-out detection disabled

###############################################################################

C_SOURCE := $(shell find $(SOURCE_DIR) -name '*.c')
ASM_SOURCE := $(shell find $(SOURCE_DIR) -name '*.S')
C_OBJECTS := $(patsubst $(SOURCE_DIR)/%.c, $(OBJECT_DIR)/%.o, $(C_SOURCE))
ASM_OBJECTS := $(patsubst $(SOURCE_DIR)/%.S, $(OBJECT_DIR)/%.o, $(ASM_SOURCE))
ALL_OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)
DEPS := $(ALL_OBJECTS:.o=.d)

OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -MMD -MP -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# symbolic targets:
all: $(OUT_NAME).hex

$(C_OBJECTS): $(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c Makefile
	$(COMPILE) -c $< -o $@

$(ASM_OBJECTS): $(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.S Makefile
	$(COMPILE) -dD -c $< -o $@

$(OBJECT_DIR):
	@ mkdir -p $(OBJECT_DIR)

verify:
	sudo $(AVRDUDE) -n

flash:	all
	sudo $(AVRDUDE) -U flash:w:$(OUT_NAME).hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID $(OUT_NAME).hex

clean:
	rm -fr $(OUT_NAME).hex $(OUT_NAME).elf $(OBJECT_DIR)

# file targets:
$(OUT_NAME).elf: $(OBJECT_DIR) $(OBJECTS)
	$(COMPILE) -o $(OUT_NAME).elf $(OBJECTS)

$(OUT_NAME).hex: $(OUT_NAME).elf
	rm -f $(OUT_NAME).hex
	avr-objcopy -j .text -j .data -O ihex $(OUT_NAME).elf $(OUT_NAME).hex
	avr-size --format=avr --mcu=$(DEVICE) $(OUT_NAME).elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	$(OUT_NAME).elf
	avr-objdump -d $(OUT_NAME).elf

-include $(DEPS)
