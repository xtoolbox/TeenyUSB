TARGET ?= f072comp
BOARD ?= stm32f072c8t6

BOARD_PATH = ../boards/$(BOARD)
include $(BOARD_PATH)/board.mk

# TeenyDT format descriptor
TEENYDT = composite_desc.lua
USBMEM = 512
USBEP = 5
# project source
SOURCES  = composite_device.c \
           teeny_usb_desc.c
INCLUDES = . 
DEFINES  = __forceinline=inline NO_HOST MSC_DATA_PACKET_LENGTH=2048
# board source
SOURCES  += $(BOARD_SRC)
INCLUDES += $(BOARD_INC)
DEFINES  += $(BOARD_DEF)

# Teeny USB source
TUSB_PATH = ../../core
SOURCES  += $(wildcard $(TUSB_PATH)/*.c)
INCLUDES += $(TUSB_PATH)
INCLUDES += $(TUSB_PATH)/../driver_stm32
INCLUDES += $(TUSB_PATH)/../class
INCLUDES += $(TUSB_PATH)/../class/msc
INCLUDES += $(TUSB_PATH)/../class/cdc
INCLUDES += $(TUSB_PATH)/../class/hid
INCLUDES += $(TUSB_PATH)/../class/user
INCLUDES += $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Include

# Teeny USB device class source
SOURCES  += $(wildcard $(TUSB_PATH)/../class/*.c)
SOURCES  += $(wildcard $(TUSB_PATH)/../class/msc/*.c)
SOURCES  += $(wildcard $(TUSB_PATH)/../class/cdc/*.c)
SOURCES  += $(wildcard $(TUSB_PATH)/../class/hid/*.c)
SOURCES  += $(wildcard $(TUSB_PATH)/../class/user/*.c)

# MCU driver source
SOURCES += $(wildcard $(TUSB_PATH)/../driver_stm32/*.c)

TOOLSET     ?= arm-none-eabi-
CC           = $(TOOLSET)gcc
LD           = $(TOOLSET)gcc
AR           = $(TOOLSET)ar
AS           = $(TOOLSET)as
SIZE         = $(TOOLSET)size
OBJCOPY      = $(TOOLSET)objcopy
RM           = rm -rf
CP           = cp
MKDIR        = mkdir -p

CPU ?= cortex-m3
CCFLAGS     += -Wall -mcpu=$(CPU) -mthumb -g -Os -MMD -Wno-attributes
CCFLAGS     += $(addprefix -D, $(DEFINES)) 
CCFLAGS     += $(addprefix -I, $(INCLUDES))
CCFLAGS     += -ffunction-sections -fdata-sections -specs=nano.specs -specs=nosys.specs
LDFLAGS     += $(CCFLAGS)
LDFLAGS     += -Wl,--script='$(LDSCRIPT)' -Wl,-Map=$(OUTDIR)/$(TARGET).map
LDFLAGS     += -Wl,--gc-sections

OUTDIR = ./output
OBJDIR = ./obj/$(TARGET)
OBJECTS      = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(notdir $(basename $(SOURCES)))))
DEPENDS      = $(addprefix $(OBJDIR)/, $(addsuffix .d, $(notdir $(basename $(filter %c, $(SOURCES))))))
SRCPATH      = $(sort $(dir $(SOURCES)))
vpath %.c $(SRCPATH)
vpath %.S $(SRCPATH)
vpath %.s $(SRCPATH)

all: f072 f103 f303 f407 f723 f767 h743

f072:
	@echo Building stm32f072c8t6
	@$(MAKE) demo TARGET='f072comp' BOARD='stm32f072c8t6'
f103:
	@echo Building stm32f103ret6
	@$(MAKE) demo TARGET='f103comp' BOARD='stm32f103ret6'
f303:
	@echo Building stm32f303vct6
	@$(MAKE) demo TARGET='f303comp' BOARD='stm32f3_disco'
f407:
	@echo Building stm32f407igt6
	@$(MAKE) demo TARGET='f407comp' BOARD='stm32f407_evk'
f723:
	@echo Building stm32f723iek6
	@$(MAKE) demo TARGET='f723comp' BOARD='stm32f723e_disco'
f767:
	@echo Building stm32f767zit6
	@$(MAKE) demo TARGET='f767comp' BOARD='stm32f767zi_nucleo'
h743:
	@echo Building stm32h743vit6
	@$(MAKE) demo TARGET='h743comp' BOARD='stm32h743_openmv'

demo: $(OUTDIR) $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).hex

$(OBJDIR)/%.o:%.c
	@echo building $<
	@$(CC) $(CCFLAGS) -c $< -o $@
$(OBJDIR)/%.o:%.s
	@echo building $<
	@$(CC) $(ASFLAGS) -c $< -o $@
$(OBJDIR)/%.o:%.S
	@echo building $<
	@$(CC) $(ASFLAGS) -c $< -o $@

$(OBJDIR):
	@$(MKDIR) $@
$(OUTDIR):
	@$(MKDIR) $@

$(OUTDIR)/$(TARGET).hex:$(OUTDIR)/$(TARGET).elf
	@echo generating $@
	@$(OBJCOPY) -Oihex $< $@
$(OUTDIR)/$(TARGET).bin:$(OUTDIR)/$(TARGET).elf
	@echo generating $@
	@$(OBJCOPY) -Obinary $< $@
$(OUTDIR)/$(TARGET).elf: $(OBJDIR) $(OBJECTS)
	@echo linking $@
	@$(LD) $(LDFLAGS) $(OBJECTS) -o $@
	@$(SIZE) $@

USBMEM ?= 512
USBEP  ?= 7

desc: teeny_usb_desc.c

PWD = $(shell pwd)
teeny_usb_desc.h teeny_usb_desc.c:$(TEENYDT)
	@echo generate USB descriptor
	cd $(PWD)/../../TeenyDT && \
	lua gen_descriptor.lua $(PWD)/$< -maxmem=$(USBMEM) -maxep=$(USBEP) && \
	cd $(PWD)

clean:
	$(RM) $(OUTDIR)/* $(OBJDIR)/*
clear:
	$(RM) $(OUTDIR) obj

.PHONY: all clean clear

-include $(DEPENDS)
