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

#define  HOST_PORT_POWER_ON_HS() \
do{\
  __HAL_RCC_GPIOH_CLK_ENABLE();\
  GPIOH->MODER &= ~(GPIO_MODER_MODER0 << (12*2));\
  GPIOH->MODER |= (GPIO_MODER_MODER0_0 << (12*2));\
  GPIOH->BSRR = GPIO_PIN_12;\
}while(0)

#define  HOST_PORT_POWER_OFF_HS() \
do{\
  __HAL_RCC_GPIOH_CLK_ENABLE();\
  GPIOH->MODER &= ~(GPIO_MODER_MODER0 << (12*2));\
  GPIOH->MODER |= (GPIO_MODER_MODER0_0 << (12*2));\
  GPIOH->BSRR = GPIO_PIN_12<<16;\
}while(0)


#define GET_HOST_PORT_ID_HS() (GPIOB->IDR & GPIO_PIN_12)
#define SETUP_HOST_PORT_ID_HS() \
do{\
    __HAL_RCC_GPIOB_CLK_ENABLE();\
    GPIOB->MODER &= ~(GPIO_MODER_MODER0 << (12*2));\
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (12*2));\
    GPIOB->PUPDR |=  (GPIO_PUPDR_PUPDR0_0 << (12*2));\
}while(0)
    
    

#define  HOST_PORT_POWER_ON_FS() \
do{\
  __HAL_RCC_GPIOG_CLK_ENABLE();\
  GPIOG->MODER &= ~(GPIO_MODER_MODER0 << (8*2));\
  GPIOG->MODER |= (GPIO_MODER_MODER0_0 << (8*2));\
  GPIOG->BSRR = GPIO_PIN_8<<16;\
}while(0)

#define  HOST_PORT_POWER_OFF_FS() \
do{\
  __HAL_RCC_GPIOG_CLK_ENABLE();\
  GPIOG->MODER &= ~(GPIO_MODER_MODER0 << (8*2));\
  GPIOG->MODER |= (GPIO_MODER_MODER0_0 << (8*2));\
  GPIOG->BSRR = GPIO_PIN_8;\
}while(0)


#define GET_HOST_PORT_ID_FS() (GPIOA->IDR & GPIO_PIN_10)
#define SETUP_HOST_PORT_ID_FS() \
do{\
    __HAL_RCC_GPIOA_CLK_ENABLE();\
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 << (10*2));\
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (10*2));\
    GPIOA->PUPDR |=  (GPIO_PUPDR_PUPDR0_0 << (10*2));\
}while(0)



#define  HOST_PORT_POWER_ON()  HOST_PORT_POWER_ON_HS()

// init the stdio hardware
void stdio_init(void);
// stdin recv char handler
void stdin_recvchar(int ch);
// stdout send char handler 
void stdout_sendchar(int ch);


#endif

