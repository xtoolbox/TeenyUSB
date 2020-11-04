TARGET ?= f723bulk
BOARD ?= stm32f723e_disco

BOARD_PATH = ../boards/$(BOARD)
include $(BOARD_PATH)/board.mk

# project source
SOURCES  = simple_host.c
INCLUDES = . 
DEFINES  = __forceinline=inline NO_DEVICE
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
SOURCES  += $(wildcard $(TUSB_PATH)/../class/tusbh*.c)
SOURCES  += $(wildcard $(TUSB_PATH)/../class/msc/tusbh*.c)
SOURCES  += $(wildcard $(TUSB_PATH)/../class/cdc/tusbh*.c)
SOURCES  += $(wildcard $(TUSB_PATH)/../class/hid/tusbh*.c)
SOURCES  += $(wildcard $(TUSB_PATH)/../class/user/tusbh*.c)

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

all: f723

f723:
	@echo Building stm32f723iek6
	@$(MAKE) demo TARGET='f723host' BOARD='stm32f723e_disco'

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

clean:
	$(RM) $(OUTDIR)/* $(OBJDIR)/*
clear:
	$(RM) $(OUTDIR) obj

.PHONY: all clean clear

-include $(DEPENDS)
