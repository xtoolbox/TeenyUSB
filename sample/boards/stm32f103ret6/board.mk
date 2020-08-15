CPU = cortex-m3

DEV_TYPE = STM32F1xx
BOARD_INC = $(BOARD_PATH) $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Inc $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/$(DEV_TYPE)/Include
BOARD_DEF = USE_HAL_DRIVER STM32F103xE HSE_VALUE=8000000
BOARD_SRC = $(BOARD_PATH)/system_stm32f10x.c board.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xe.s
LDSCRIPT = $(BOARD_PATH)/STM32F103RETx_FLASH.ld

