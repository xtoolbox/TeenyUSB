CPU = cortex-m4

DEV_TYPE = STM32F4xx
BOARD_INC = $(BOARD_PATH) $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Inc $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/$(DEV_TYPE)/Include
BOARD_DEF = USE_HAL_DRIVER STM32F407xx HSE_VALUE=8000000
BOARD_SRC = $(BOARD_PATH)/system_stm32f4xx.c board.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32f4xx_ll_usb.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32f4xx_hal_pcd.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s
LDSCRIPT = $(BOARD_PATH)/STM32F407VGTx_FLASH.ld

