CPU = cortex-m7

DEV_TYPE = STM32H7xx
BOARD_INC = $(BOARD_PATH) $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Inc $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/$(DEV_TYPE)/Include
BOARD_DEF = USE_HAL_DRIVER STM32H743xx HSE_VALUE=12000000
BOARD_SRC = $(BOARD_PATH)/system_stm32h7xx_HSI.c board.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32h7xx_ll_usb.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32h7xx_hal_pcd.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32h7xx_hal_pcd_ex.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/$(DEV_TYPE)_HAL_Driver/Src/stm32h7xx_hal_eth.c
BOARD_SRC += $(TUSB_PATH)/../mcu_lib/st_driver/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/startup_stm32h743xx.s
LDSCRIPT = $(BOARD_PATH)/STM32H743ZITx_FLASH.ld

