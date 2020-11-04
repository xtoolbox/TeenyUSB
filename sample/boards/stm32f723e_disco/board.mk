CPU = cortex-m7

DEV_TYPE = STM32F7xx
BOARD_INC = $(BOARD_PATH) $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Inc $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/$(DEV_TYPE)/Include
BOARD_DEF = USE_HAL_DRIVER STM32F723xx HSE_VALUE=25000000
BOARD_SRC = $(BOARD_PATH)/system_stm32f7xx_HSE25MHz.c $(BOARD_PATH)/board.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32f7xx_ll_usb.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32f7xx_hal_pcd.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32f7xx_hal_pcd_ex.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32f7xx_hal_hcd.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f723xx.s
LDSCRIPT = $(BOARD_PATH)/STM32F723IEKx_FLASH.ld

