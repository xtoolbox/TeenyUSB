CPU = cortex-m0

DEV_TYPE = STM32F0xx
BOARD_INC = $(BOARD_PATH) $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Inc $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/$(DEV_TYPE)/Include
BOARD_DEF = USE_HAL_DRIVER STM32F072xB HSE_VALUE=8000000
BOARD_SRC = $(BOARD_PATH)/system_stm32f0xx.c board.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f072xb.s
LDSCRIPT = $(BOARD_PATH)/STM32F072C8Tx_FLASH.ld

