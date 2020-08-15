/*       
 *         _______                    _    _  _____ ____  
 *        |__   __|                  | |  | |/ ____|  _ \ 
 *           | | ___  ___ _ __  _   _| |  | | (___ | |_) |
 *           | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ < 
 *           | |  __/  __/ | | | |_| | |__| |____) | |_) |
 *           |_|\___|\___|_| |_|\__, |\____/|_____/|____/ 
 *                               __/ |                    
 *                              |___/                     
 *
 * TeenyUSB - light weight usb stack for micro controllers
 * 
 * Copyright (c) 2020 XToolBox  - admin@xtoolbox.org
 *                         www.tusb.org
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#if defined(USE_RTTHREAD)
#include <rtthread.h>
#define RTOS_INTERRUPT_ENTER()  rt_interrupt_enter()
#define RTOS_INTERRUPT_LEAVE()  rt_interrupt_leave()
#define TUSB_PRINTF  rt_kprintf
#define TUSB_HAS_OS
#endif

    /**USB_OTG_HS GPIO Configuration    
    PA3     ------> USB_OTG_HS_ULPI_D0
    PB0     ------> USB_OTG_HS_ULPI_D1
    PB1     ------> USB_OTG_HS_ULPI_D2
    PB10     ------> USB_OTG_HS_ULPI_D3
    PB11     ------> USB_OTG_HS_ULPI_D4
    PB12     ------> USB_OTG_HS_ULPI_D5
    PB13     ------> USB_OTG_HS_ULPI_D6
    PB5     ------> USB_OTG_HS_ULPI_D7 
    PI11     ------> USB_OTG_HS_ULPI_DIR
    PC0     ------> USB_OTG_HS_ULPI_STP
    PH4     ------> USB_OTG_HS_ULPI_NXT
    PA5     ------> USB_OTG_HS_ULPI_CK
    */
#define  OTG_HS_ULPI_IO_CLK_ENABLE() \
do { \
  __IO uint32_t tmpreg = 0x00U; \
  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN  \
                       |RCC_AHB1ENR_GPIOBEN  \
                       |RCC_AHB1ENR_GPIOCEN  \
                       |RCC_AHB1ENR_GPIOHEN  \
                       |RCC_AHB1ENR_GPIOIEN);\
  /* Delay after an RCC peripheral clock enabling */ \
  tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);\
  UNUSED(tmpreg); \
} while(0U)

#define  OTG_HS_ULPI_D0    GPIOA,  3, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_D1    GPIOB,  0, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_D2    GPIOB,  1, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_D3    GPIOB, 10, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_D4    GPIOB, 11, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_D5    GPIOB, 12, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_D6    GPIOB, 13, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_D7    GPIOB,  5, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_DIR   GPIOI, 11, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_STP   GPIOC,  0, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_NXT   GPIOH,  4, GPIO_AF10_OTG_HS
#define  OTG_HS_ULPI_CK    GPIOA,  5, GPIO_AF10_OTG_HS


#define  HOST_PORT_POWER_ON()

#define  HOST_PORT_POWER_ON_HS()

#define  HOST_PORT_POWER_ON_FS() \
do{\
  __HAL_RCC_GPIOC_CLK_ENABLE();\
  GPIOC->MODER &= ~(GPIO_MODER_MODER0 << (1*2));\
  GPIOC->MODER |= (GPIO_MODER_MODER0_0 << (1*2));\
  GPIOC->BSRR = GPIO_PIN_1<<16;\
}while(0)

// init the stdio hardware
void stdio_init(void);
// stdin recv char handler
void stdin_recvchar(int ch);
// stdout send char handler 
void stdout_sendchar(int ch);

#endif

